/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol, support USB Open DMX and USB RS485 dongle.
	+ Art-Net protocol, support UDP local network broadcast.
	(c) CC 2023, MIT

	MMTPLUGINx* DLL (LightKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2ipdef.h>

namespace Common {
	namespace LIGHT {

		typedef asio::ip::address_v4 IpAddr;

		DMXArtnet::DMXArtnet(std::function<DMXPacket()> f) : DMXClassInterface(f) {
		}
		DMXArtnet::~DMXArtnet() {
			dispose_();
		}
		void DMXArtnet::dispose_() {
			try {
				if (wbg_id)
					wbg_id = worker_background::Get().remove(wbg_id);
				if (isrun_)
					udp_.stop();
				isrun_ = isready_ = false;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		bool DMXArtnet::reconnect_() {
			if (udp_.is_open()) return true;
			try {
				isrun_ = isready_ = false;
				ArtnetConfig cnf = config_;
				return Start(cnf);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}

		ArtnetPacketType DMXArtnet::GetPacketType() const {
			return packet_.PacketType;
		}
		void DMXArtnet::SetPacketType(ArtnetPacketType t) {
			packet_.PacketType = t;
		}

		bool DMXArtnet::IsRun() {
			return isrun_.load() && isready_.load();
		}
		bool DMXArtnet::Start(ArtnetConfig& cnf) {
			if (isrun_) {
				if (isready_) return true;
				dispose_();
			}
			isrun_ = true;

			try {
				config_ = cnf;
				auto f = std::async(std::launch::async, [=]() -> bool {
					try {
						return udp_.start(config_);
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
					return false;
				});
				isrun_ = isready_ = f.get();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return isready_;
		}
		void DMXArtnet::Stop() {
			dispose_();
		}
		bool DMXArtnet::Send(DMXPacket p, bool) {
			if (!IsRun()) return false;
			try {
				std::vector<byte> v = packet_.create(p, config_.universe);
				bool b = udp_.send(&v[0], v.size());
				if (!b) return reconnect_();
				return b;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return reconnect_();
		}
		void DMXArtnet::Send(std::vector<byte>& v) {
			if (!IsRun()) return;
			try {
				if (!udp_.send(&v[0], v.size()))
					reconnect_();
				return;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			reconnect_();
		}
		bool DMXArtnet::Send_async(DMXPacket p, DWORD id, bool dup) {
			if (!IsRun() || (pkt_id_ >= id)) return false;
			try {
				pkt_id_ = id;
				auto f = std::async(std::launch::async, [=](DMXPacket p_) -> bool {
					std::unique_lock<std::mutex> lock(mutex_);
					if (pkt_id_ >= id) return true;
					return Send(p_, dup);
				}, p);
				return f.get();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return reconnect_();
		}
		std::vector<byte> DMXArtnet::Receive() {
			throw make_common_error(L"Not implemented yet!");
		}

		ArtnetConfigs& DMXArtnet::GetInterfaces(bool isrescan) {
			if (isrescan || interfaces_.empty()) {
				PIP_ADAPTER_ADDRESSES paddr = nullptr;
				try {
						DWORD paddrsize = 1 << 19;
						PIP_ADAPTER_ADDRESSES paddr = (PIP_ADAPTER_ADDRESSES) new uint8_t[paddrsize];

						ULONG r = ::GetAdaptersAddresses(
							AF_UNSPEC,
							GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_DNS_SERVER,
							0, paddr, &paddrsize);

						if (r == NO_ERROR) {
							for (PIP_ADAPTER_ADDRESSES addr = paddr; addr != 0; addr = addr->Next) {
								if ((addr->OperStatus != IfOperStatusUp) || addr->NoMulticast) continue;
								if (addr->Ipv4Enabled) {
									for (PIP_ADAPTER_UNICAST_ADDRESS_LH maddr = addr->FirstUnicastAddress; maddr != 0; maddr = maddr->Next) {

										SOCKET_ADDRESS sa = maddr->Address;
										if (sa.lpSockaddr->sa_family != AF_INET) continue;

										uint8_t pref = maddr->OnLinkPrefixLength;
										IN_ADDR cmask{};
										cmask.S_un.S_addr = pref ? (0xFFFFFFFF >> (32 - pref)) : 0;

										IpAddr ip = asio::ip::make_address_v4(::ntohl(reinterpret_cast<sockaddr_in*>(sa.lpSockaddr)->sin_addr.s_addr));
										IpAddr mask = asio::ip::make_address_v4(::ntohl(cmask.S_un.S_addr));
										IpAddr bcast = IpAddr::broadcast(ip, mask);

										interfaces_.add(ip.to_string(), mask.to_string(), bcast.to_string());
									}
								}
							}
						}
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				if (paddr)
					try { delete [] paddr; } catch (...) {}
			}
			return std::ref(interfaces_);
		}
		ArtnetConfig& DMXArtnet::GetInterface() {
			return std::ref(config_);
		}
	}
}