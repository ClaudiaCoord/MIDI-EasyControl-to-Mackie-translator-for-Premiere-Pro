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

		TrayMenu::TrayMenu() {
			try {
				LangInterface& lang = LangInterface::Get();

				uint16_t iconsId[] = {
					IDI_MMENU_I0,
					IDI_MMENU_I1,
					IDI_MMENU_I2,
					IDI_MMENU_I3,
					IDI_MMENU_I4,
					IDI_MMENU_I6,
					IDI_MMENU_I7,
					IDI_MMENU_I9
				};
				icons__.Init(iconsId, std::size(iconsId), true);
			} catch (...) {}
		}
		TrayMenu::~TrayMenu() {
			dispose();
		}
		void TrayMenu::dispose() {
			icons__.Release();
		}
		void TrayMenu::setitem(HMENU hm, uint16_t id, uint16_t iid, bool b, bool isstatus) {
			if (isstatus) {
				(void) ::CheckMenuItem(hm, id, (b ? MF_CHECKED : MF_UNCHECKED) | MF_BYPOSITION);
				(void) ::EnableMenuItem(hm, id, (b ? MF_GRAYED : MF_ENABLED) | MF_BYPOSITION);
			}
			HBITMAP ico = icons__.GetIcon(iid);
			if (ico == nullptr) return;
			(void) ::SetMenuItemBitmaps(hm, id, MF_BITMAP | MF_BYPOSITION, ico, ico);
		}

		void TrayMenu::EndDialog() {
			icons__.Reset();
		}
		void TrayMenu::Show(HWND hwnd, const POINT p) {
			try {
				if (icons__.IsEmpty())
					throw new runtime_werror(L"bad menu count!");

				HMENU hm = LangInterface::Get().GetMenu(MAKEINTRESOURCEW(IDR_TRAY_MENU));
				if (hm) {
					HMENU hsm = ::GetSubMenu(hm, 0);
					if (hsm) {
						uint32_t uFlags = TPM_RIGHTBUTTON;
						if (::GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0) uFlags |= TPM_RIGHTALIGN;
						else uFlags |= TPM_LEFTALIGN;

						bool brun = common_config::Get().Local.IsMidiBridgeRun();
						uint32_t cnt = ::GetMenuItemCount(hsm);

						for (uint16_t i = 0, ii = 0; i < cnt; i++) {
							switch (::GetMenuItemID(hsm, i)) {
								case 0: break;
								case IDM_GO_STOP:
								case IDM_GO_MIXER:
								case IDM_GO_MONITOR: setitem(hsm, i, ii++, !brun, true);  break;
								case IDM_GO_START:   setitem(hsm, i, ii++, brun, true);   break;
								default: 			 setitem(hsm, i, ii++, false, false); break;
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