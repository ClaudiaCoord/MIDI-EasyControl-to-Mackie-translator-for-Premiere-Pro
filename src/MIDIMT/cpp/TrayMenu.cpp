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

namespace Common {
	namespace MIDIMT {

		TrayMenu::TrayMenu() {
			try {
				uint16_t iconsId[] = {
					ICON_APP_MMENU_I0,
					ICON_APP_MMENU_I1,
					ICON_APP_MMENU_I2,
					ICON_APP_MMENU_I3,
					ICON_APP_MMENU_I4,
					ICON_APP_MMENU_I6,
					ICON_APP_MMENU_I7,
					ICON_APP_MMENU_I9,
					ICON_APP_MMENU_I10
				};
				icons_.Init(
					iconsId,
					std::size(iconsId),
					[=](uint16_t n) -> HICON {
						return LangInterface::Get().GetIcon24x24(MAKEINTRESOURCEW(n));
					},
					true);
			} catch (...) {}
		}
		TrayMenu::~TrayMenu() {
			dispose_();
		}
		void TrayMenu::dispose_() {
			icons_.Release();
		}
		void TrayMenu::setitem_(HMENU hm, uint16_t id, uint16_t iid, bool b, bool isstatus) {
			if (isstatus) {
				(void) ::CheckMenuItem(hm, id, (b ? MF_CHECKED : MF_UNCHECKED) | MF_BYPOSITION);
				(void) ::EnableMenuItem(hm, id, (b ? MF_GRAYED : MF_ENABLED) | MF_BYPOSITION);
			}
			HBITMAP ico = icons_.GetIcon(iid);
			if (ico == nullptr) return;
			(void) ::SetMenuItemBitmaps(hm, id, MF_BITMAP | MF_BYPOSITION, ico, ico);
		}

		void TrayMenu::End() {
			icons_.Reset();
		}
		void TrayMenu::Show(HWND hwnd, const POINT p) {
			try {
				if (icons_.IsEmpty())
					throw make_common_error(L"bad menu count!");

				HMENU hm = LangInterface::Get().GetMenu(MAKEINTRESOURCEW(DLG_TRAY_MENU));
				if (hm) {
					HMENU hsm = ::GetSubMenu(hm, 0);
					if (hsm) {
						uint32_t uFlags = TPM_RIGHTBUTTON;
						if (::GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0) uFlags |= TPM_RIGHTALIGN;
						else uFlags |= TPM_LEFTALIGN;

						bool isrun = IO::IOBridge::Get().IsStarted(),
							 isdriver = MIDI::MidiDevices::Get().CheckVirtualDriver();
						uint32_t cnt = ::GetMenuItemCount(hsm);

						for (uint16_t i = 0, ii = 0; i < cnt; i++) {
							switch (::GetMenuItemID(hsm, i)) {
								case 0: break;
								case IDM_GO_EMPTY: {
									if (!isdriver) {
										std::wstring warn = LangInterface::Get().GetString(STRING_TMENU_MSG1);
										MENUITEMINFOW mi{};
										mi.cbSize = sizeof(mi);
										mi.fMask = MIIM_TYPE | MIIM_DATA;
										::GetMenuItemInfoW(hsm, IDM_GO_EMPTY, false, &mi);
										mi.fMask = MIIM_TYPE | MIIM_DATA;
										mi.dwTypeData = reinterpret_cast<LPWSTR>(warn.data());
										::SetMenuItemInfoW(hsm, IDM_GO_EMPTY, false, &mi);
									}
									setitem_(hsm, i, ii++, false, false);
									break;
								}
								case IDM_GO_STOP:
								case IDM_GO_MIXER: setitem_(hsm, i, ii++, !isrun, true); break;
								default:		   setitem_(hsm, i, ii++, false, false); break;
							}
						}
						HMENU hsmm = ::GetSubMenu(hsm, 3);
						if (hsmm) {
							if (::GetMenuItemCount(hsmm) >= 2) {
								setitem_(hsmm, 0, 3, !isrun, true); // IDM_GO_MONITOR: 
								setitem_(hsmm, 1, 8, false, false); // IDM_GO_VIEWLOG: 
							}
						}
						::TrackPopupMenuEx(hsm, uFlags, p.x + 20, p.y, hwnd, NULL);
					}
					::DestroyMenu(hm);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
	}
}