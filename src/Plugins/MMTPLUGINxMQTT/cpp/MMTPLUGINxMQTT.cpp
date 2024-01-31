/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (SmartHomeKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include "..\lib\h\includelib.h"

namespace Common {
	namespace PLUGINS {

		#include "plugininfo.h"

		using namespace Common::IO;
		using namespace std::placeholders;

		MQTTPlugin::MQTTPlugin(std::wstring path, HWND hwnd)
			: plugin_ui_(*static_cast<PluginCb*>(this), hwnd),
			  IO::Plugin(
				Utils::to_hash(path), DLG_PLUG_MQTT_WINDOW,
				Plugin::GuidFromString(_PLUGINGUID),
				path,
				PLUG_HEADER,
				PLUG_DESCRIPTION,
				this,
				IO::PluginClassTypes::ClassMqttKey,
				(IO::PluginCbType::Out2Cb | IO::PluginCbType::LogCb | PluginCbType::LogsCb | IO::PluginCbType::ConfCb),
				hwnd
			  ) {
			PluginCb::out2_cb_ = std::bind(static_cast<void(MQTTPlugin::*)(MIDI::MidiUnit&, DWORD)>(&MQTTPlugin::cb_out_call_), this, _1, _2);
			PluginCb::cnf_cb_  = std::bind(static_cast<void(MQTTPlugin::*)(std::shared_ptr<JSON::MMTConfig>&)>(&MQTTPlugin::set_config_cb_), this, _1);
		}
		MQTTPlugin::~MQTTPlugin() {
			dispose_();
		}
		void MQTTPlugin::dispose_() {
			TRACE_CALL();
			try {
				if (broker_) {
					broker_->reset();
					broker_.reset();
				}
				export_list_.clear();

			} catch (...) {}
			is_config_ = is_enable_ = is_started_ = false;
		}
		void MQTTPlugin::load_(MQTT::BrokerConfig<std::wstring>& bc) {
			is_config_ = !bc.empty();
			is_enable_ = (is_config_ && bc.enable);
		}

		#pragma region private call cb
		void MQTTPlugin::set_config_cb_(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALL();
			try {
				if (!is_started_) {
					(void) load(mmt);
					#if defined (_DEBUG_CONFIG)
					::OutputDebugStringW(mmt->mqttconf.dump().c_str());
					#endif
					return;
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}
		void MQTTPlugin::cb_out_call_(MIDI::MidiUnit& m, DWORD t) {
			try {
				if ((t <= time_.load(std::memory_order_acquire)) ||
					(m.target != MIDI::Mackie::Target::MQTTKEY) ||
					 !broker_ || !broker_->isrunning()) return;

				time_.store(t, std::memory_order_release);

				switch (m.type) {
					using enum MIDI::MidiUnitType;
					case BTN:
					case BTNTOGGLE: {
						broker_->send<bool>(m.value.lvalue, m.longtarget);
						break;
					}
					case FADER:
					case FADERINVERT:
					case SLIDER:
					case SLIDERINVERT:
					case KNOB:
					case KNOBINVERT: {
						broker_->send<uint32_t>(static_cast<uint32_t>(m.value.value), m.longtarget);
						break;
					}
					default: break;
				}
			} catch (...) {}
		}
		#pragma endregion

		#pragma region public actions
		bool MQTTPlugin::load(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->empty()));
			try {
				load_(mmt->mqttconf);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return true;
		}
		bool MQTTPlugin::load(std::wstring s) {
			TRACE_CALLX(s);
			try {
				if (s.empty()) return true;

				worker_background::Get().to_async(std::async(std::launch::async, [=](std::wstring p) {
					try {
						MQTT::BrokerConfig<std::wstring> bc{};
						JSON::json_config jc{};

						is_enable_ = jc.ReadFile(p,
							[&](Tiny::TinyJson& root, std::wstring& path) -> bool {
								jc.ReadMqttConfig(root, bc);
								return true;
							}
						);
						if (is_enable_) load_(bc);
						if (is_enable_ && is_config_)
							common_config::Get().GetConfig()->mqttconf.Copy(bc);

					} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
				}, s));

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return true;
		}

		bool MQTTPlugin::start(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->empty()));
			try {
				if (is_started_) dispose_();

				load_(mmt->mqttconf);
				if (!is_config_ || !is_enable_) return false;

				broker_.reset(new MQTT::Broker());

				if (broker_->init(mmt->mqttconf))
					is_started_ = broker_->isrunning();

				if (is_started_) {
					PluginCb::ToLogRef(log_string().to_log_string(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MQTT_START))
					);
					worker_background::Get().to_async(
						std::async(std::launch::async, [=](MQTT::BrokerConfig<std::wstring>& c, std::vector<MIDI::MidiUnit>& u) {
							try {
								log_string ls{};
								ls << L"mqtt://";

								if (!c.login.empty()) {
									ls << c.login;
									if (!c.password.empty()) ls << L":***@";
									else ls << L"@";
								}
								if (!c.host.empty()) ls << c.host;
								else ls << L"127.0.0.1";

								if (c.port > 0U) ls << L":" << c.port;
								export_list_.push_back(std::make_pair(0, ls.str()));

							} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
							try {
								for (auto& a : u)
									if (a.target == MIDI::Mackie::Target::MQTTKEY)
										broker_->settitle(a.longtarget);

							} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }

						}, std::ref(mmt->mqttconf), std::ref(mmt->units))
					);
				}
				return is_started_;

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
		bool MQTTPlugin::stop() {
			TRACE_CALL();
			try {
				if (is_started_)
					PluginCb::ToLogRef(log_string().to_log_string(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MQTT_STOP))
					);
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			dispose_();
			return true;
		}
		void MQTTPlugin::release() {
			TRACE_CALL();
			dispose_();
		}

		IO::PluginUi& MQTTPlugin::GetPluginUi() {
			return std::ref(*plugin_ui_.GetPluginUi());
		}
		#pragma endregion

		std::vector<std::pair<uint16_t, std::wstring>>& MQTTPlugin::GetDeviceList() {
			TRACE_CALL();
			return std::ref(export_list_);
		}
		std::any MQTTPlugin::GetDeviceData() {
			std::any a = (int)42;
			return a;
		}
	}
}