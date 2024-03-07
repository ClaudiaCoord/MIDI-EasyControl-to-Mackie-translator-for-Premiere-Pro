/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Joystick support.
	+ GamePad support.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (GamePad/Joistick Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#pragma comment(lib, "winmm.lib")

namespace Common {
	namespace PLUGINS {

		#include "plugininfo.h"

		using namespace Common::IO;
		using namespace std::placeholders;
		using namespace std::string_view_literals;

		static constexpr std::wstring_view CLZ_ = L"PLUGINS.GamePadPlugin"sv;

		GamePadPlugin::GamePadPlugin(std::wstring path, HWND hwnd)
			: plugin_ui_(*static_cast<PluginCb*>(this), std::ref(jb_), hwnd),
			  IO::Plugin(
				Utils::to_hash(path), DLG_PLUG_GAMEPAD_WINDOW,
				Plugin::GuidFromString(_PLUGINGUID),
				path,
				PLUG_HEADER,
				PLUG_DESCRIPTION,
				this,
				IO::PluginClassTypes::ClassIn,
				(IO::PluginCbType::In2Cb | IO::PluginCbType::LogCb | PluginCbType::LogsCb | IO::PluginCbType::ConfCb),
				hwnd
			  ) {
			PluginCb::cnf_cb_ = std::bind(static_cast<void(GamePadPlugin::*)(std::shared_ptr<JSON::MMTConfig>&)>(&GamePadPlugin::set_config_cb_), this, _1);
			jb_.log_cb = [=](const std::wstring& s) { PluginCb::ToLog(s); };
			jb_.event_cb = [=](MIDI::Mackie::MIDIDATA m) { PluginCb::GetCbIn2()(m, 0); };
			worker_background::Get().to_async(std::async(std::launch::async, [=]() {
					jb_.init();
				})
			);
		}
		GamePadPlugin::~GamePadPlugin() {
			dispose_();
		}
		void GamePadPlugin::dispose_() {
			TRACE_CALL();
			jb_.stop();
			started_.store(false, std::memory_order_release);
			is_config_ = is_enable_ = is_started_ = false;
		}
		void GamePadPlugin::load_(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALL();
			if (!mmt) {
				is_config_ = is_enable_ = false;
				return;
			}
			is_config_ = !mmt->empty() && !mmt->gamepadconf.empty();
			is_enable_ = (is_config_ && IO::Plugin::mhwnd_ && mmt->gamepadconf.enable);

			if (is_config_ && is_enable_) {
				mmt->gamepadconf.rloglevel = mmt->remoteconf.loglevel;
				jb_.update(mmt);
			}
			else jb_.clear();
		}
		void GamePadPlugin::start_(GAMEPAD::JoystickConfig& cnf) {
			TRACE_CALLX(std::to_wstring((uint32_t)!cnf.empty()));
			try {
				UI::UiUtils::Post(IO::Plugin::mhwnd_, [=]() {

						HWND h = ::CreateWindowExW(WS_EX_NOACTIVATE,
							WC_STATICW, L"", SS_SIMPLE | WS_CHILD,
							127, 0, 127, 0, IO::Plugin::mhwnd_, (HMENU)101, ::GetModuleHandleW(nullptr), nullptr);
						if (!h) return;
						if (!(is_started_ = jb_.start(h, cnf))) return;

						for (auto& a : jb_.data)
							if (!a.is_offline)
								devices_list.push_back(
									std::make_pair(a.id, a.label())
								);

						started_.store(is_started_, std::memory_order_release);

						PluginCb::ToLog(log_string().to_log_format(CLZ_.data(),
							common_error_code::Get().get_error(common_error_id::err_JOY_START),
							jb_.StartDevices(), jb_.OnlineDevices()).str()
						);
					}
				);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		#pragma region private call cb
		void GamePadPlugin::set_config_cb_(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALL();
			try {
				load_(mmt);
				if (is_started_ && !is_enable_) {
					stop();
					#if defined (_DEBUG_CONFIG)
					::OutputDebugStringW(mmt->gamepadconf.dump().c_str());
					#endif
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}
		#pragma endregion

		#pragma region public actions
		bool GamePadPlugin::load(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->empty()));
			try {
				load_(mmt);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return true;
		}
		bool GamePadPlugin::load(std::wstring s) {
			TRACE_CALLX(s);
			try {
				if (s.empty()) return true;

				worker_background::Get().to_async(std::async(std::launch::async, [=](std::wstring p) {
					try {
						GAMEPAD::JoystickConfig mc{};
						JSON::json_config jc{};

						is_enable_ = jc.ReadFile(p,
							[&](Tiny::TinyJson& root, std::wstring& path) -> bool {
								jc.ReadGamepadConfig(root, mc);
								return true;
							}
						);
						if (is_enable_) {
							is_config_ = !mc.empty();
							is_enable_ = (is_config_ && (IO::Plugin::mhwnd_) && mc.enable);
						}
						if (is_config_ && is_enable_) common_config::Get().GetConfig()->gamepadconf.copy(mc);

					} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
				}, s));

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return true;
		}

		bool GamePadPlugin::start(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->gamepadconf.empty()));
			try {
				devices_list.clear();
				load_(mmt);
				if (!is_config_ || !is_enable_) return false;

				if (jb_.empty()) {
					auto f = std::async(std::launch::async, [=]() {
						try {
							return jb_.init();
						} catch (...) {
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						}
						return false;
					});

					is_enable_ = f.get();
					if (jb_.empty())
						return false;
				}

				start_(mmt->gamepadconf);
				return true;

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
		bool GamePadPlugin::stop() {
			TRACE_CALL();
			dispose_();

			PluginCb::ToLogRef(log_string().to_log_string(CLZ_.data(),
				common_error_code::Get().get_error(common_error_id::err_JOY_STOP)));

			return true;
		}
		void GamePadPlugin::release() {
			TRACE_CALL();
			dispose_();
		}

		IO::PluginUi& GamePadPlugin::GetPluginUi() {
			return std::ref(*plugin_ui_.GetPluginUi());
		}
		#pragma endregion

		IO::export_list_t& GamePadPlugin::GetDeviceList() {
			TRACE_CALL();
			return std::ref(devices_list);
		}
		std::any GamePadPlugin::GetDeviceData() {
			std::any a = (int)42;
			return a;
		}
	}
}