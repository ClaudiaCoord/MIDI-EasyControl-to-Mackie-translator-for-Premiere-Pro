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

		#define ALOAD(A) A.load(std::memory_order_acquire)
		#define ASTORE(A,B) A.store(B, std::memory_order_release)
		#define TSLEEP(A) std::this_thread::sleep_for(std::chrono::milliseconds(A))

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
			dmx_ = std::make_unique<LIGHT::DMXSerial>(pkt);
			artnet_ = std::make_unique<LIGHT::DMXArtnet>(pkt);
		}
		LightsPlugin::~LightsPlugin() {
			dispose_();
		}
		void LightsPlugin::dispose_() {
			TRACE_CALL();
			try {

				ASTORE(dmx_pause_, true);
				ASTORE(dmx_pool_active_, false);

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
			return dmx_packet_;
		}
		void LightsPlugin::poolDMXPacket_() {
			try {
				std::jthread t([=]() {
					try {
						const long tm = (dmx_->IsRun() && artnet_->IsRun()) ? (ALOAD(dmx_pool_enable_) ? 25 : 10) : (ALOAD(dmx_pool_enable_) ? 50 : 15);
						const bool is_artnet{ artnet_->IsRun() };
						const bool is_dmx{ dmx_->IsRun() };

						if (!is_dmx && !is_artnet) return;
						ASTORE(dmx_pool_active_, true);

						while (ALOAD(dmx_pool_active_)) {
							try {
								bool is_resend_packet{ false };

								if (ALOAD(dmx_pause_)) {
									TSLEEP(100);
									continue;
								}
								if (queue_.empty()) {
									if (!ALOAD(dmx_pool_enable_)) {
										TSLEEP(tm);
										continue;
									}
								}
								else {
									try {
										auto& update = queue_.front();

										switch (update[0]) {
											case 8:		dmx_packet_.set_value8(update[1], static_cast<uint8_t>(update[2])); break;
											case 16:	dmx_packet_.set_value16(update[1], static_cast<uint8_t>(update[2])); break;
											default: {
												queue_.pop();
												continue;
											}
										}
										queue_.pop();
										is_resend_packet = (queue_.empty() && ((update[2] == 255) || (update[2] == 0)));

									} catch (...) {}
								}

								if (is_dmx) {
									std::vector<byte> v = dmx_packet_.create();
									std::vector<byte>& ref = std::ref(v);

									(void)dmx_->Send(ref);
									if (is_resend_packet) (void)dmx_->Send(ref);
								}
								if (is_artnet) {
									std::vector<byte> v = artnet_->CreateArtnetPacket(std::ref(dmx_packet_));
									std::vector<byte>& ref = std::ref(v);

									(void)artnet_->Send(ref);
									if (is_resend_packet) (void)artnet_->Send(ref);
								}

							} catch (...) {
								Utils::get_exception(std::current_exception(), __FUNCTIONW__);
							}

							if (!ALOAD(dmx_pool_active_)) break;
							if (queue_.empty())	TSLEEP(tm);
						}

					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
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
					ALOAD(dmx_pause_) ||
					(ALOAD(packet_id_) >= t)) return;

				ASTORE(packet_id_, t);
				uint16_t val = m.value.value;
				uint16_t id = static_cast<uint16_t>(m.longtarget);

				if ((m.type == MIDI::MidiUnitType::BTN) || (m.type == MIDI::MidiUnitType::BTNTOGGLE))
					val = (dmx_packet_.get_value(id) > 0) ? 0 : 255;
				else
					val = (val == 127) ? 255 : ((val <= 127) ? (val * 2) : val);
				val = (val <= 4) ? 0 : val; /* correct light barier, end input */

				queue_.emplace(
					dmx_update_t{
						(m.target == MIDI::Mackie::Target::LIGHTKEY8B) ? uint16_t(8) : uint16_t(16),
						id,
						val
					}
				);

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
							common_config::Get().GetConfig()->lightconf.copy(lc);

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
							if (artconf.broadcast._Equal(a.broadcast)) {
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
					ASTORE(dmx_pool_enable_, mmt->lightconf.ispool);
					queue_ = {};
					poolDMXPacket_();
					is_started_ = true;
					ASTORE(dmx_pause_, false);
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

		IO::export_list_t& LightsPlugin::GetDeviceList() {
			TRACE_CALL();
			return std::ref(export_list_);
		}
		std::any LightsPlugin::GetDeviceData() {
			std::any a = (int)42;
			return a;
		}
	}
}