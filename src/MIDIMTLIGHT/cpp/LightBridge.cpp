/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol, support USB Open DMX and USB RS485 dongle.
	+ Art-Net protocol, support UDP local network broadcast.
	(c) CC 2023, MIT

	MIDIMTLIGHT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace LIGHT {

		using namespace std::placeholders;

		static LightBridge lightbridge__{};

		LightBridge::LightBridge() {
			pid_cb__ = [](std::wstring&) { return 0U; };
			type__ = MIDI::ClassTypes::ClassLightKey;
			out1__ = [](MIDI::Mackie::MIDIDATA&, DWORD&) { return false; };
			out2__ = std::bind(static_cast<const bool(LightBridge::*)(MIDI::MidiUnit&, DWORD&)>(&LightBridge::InCallBack), this, _1, _2);
			id__   = Utils::random_hash(this);
			lock__ = std::make_shared<locker_awaiter>();
			artnet__ = std::unique_ptr<DMXArtnet>(new DMXArtnet(std::bind(&LightBridge::getDMXPacket_, this)));
			dmxport__ = std::unique_ptr<DMXSerial>(new DMXSerial(std::bind(&LightBridge::getDMXPacket_, this)));
		}
		LightBridge::~LightBridge() {
			try {
				lock__->reset();
				lock__.reset();
				artnet__.reset();
				dmxport__.reset();
			} catch (...) {}
		}

		DMXPacket LightBridge::getDMXPacket_() {
			if (lock__->IsLock()) return DMXPacket();
			return dmx_packet__;
		}

		void LightBridge::BlackOut(bool b) {
			dmx_packet__.set_blackout(b);
		}
		bool LightBridge::IsBlackOut() {
			return dmx_packet__.is_blackout();
		}
		void LightBridge::DmxPause(bool b) {
			dmx_pause__ = b;
		}
		bool LightBridge::IsDmxPause() {
			return dmx_pause__.load();
		}

		bool LightBridge::Start() {
			try {
				bool r[]{ false, false };
				common_config& cnf = common_config::Get();
				auto& conf = cnf.GetConfig();

				if (conf->dmxconf.enable) {
					if (!conf->dmxconf.empty()) {
						auto& devs = dmxport__->GetDivices().get();
						for (auto& a : devs) {
							if (a.port == conf->dmxconf.port) {
								r[0] = true; break;
							}
						}
						if (r[0]) {
							r[0] = dmxport__->Start(conf->dmxconf);
							to_log::Get() << log_string().to_log_fomat(
								__FUNCTIONW__,
								common_error_code::Get().get_error(common_error_id::err_LIGHT_DMX_START),
								conf->dmxconf.port, conf->dmxconf.name.c_str()
							);
						} else
							to_log::Get() << log_string().to_log_fomat(
								__FUNCTIONW__,
								common_error_code::Get().get_error(common_error_id::err_LIGHT_DMX_NOT_COM),
								conf->dmxconf.port
							);
					} else
						to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_LIGHT_DMX_NOT_CONFIG));
				}
				if (conf->artnetconf.enable) {
					if (!conf->artnetconf.empty()) {
						auto& ifaces = artnet__->GetInterfaces().get();
						for (auto& a : ifaces) {
							if (std::equal(conf->artnetconf.broadcast.begin(), conf->artnetconf.broadcast.end(), a.broadcast.begin())) {
								r[1] = true; break;
							}
						}
						if (r[1]) {
							r[1] = artnet__->Start(conf->artnetconf);
							to_log::Get() << log_string().to_log_fomat(
								__FUNCTIONW__,
								common_error_code::Get().get_error(common_error_id::err_LIGHT_ARTNET_START),
								conf->artnetconf.broadcast.c_str(), conf->artnetconf.port, conf->artnetconf.ip.c_str()
							);
						} else
							to_log::Get() << log_string().to_log_fomat(
								__FUNCTIONW__,
								common_error_code::Get().get_error(common_error_id::err_LIGHT_ARTNET_NOT_COM),
								conf->artnetconf.ip.c_str()
							);
					} else
						to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_LIGHT_ARTNET_NOT_CONFIG));
				}
				if (r[0] || r[1]) {
					common_config::Get().Local.IsLightsRun(true);
					if (common_config::Get().Registry.GetDMXPollEnable())
						poolDMXPacket_();
					return true;
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		void LightBridge::Stop() {
			try {
				if (dmx_pool_active__) {
					dmx_pool_active__ = false;
					std::this_thread::sleep_for(std::chrono::milliseconds(300));
				}
				if (dmxport__->IsRun()) {
					dmxport__->Stop();
					to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_LIGHT_DMX_STOP));
				}
				if (artnet__->IsRun()) {
					artnet__->Stop();
					to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_LIGHT_ARTNET_STOP));
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			common_config::Get().Local.IsLightsRun(false);
		}
		LightBridge& LightBridge::Get() {
			return std::ref(lightbridge__);
		}
		void LightBridge::SetPidCb(callGetPid cb) {
			pid_cb__ = cb;
		}

		SerialPortConfigs& LightBridge::GetDivices(bool isrescan) {
			return dmxport__->GetDivices(isrescan);
		}
		ArtnetConfigs& LightBridge::GetInterfaces(bool isrescan) {
			return artnet__->GetInterfaces(isrescan);
		}

		/* Private */

		void LightBridge::poolDMXPacket_() {
			try {
				std::thread t([=]() {
					dmx_pool_active__ = true;
					const long tm = (dmxport__->IsRun() && artnet__->IsRun()) ? 50 : 75;
					while (dmx_pool_active__) {
						try {
							if (dmx_pause__) {
								std::this_thread::sleep_for(std::chrono::milliseconds(100));
								continue;
							}
							std::vector<byte> v = dmx_packet__.create();
							if (!dmx_pool_active__) break;
							if (dmxport__->IsRun()) (void) dmxport__->Send(v);
							if (!dmx_pool_active__) break;
							if (artnet__->IsRun()) (void) artnet__->Send(v);
							if (!dmx_pool_active__) break;
						} catch (...) {
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(tm));
					}
				});
				t.detach();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		const bool LightBridge::InCallBack(MIDI::MidiUnit& m, DWORD& d) {
			try {
				if (m.IsEmpty() || packet_id__ >= d) return false;
				packet_id__ = d;
				switch (m.value.value) {
					case 0:
					case 127: {
						if (lock__->IsCanceled()) return true;
						break;
					}
					default: {
						if (lock__->IsLock() || lock__->IsOnlyOne() || lock__->IsCanceled()) return true;
						break;
					}
				}

				locker_auto locker(lock__, locker_auto::LockType::TypeLockOnlyOne);
				if (locker.IsOnlyOne() || !locker.Begin() || locker.IsCanceled()) return true;

				bool r = false;
				uint8_t val = m.value.value;
				uint16_t devid = static_cast<uint16_t>(m.longtarget);

				if ((m.type == MIDI::MidiUnitType::BTN) || (m.type == MIDI::MidiUnitType::BTNTOGGLE))
					val = (dmx_packet__.get_value(devid) > 0U) ? 0U : 255U;
				else
					val = (val == 127U) ? 255U : ((val <= 127U) ? (val * 2) : val);
				val = (val <= 4U) ? 0U : val; /* correct end input */

				dmx_packet__.set_value(devid, val);

				#if defined(_DEBUG_PRINT)
				{
					log_string ls{};
					auto& data = dmx_packet__.get_data();
					for (uint32_t i = 0; i < 11; i++)
						ls << data[i] << L"|";
					ls << L" + " << m.value.value << L"|" << val << L"\n";
					::OutputDebugStringW(ls.str().c_str());
				}
				#endif

				if (dmx_pool_active__ || dmx_pause__) return true;

				if (locker.IsCanceled()) return true;
				if (dmxport__->IsRun()) r = dmxport__->Send(dmx_packet__, ((val == 255U) || (val == 0U)));
				if (locker.IsCanceled()) return true;
				if (artnet__->IsRun()) r = artnet__->Send(dmx_packet__);
				return r;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
	}
}
