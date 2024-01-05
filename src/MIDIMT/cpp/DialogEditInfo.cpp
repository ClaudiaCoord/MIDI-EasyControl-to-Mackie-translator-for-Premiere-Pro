/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"
#include <Commdlg.h>

namespace Common {
	namespace MIDIMT {

		void DialogEditInfo::dispose_() {
			isload_ = false;
			hwnd_.reset();
		}
		void DialogEditInfo::init_() {
			isload_ = true;
		}

		IO::PluginUi* DialogEditInfo::GetUi() {
			return static_cast<IO::PluginUi*>(this);
		}

		const bool DialogEditInfo::IsRunOnce() {
			return !hwnd_ && !isload_;
		}
		void DialogEditInfo::SetFocus() {
			if (hwnd_) (void) ::SetFocus(hwnd_);
		}

		#pragma region Override
		HWND DialogEditInfo::BuildDialog(HWND h) {
			return IO::PluginUi::BuildDialog(LangInterface::Get().GetLangHinstance(), h, MAKEINTRESOURCEW(DLG_EDIT_INFO_WINDOW));
		}
		LRESULT DialogEditInfo::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
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
						UI::UiUtils::ShowHelpPage(DLG_EDIT_INFO_WINDOW, reinterpret_cast<HELPINFO*>(l));
						return static_cast<INT_PTR>(1);
					}
					case WM_COMMAND: {
						if (!isload_) break;
						uint16_t c{ LOWORD(w) };
						switch (c) {
							case DLG_GO_URL: {
								::ShellExecuteW(0, 0, LangInterface::Get().GetString(STRING_URL_ESETUP).c_str(), 0, 0, SW_SHOW);
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
