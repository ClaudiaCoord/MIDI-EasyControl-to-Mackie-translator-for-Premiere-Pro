/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"

namespace Common {
	namespace MIDIMT {

		using namespace std::placeholders;

		static void SetControls(HWND hwnd, bool action, bool isenable) {
			try {
				if (isenable) {
					Gui::SetControlEnable(hwnd, IDC_MONITOR_STOP, action);
					Gui::SetControlEnable(hwnd, IDC_MONITOR_START, !action);
					return;
				}
				Gui::SetControlEnable(hwnd, IDC_MONITOR_STOP, false);
				Gui::SetControlEnable(hwnd, IDC_MONITOR_START, false);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		DialogMonitor::DialogMonitor() {
			mcb__.Init(IDC_IEVENT_LOG, IDC_IEVENT_MONITOR);
			mcb__.HwndCb = [=]() { return hwnd__.get(); };
		}
		DialogMonitor::~DialogMonitor() {
			dispose_();
		}
		void DialogMonitor::dispose_() {
			clear_();
			mcb__.Clear();
		}
		void DialogMonitor::clear_() {
			try {
				if (!hwnd__) return;
				(void) ::PostMessageW(hwnd__.get(), WM_COMMAND, MAKEWPARAM(IDCANCEL, 0), 0);
				hwnd__.reset();
				to_log::Get().unregistred(mcb__.GetCbLog());
				MIDI::MidiBridge::Get().RemoveCallbackOut(std::ref(mcb__));
			} catch (...) {}
		}

		const bool DialogMonitor::IsRunOnce() {
			return !hwnd__;
		}
		void DialogMonitor::SetFocus() {
			if (hwnd__) (void) ::SetFocus(hwnd__.get());
		}

		void DialogMonitor::EventLog(CbEventData* data) {
			try {
				if (data == nullptr) return;
				CbEventDataDeleter d = data->GetDeleter();
				if (!hwnd__) return;
				CbEvent::ToLog(GetDlgItem(hwnd__.get(), IDC_LOG), d.GetData(), true);
			} catch (...) {}
		}
		void DialogMonitor::EventMonitor(CbEventData* data) {
			try {
				if (data == nullptr) return;
				CbEventDataDeleter d = data->GetDeleter();
				if (!hwnd__) return;
				CbEvent::ToMonitor(GetDlgItem(hwnd__.get(), IDC_MONITOR), d.GetData(), false);
			} catch (...) {}
		}

		void DialogMonitor::InitDialog(HWND hwnd) {
			hwnd__.reset(hwnd);
			try {
				to_log::Get().registred(mcb__.GetCbLog());
				bool isrun = common_config::Get().Local.IsMidiBridgeRun();
				SetControls(hwnd__.get(), false, isrun);
				if (!isrun)
					mcb__.AddToLog(LangInterface::Get().GetString(IDS_DLG_MSG12));
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogMonitor::EndDialog() {
			if (!hwnd__) return;
			clear_();
		}

		void DialogMonitor::Start() {
			if (!hwnd__) return;
			try {
				SetControls(hwnd__.get(), true, common_config::Get().Local.IsMidiBridgeRun());
				MIDI::MidiBridge::Get().SetCallbackOut(std::ref(mcb__));
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogMonitor::Stop() {
			if (!hwnd__) return;
			try {
				SetControls(hwnd__.get(), false, common_config::Get().Local.IsMidiBridgeRun());
				MIDI::MidiBridge::Get().RemoveCallbackOut(std::ref(mcb__));
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogMonitor::Clear() {
			if (!hwnd__) return;
			HWND hwnd = hwnd__.get();

			try {
				::SetDlgItemTextW(hwnd, IDC_MONITOR, L"");
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
	}
}