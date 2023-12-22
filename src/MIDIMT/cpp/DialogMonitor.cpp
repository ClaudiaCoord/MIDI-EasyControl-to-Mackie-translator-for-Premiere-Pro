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

		static inline void set_controls__(HWND hwnd, bool action, bool isenable) {
			try {
				if (isenable) {
					UI::UiUtils::SetControlEnable(hwnd, DLG_MONITOR_STOP, action);
					UI::UiUtils::SetControlEnable(hwnd, DLG_MONITOR_START, !action);
					return;
				}
				UI::UiUtils::SetControlEnable(hwnd, DLG_MONITOR_STOP, false);
				UI::UiUtils::SetControlEnable(hwnd, DLG_MONITOR_START, false);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		DialogMonitor::DialogMonitor() {
			CbEvent::GetHwndCb = [=]() { return hwnd_.get(); };
		}

		void DialogMonitor::dispose_() {
			isload_ = false;
			IO::IOBridge::Get().UnSetCb(*static_cast<CbEvent*>(this));
			CbEvent::Init(-1);
			hwnd_.reset();
		}
		void DialogMonitor::init_() {
			try {
				set_controls__(hwnd_, false, IO::IOBridge::Get().IsStarted());

				CbEvent::Init(DLG_EVENT_LOG, DLG_EVENT_MONITOR);
				isload_ = true;

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogMonitor::start_() {
			try {
				if (!hwnd_) return;
				IO::IOBridge::Get().SetCb(*static_cast<CbEvent*>(this));
				set_controls__(hwnd_, true, IO::IOBridge::Get().IsStarted());
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogMonitor::stop_() {
			try {
				if (!hwnd_) return;
				IO::IOBridge::Get().UnSetCb(*static_cast<CbEvent*>(this));
				set_controls__(hwnd_, false, IO::IOBridge::Get().IsStarted());
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogMonitor::clear_() {
			try {
				if (!hwnd_) return;
				::SetDlgItemTextW(hwnd_, DLG_MONITOR_BOX, L"");
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		IO::PluginUi* DialogMonitor::GetUi() {
			return static_cast<IO::PluginUi*>(this);
		}

		const bool DialogMonitor::IsRunOnce() {
			return !hwnd_ && !isload_;
		}
		void DialogMonitor::SetFocus() {
			if (hwnd_) (void) ::SetFocus(hwnd_);
		}

		#pragma region Change On events
		void DialogMonitor::eventLog_(CbEventData* data) {
			try {
				if (!data) return;
				CbEventDataDeleter d = data->GetDeleter();
				if (!hwnd_) return;
				CbEvent::ToLog(GetDlgItem(hwnd_, DLG_MONITOR_LOG), d.GetData(), true);
			} catch (...) {}
		}
		void DialogMonitor::eventMonitor_(CbEventData* data) {
			try {
				if (!data) return;
				CbEventDataDeleter d = data->GetDeleter();
				if (!hwnd_) return;
				CbEvent::ToMonitor(GetDlgItem(hwnd_, DLG_MONITOR_BOX), d.GetData(), false);
			} catch (...) {}
		}
		#pragma endregion

		#pragma region Override
		HWND DialogMonitor::BuildDialog(HWND h) {
			hinst_.reset(LangInterface::Get().GetLangHinstance());
			return IO::PluginUi::BuildDialog(hinst_, h, MAKEINTRESOURCEW(DLG_MONITOR_WINDOW));
		}
		LRESULT DialogMonitor::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
			try {
				switch (m) {
					case WM_INITDIALOG: {
						hwnd_.reset(h, static_cast<SUBCLASSPROC>(&PluginUi::DialogProc_), reinterpret_cast<DWORD_PTR>(this), 0);
						init_();
						::ShowWindow(h, SW_SHOW);
						::SetFocus(::GetDlgItem(h, IDCANCEL));
						return static_cast<INT_PTR>(1);
					}
					case WM_HELP: {
						if (!l) break;
						UI::UiUtils::ShowHelpPage(DLG_MONITOR_WINDOW, reinterpret_cast<HELPINFO*>(l));
						return static_cast<INT_PTR>(1);
					}
					case WM_COMMAND: {
						if (!isload_) break;
						uint16_t c{ LOWORD(w) };
						switch (c) {
							case DLG_EVENT_LOG: {
								eventLog_(reinterpret_cast<MIDIMT::CbEventData*>(l));
								break;
							}
							case DLG_EVENT_MONITOR: {
								eventMonitor_(reinterpret_cast<MIDIMT::CbEventData*>(l));
								break;
							}
							case DLG_MONITOR_START: {
								start_();
								break;
							}
							case DLG_MONITOR_STOP: {
								stop_();
								break;
							}
							case DLG_MONITOR_CLEAR: {
								clear_();
								break;
							}
							case DLG_EXIT:
							case IDCANCEL: {
								dispose_();
								return static_cast<INT_PTR>(1);
							}
							default: break;
						}
						break;
					}
					default: break;
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return ::DefSubclassProc(h, m, w, l);
		}
		#pragma endregion

	}
}