/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MEDIAKEYS DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MMKey {

		using namespace std::placeholders;

		static bool message_once__ = false;
		static MMKBridge mmkeysbridge__;

		MMKBridge::MMKBridge() {
			pid_cb__ = [](std::wstring&) { return 0U; };
			type__ = MIDI::ClassTypes::ClassMediaKey;
			out1__ = [](MIDI::Mackie::MIDIDATA&, DWORD&) { return false; };
			out2__ = std::bind(static_cast<const bool(MMKBridge::*)(MIDI::MidiUnit&, DWORD&)>(&MMKBridge::InCallBack), this, _1, _2);
			id__   = Utils::random_hash(this);
		}
		MMKBridge::~MMKBridge() {

		}

		void MMKBridge::Stop() {
			common_config::Get().Local.IsMMKeyesRun(false);
			to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_MMKEY_STOP));
			message_once__ = false;
		}
		MMKBridge& MMKBridge::Get() {
			common_config::Get().Local.IsMMKeyesRun(true);
			if (!message_once__) {
				message_once__ = true;
				to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_MMKEY_START));
			}
			return std::ref(mmkeysbridge__);
		}

		void MMKBridge::SetPidCb(callGetPid cb) {
			pid_cb__ = cb;
		}

		/* Private */

		const bool MMKBridge::InCallBack(MIDI::MidiUnit& m, DWORD&) {
			try {

				if ((m.type != Common::MIDI::MidiUnitType::BTN) &&
					(m.type != Common::MIDI::MidiUnitType::BTNTOGGLE))
					return false;
				
				KEYBDINPUT kbi{};
				
				/* see:  Common::MIDI::MackieHelper::GetTranslateMMKey(..) */

				switch (m.longtarget) {
					case Common::MIDI::Mackie::Target::SYS_Zoom:    { kbi.wVk = VK_ZOOM; break; }
					case Common::MIDI::Mackie::Target::SYS_Scrub:   { kbi.wVk = VK_LAUNCH_MEDIA_SELECT; break; }
					case Common::MIDI::Mackie::Target::SYS_Record:  { kbi.wVk = VK_PLAY; break; }
					case Common::MIDI::Mackie::Target::SYS_Rewind:  { kbi.wVk = VK_MEDIA_PREV_TRACK; break; }
					case Common::MIDI::Mackie::Target::SYS_Forward: { kbi.wVk = VK_MEDIA_NEXT_TRACK; break; }
					case Common::MIDI::Mackie::Target::SYS_Stop:    { kbi.wVk = VK_MEDIA_STOP; break; }
					case Common::MIDI::Mackie::Target::SYS_Play:    { kbi.wVk = VK_MEDIA_PLAY_PAUSE; break; }
					case Common::MIDI::Mackie::Target::SYS_Up:      { kbi.wVk = VK_VOLUME_UP; break; }
					case Common::MIDI::Mackie::Target::SYS_Down:    { kbi.wVk = VK_VOLUME_DOWN; break; }
					case Common::MIDI::Mackie::Target::SYS_Left:    { kbi.wVk = VK_VOLUME_MUTE; break; }
					case Common::MIDI::Mackie::Target::SYS_Right:   { kbi.wVk = VK_PAUSE; break; }
					default: return false;
				}

				/* TO-DO:
				if (!m.appvolume.empty()) {
					try {
						for (auto& a : m.appvolume) {
							uint32_t pid = pid_cb__(a);
							if (pid != 0) {
								HANDLE hwnd = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
								SetForegroundWindow((HWND)hwnd);
								SendMessageA((HWND)hwnd, WM_KEYDOWN, VK_MEDIA_PLAY_PAUSE, 0);
								SendMessageA((HWND)hwnd, WM_KEYUP, VK_MEDIA_PLAY_PAUSE, 0);
								CloseHandle(hwnd);
								return true;
							}
						}
					} catch (...) {}
				}
				*/

				kbi.dwFlags = KEYEVENTF_EXTENDEDKEY;
				kbi.dwExtraInfo = (ULONG_PTR) ::GetMessageExtraInfo();

				INPUT i{};
				i.type = INPUT_KEYBOARD;
				i.ki = kbi;
				return ::SendInput(1, &i, sizeof(INPUT)) == 1U;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
	}
}
