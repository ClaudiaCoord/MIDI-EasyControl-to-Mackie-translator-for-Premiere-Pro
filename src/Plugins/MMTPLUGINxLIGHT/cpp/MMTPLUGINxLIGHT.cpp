/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (LightKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#pragma comment(lib, "iphlpapi.lib")

namespace Common {
	namespace PLUGINS {

		#include "plugininfo.h"

		using namespace Common::IO;
		using namespace std::placeholders;

		LightsPlugin::LightsPlugin(std::wstring path, HWND hwnd)
			: plugin_ui_(
				*static_cast<PluginCb*>(this),
				[=]() -> LIGHT::SerialPortConfigs& { return dmx_->GetDivices(); },
				[=]() -> LIGHT::ArtnetConfigs& { return artnet_->GetInterfaces(); },
				hwnd
			  ),
			  IO::Plugin(
				Utils::to_hash(path), DLG_PLUG_LIGHT_WINDOW,
				Plugin::GuidFromString(_PLUGINGUID),
				path,
				PLUG_HEADER,
				PLUG_DESCRIPTION,
				this,
				IO::PluginClassTypes::ClassLightKey,
				(IO::PluginCbType::Out2Cb | IO::PluginCbType::LogCb | IO::PluginCbType::LogsCb | IO::PluginCbType::ConfCb),
				hwnd
			  ) {

			PluginCb::out2_cb_ = std::bind(static_cast<void(LightsPlugin::*)(MIDI::MidiUnit&, DWORD)>(&LightsPlugin::cb_out_call_), this, _1, _2);
			PluginCb::cnf_cb_ = std::bind(static_cast<void(LightsPlugin::*)(std::shared_ptr<JSON::MMTConfig>&)>(&LightsPlugin::set_config_cb_), this, _1);

			auto pkt = std::bind(&LightsPlugin::getDMXPacket_, this);
			lock_ = std::make_shared<locker_awaiter>();
			dmx_ = std::make_unique<LIGHT::DMXSerial>(pkt);
			artnet_ = std::make_unique<LIGHT::DMXArtnet>(pkt);
		}
		LightsPlugin::~LightsPlugin() {
			dispose_();
		}
		void LightsPlugin::dispose_() {
			TRACE_CALL();
			try {

				dmx_pause_.store(true, std::memory_order_release);
				dmx_pool_active_.store(false, std::memory_order_release);

				if (dmx_->IsRun()) {
					dmx_->Stop();
					PluginCb::ToLogRef(log_string().to_log_string(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_LIGHT_DMX_STOP)
					));
				}
				if (artnet_->IsRun()) {
					artnet_->Stop();
					PluginCb::ToLogRef(log_string().to_log_string(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_LIGHT_ARTNET_STOP)
					));
				}

				export_list_.clear();

			} catch (...) {}
			is_config_ = is_enable_ = is_started_ = false;
		}
		void LightsPlugin::load_(LIGHT::LightsConfig& lc) {
			is_config_ = !lc.dmxconf.empty() || !lc.artnetconf.empty();
			is_enable_ = (is_config_ && (lc.dmxconf.enable || lc.artnetconf.enable));
		}
		LIGHT::DMXPacket LightsPlugin::getDMXPacket_() {
			if (lock_->IsLock()) return LIGHT::DMXPacket();
			return dmx_packet_;
		}
		void LightsPlugin::poolDMXPacket_() {
			try {
				std::thread t([=]() {
					dmx_pool_active_.store(true, std::memory_order_release);
					const long tm = (dmx_->IsRun() && artnet_->IsRun()) ? 50 : 75;
					while (dmx_pool_active_.load(std::memory_order_acquire)) {
						try {
							if (dmx_pause_.load(std::memory_order_acquire)) {
								std::this_thread::sleep_for(std::chrono::milliseconds(100));
								continue;
							}
							std::vector<byte> v = dmx_packet_.create();
							if (!dmx_pool_active_.load(std::memory_order_acquire)) break;
							if (dmx_->IsRun()) (void) dmx_->Send(v);
							if (!dmx_pool_active_.load(std::memory_order_acquire)) break;
							if (artnet_->IsRun()) (void) artnet_->Send(v);
							if (!dmx_pool_active_.load(std::memory_order_acquire)) break;
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

		#pragma region private call cb
		void LightsPlugin::set_config_cb_(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALL();
			try {
				if (!is_started_) {
					(void) load(mmt);
					#if defined (_DEBUG_CONFIG)
					::OutputDebugStringW(mmt->lightconf.dump().c_str());
					#endif
					return;
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}
		void LightsPlugin::cb_out_call_(MIDI::MidiUnit& m, DWORD t) {
			try {
				if (m.empty() ||
					dmx_pause_.load(std::memory_order_acquire) ||
					(packet_id_.load(std::memory_order_acquire) >= t)) return;

				packet_id_ = t;
				uint8_t val = m.value.value;
				switch (val) {
					case 0:
					case 127: {
						if (lock_->IsCanceled()) return;
						break;
					}
					default: {
						if (lock_->IsLock() || lock_->IsOnlyOne() || lock_->IsCanceled()) return;
						break;
					}
				}

				locker_auto locker(lock_, locker_auto::LockType::TypeLockOnlyOne);
				if (!locker.Begin()) return;

				uint8_t target = m.target;
				uint16_t did = static_cast<uint16_t>(m.longtarget);

				if ((m.type == MIDI::MidiUnitType::BTN) || (m.type == MIDI::MidiUnitType::BTNTOGGLE))
					val = (dmx_packet_.get_value(did) > 0U) ? 0U : 255U;
				else
					val = (val == 127U) ? 255U : ((val <= 127U) ? (val * 2) : val);
				val = (val <= 4U) ? 0U : val; /* correct end input */

				switch (target) {
					using enum MIDI::Mackie::Target;
					case LIGHTKEY8B: {
						dmx_packet_.set_value8(did, val);
						break;
					}
					case LIGHTKEY16B: {
						dmx_packet_.set_value16(did, val);
						break;
					}
					default: return;
				}

				#if defined(_DEBUG_PRINT)
				{
					log_string ls{};
					auto& data = dmx_packet_.get_data();
					for (uint32_t i = 0; i < 11; i++)
						ls << data[i] << L"|";
					ls << L" + " << m.value.value << L"|" << val << L"\n";
					::OutputDebugStringW(ls.str().c_str());
				}
				#endif

				if (dmx_pool_active_.load(std::memory_order_acquire) || dmx_pause_.load(std::memory_order_acquire)) return;

				if (locker.IsCanceled()) return;
				if (dmx_->IsRun()) (void) dmx_->Send(dmx_packet_, ((val == 255U) || (val == 0U)));
				if (locker.IsCanceled()) return;
				if (artnet_->IsRun()) (void) artnet_->Send(dmx_packet_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region public actions
		bool LightsPlugin::load(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->empty()));
			try {
				load_(mmt->lightconf);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return true;
		}
		bool LightsPlugin::load(std::wstring s) {
			TRACE_CALLX(s);
			try {
				if (s.empty()) return true;

				worker_background::Get().to_async(std::async(std::launch::async, [=](std::wstring p) {
					try {
						LIGHT::LightsConfig lc{};
						JSON::json_config jc{};

						is_enable_ = jc.ReadFile(p,
							[&](Tiny::TinyJson& root, std::wstring& path) -> bool {
								jc.ReadLightConfig(root, lc);
								return true;
							}
						);
						is_config_ = !lc.empty();

						if (is_enable_ && is_config_)
							common_config::Get().GetConfig()->lightconf.Copy(lc);

					} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
				}, s));

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return true;
		}

		bool LightsPlugin::start(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->empty()));
			try {

				dispose_();
				load_(mmt->lightconf);
				if (mmt->empty() || !is_config_ || !is_enable_) return false;

				LIGHT::ArtnetConfig& artconf = mmt->lightconf.artnetconf;
				LIGHT::SerialPortConfig& dmxconf = mmt->lightconf.dmxconf;
				bool r[]{ false, false };

				#pragma region DMX Serial
				if (dmxconf.enable) {
					if (!dmxconf.empty()) {
						auto& devs = dmx_->GetDivices().get();
						for (auto& a : devs) {
							if (a.port == dmxconf.port) {
								r[0] = true; break;
							}
						}
						if (r[0]) {
							r[0] = dmx_->Start(dmxconf);
							if (r[0]) {
								PluginCb::ToLogRef(log_string().to_log_format(
									__FUNCTIONW__,
									common_error_code::Get().get_error(common_error_id::err_LIGHT_DMX_START),
									dmxconf.port, dmxconf.name.c_str()
								));
								try {
									log_string ls{};
									ls << L"COM" << dmxconf.port << L": " << dmxconf.name.c_str();
									export_list_.push_back(std::make_pair(0, ls.str()));
								} catch (...) {}
							}
						}
						else
							PluginCb::ToLogRef(log_string().to_log_format(
								__FUNCTIONW__,
								common_error_code::Get().get_error(common_error_id::err_LIGHT_DMX_NOT_COM),
								dmxconf.port
							));
					}
					else
						PluginCb::ToLogRef(log_string().to_log_string(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_LIGHT_DMX_NOT_CONFIG)
						));
				}
				#pragma endregion

				#pragma region Art NET UDP
				if (artconf.enable) {
					if (!artconf.empty()) {
						auto& ifaces = artnet_->GetInterfaces().get();
						for (auto& a : ifaces) {
							if (std::equal(artconf.broadcast.begin(), artconf.broadcast.end(), a.broadcast.begin())) {
								r[1] = true; break;
							}
						}
						if (r[1]) {
							r[1] = artnet_->Start(artconf);
							if (r[1]) {
								PluginCb::ToLogRef(log_string().to_log_format(
									__FUNCTIONW__,
									common_error_code::Get().get_error(common_error_id::err_LIGHT_ARTNET_START),
									artconf.broadcast.c_str(), artconf.port, artconf.ip.c_str()
								));
								try {
									log_string ls{};
									ls << artconf.broadcast.c_str() << L":" << artconf.port;
									export_list_.push_back(std::make_pair(1, ls.str()));
								} catch (...) {}
							}
						}
						else
							PluginCb::ToLogRef(log_string().to_log_format(
								__FUNCTIONW__,
								common_error_code::Get().get_error(common_error_id::err_LIGHT_ARTNET_NOT_IFACE),
								artconf.ip.c_str()
							));
					}
					else
						PluginCb::ToLogRef(log_string().to_log_string(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_LIGHT_ARTNET_NOT_CONFIG)
						));
				}
				#pragma endregion

				if (r[0] || r[1]) {
					if (mmt->lightconf.ispool)
						poolDMXPacket_();
					is_started_ = true;
					dmx_pause_.store(false, std::memory_order_release);
				}
				return is_started_;

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
		bool LightsPlugin::stop() {
			TRACE_CALL();
			dispose_();
			return true;
		}
		void LightsPlugin::release() {
			TRACE_CALL();
			dispose_();
		}

		IO::PluginUi& LightsPlugin::GetPluginUi() {
			return std::ref(*plugin_ui_.GetPluginUi());
		}
		#pragma endregion

		std::vector<std::pair<uint16_t, std::wstring>>& LightsPlugin::GetDeviceList() {
			TRACE_CALL();
			return std::ref(export_list_);
		}
		std::any LightsPlugin::GetDeviceData() {
			std::any a = (int)42;
			return a;
		}
	}
}