/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (MultimediaKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace PLUGINS {

		#include "plugininfo.h"

		using namespace Common::IO;
		using namespace std::placeholders;

		MMKeysPlugin::MMKeysPlugin(std::wstring path, HWND hwnd)
			: plugin_ui_(*static_cast<PluginCb*>(this), hwnd),
			  IO::Plugin(
				Utils::to_hash(path), DLG_PLUG_MMKEY_WINDOW,
				Plugin::GuidFromString(_PLUGINGUID),
				path,
				PLUG_HEADER,
				PLUG_DESCRIPTION,
				this,
				IO::PluginClassTypes::ClassMediaKey,
				(IO::PluginCbType::Out2Cb | IO::PluginCbType::LogCb | PluginCbType::LogsCb | IO::PluginCbType::ConfCb),
				hwnd
			  ) {
			PluginCb::out2_cb_ = std::bind(static_cast<void(MMKeysPlugin::*)(MIDI::MidiUnit&, DWORD)>(&MMKeysPlugin::cb_out_call_), this, _1, _2);
			PluginCb::cnf_cb_ = std::bind(static_cast<void(MMKeysPlugin::*)(std::shared_ptr<JSON::MMTConfig>&)>(&MMKeysPlugin::set_config_cb_), this, _1);
		}
		MMKeysPlugin::~MMKeysPlugin() {
			dispose_();
		}
		void MMKeysPlugin::dispose_() {
			TRACE_CALL();
			started_.store(false, std::memory_order_release);
			is_config_ = is_enable_ = is_started_ = false;
		}
		void MMKeysPlugin::load_(MIDI::MMKeyConfig& mc) {
			is_config_ = !mc.empty();
			is_enable_ = (is_config_ && mc.enable);
		}

		#pragma region private call cb
		void MMKeysPlugin::cb_out_call_(MIDI::MidiUnit& m, DWORD t) {
			if (!started_.load(std::memory_order_acquire)) return;
			try {
				if (((m.type  != MIDI::MidiUnitType::BTN) &&
					 (m.type  != MIDI::MidiUnitType::BTNTOGGLE)) ||
					(m.target != MIDI::Mackie::Target::MEDIAKEY)) return;

				KEYBDINPUT kbi{};

				switch (m.longtarget) {
					using enum MIDI::Mackie::Target;
					case SYS_Zoom:		{ kbi.wVk = VK_ZOOM; break; }
					case SYS_Scrub:		{ kbi.wVk = VK_LAUNCH_MEDIA_SELECT; break; }
					case SYS_Record:	{ kbi.wVk = VK_PLAY; break; }
					case SYS_Rewind:	{ kbi.wVk = VK_MEDIA_PREV_TRACK; break; }
					case SYS_Forward:	{ kbi.wVk = VK_MEDIA_NEXT_TRACK; break; }
					case SYS_Stop:		{ kbi.wVk = VK_MEDIA_STOP; break; }
					case SYS_Play:		{ kbi.wVk = VK_MEDIA_PLAY_PAUSE; break; }
					case SYS_Up:		{ kbi.wVk = VK_VOLUME_UP; break; }
					case SYS_Down:		{ kbi.wVk = VK_VOLUME_DOWN; break; }
					case SYS_Left:		{ kbi.wVk = VK_VOLUME_MUTE; break; }
					case SYS_Right:		{ kbi.wVk = VK_PAUSE; break; }
					default: return;
				}

				kbi.dwFlags = KEYEVENTF_EXTENDEDKEY;
				kbi.dwExtraInfo = (ULONG_PTR) ::GetMessageExtraInfo();

				INPUT i{};
				i.type = INPUT_KEYBOARD;
				i.ki = kbi;
				(void) ::SendInput(1, &i, sizeof(INPUT));

			} catch (...) {}
		}
		void MMKeysPlugin::set_config_cb_(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALL();
			try {
				load_(mmt->mmkeyconf);
				if (is_started_ && !is_enable_) {
					stop();
					#if defined (_DEBUG_CONFIG)
					::OutputDebugStringW(mmt->mmkeyconf.dump().c_str());
					#endif
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}
		#pragma endregion

		#pragma region public actions
		bool MMKeysPlugin::load(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->empty()));
			try {
				load_(mmt->mmkeyconf);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return true;
		}
		bool MMKeysPlugin::load(std::wstring s) {
			TRACE_CALLX(s);
			try {
				if (s.empty()) return true;

				worker_background::Get().to_async(std::async(std::launch::async, [=](std::wstring p) {
					try {
						MIDI::MMKeyConfig mc{};
						JSON::json_config jc{};

						is_enable_ = jc.ReadFile(p,
							[&](Tiny::TinyJson& root, std::wstring& path) -> bool {
								jc.ReadMMkeyConfig(root, mc);
								return true;
							}
						);
						if (is_enable_) load_(mc);
						if (is_config_ && is_enable_) common_config::Get().GetConfig()->mmkeyconf.copy(mc);

					} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
				}, s));

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return true;
		}

		bool MMKeysPlugin::start(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->mmkeyconf.empty()));
			try {
				load_(mmt->mmkeyconf);
				if (!is_config_ || !is_enable_) return false;

				is_started_ = true;
				started_.store(is_started_, std::memory_order_release);

				PluginCb::ToLogRef(log_string().to_log_string(__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_MMKEY_START)));

				return is_started_;

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
		bool MMKeysPlugin::stop() {
			TRACE_CALL();
			dispose_();

			PluginCb::ToLogRef(log_string().to_log_string(__FUNCTIONW__,
				common_error_code::Get().get_error(common_error_id::err_MMKEY_STOP)));

			return true;
		}
		void MMKeysPlugin::release() {
			TRACE_CALL();
			dispose_();
		}

		IO::PluginUi& MMKeysPlugin::GetPluginUi() {
			return std::ref(*plugin_ui_.GetPluginUi());
		}
		#pragma endregion

		IO::export_list_t& MMKeysPlugin::GetDeviceList() {
			TRACE_CALL();
			return std::ref(empty_list);
		}
		std::any MMKeysPlugin::GetDeviceData() {
			std::any a = (int)42;
			return a;
		}
	}
}