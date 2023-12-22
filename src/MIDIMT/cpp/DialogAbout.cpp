/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2023, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"

namespace Common {
	namespace MIDIMT {

		void DialogAbout::dispose_() {
			isload_ = false;
			hwnd_.reset();
		}
		void DialogAbout::init_() {
			try {
				HWND h;
				if ((h = ::GetDlgItem(hwnd_, IDCANCEL)))				::BringWindowToTop(h);
				if ((h = ::GetDlgItem(hwnd_, DLG_GO_HELP)))				::BringWindowToTop(h);
				if ((h = ::GetDlgItem(hwnd_, DLG_GO_UPDATE)))			::BringWindowToTop(h);
				if ((h = ::GetDlgItem(hwnd_, DLG_ABOUT_ABOUT_COPY)))	::BringWindowToTop(h);
				if ((h = ::GetDlgItem(hwnd_, DLG_ABOUT_ABOUT_VERSION)))	::BringWindowToTop(h);

				isload_ = true;

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		IO::PluginUi* DialogAbout::GetUi() {
			return static_cast<IO::PluginUi*>(this);
		}

		const bool DialogAbout::IsRunOnce() {
			return !hwnd_ && !isload_;
		}
		void DialogAbout::SetFocus() {
			if (hwnd_) (void) ::SetFocus(hwnd_);
		}

		#pragma region Override
		HWND DialogAbout::BuildDialog(HWND h) {
			hinst_.reset(LangInterface::Get().GetLangHinstance());
			return IO::PluginUi::BuildDialog(hinst_, h, MAKEINTRESOURCEW(DLG_ABOUT_WINDOW));
		}
		LRESULT DialogAbout::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
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
						UI::UiUtils::ShowHelpPage(DLG_ABOUT_WINDOW, reinterpret_cast<HELPINFO*>(l));
						return static_cast<INT_PTR>(1);
					}
					case WM_COMMAND: {
						if (!isload_) break;
						uint16_t c{ LOWORD(w) };
						switch (c) {
							case DLG_GO_UPDATE: {
								::ShellExecuteW(0, 0, LangInterface::Get().GetString(STRING_URL_GIT).c_str(), 0, 0, SW_SHOW);
								break;
							}
							case DLG_GO_HELP: {
								::ShellExecuteW(0, 0, LangInterface::Get().GetString(STRING_URL_WIKI).c_str(), 0, 0, SW_SHOW);
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