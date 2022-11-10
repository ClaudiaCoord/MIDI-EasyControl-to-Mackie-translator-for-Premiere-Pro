/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"

static const uint16_t iconsId[] = { IDI_ICON1, IDI_ICON2, IDI_ICON3, 0, IDI_ICON4, IDI_ICON5, 0, IDI_ICON6 };

TrayMenu::TrayMenu(HINSTANCE hinst) {
	try {
		for (size_t i = 0; i < std::size(iconsId); i++) {
			if (iconsId[i] == 0U) {
				icons.push_back(new ICONDATA());
				continue;
			}
			HICON hi = (HICON)LoadImage(hinst, MAKEINTRESOURCE(iconsId[i]), IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT | LR_DEFAULTSIZE);
			if (hi != nullptr)
				icons.push_back(new ICONDATA(hi));
		}
	}
	catch (...) {}
}
TrayMenu::~TrayMenu() {
	Dispose();
}
void TrayMenu::Dispose() {
	try {
		for (ICONDATA *data : icons) {
			if (data == nullptr) continue;
			data->Reset();
			delete data;
		}
		icons.clear();
	}
	catch (...) {}
}
void TrayMenu::SetItem(HMENU hm, uint32_t id, bool b, bool isstatus) {
	if (isstatus) {
		(void) CheckMenuItem(hm, id, (b ? MF_CHECKED : MF_UNCHECKED) | MF_BYPOSITION);
		(void) EnableMenuItem(hm, id, (b ? MF_GRAYED : MF_ENABLED) | MF_BYPOSITION);
	}
	ICONDATA* data = icons.at(id);
	if ((data == nullptr) || (data->icon == nullptr)) return;
	(void) SetMenuItemBitmaps(hm, id, MF_BITMAP | MF_BYPOSITION, data->info.hbmColor, data->info.hbmColor);
}
void TrayMenu::EndDialog() {
	Dispose();
}
void TrayMenu::Show(HINSTANCE hinst, HWND hwnd, const POINT p) {
	try {
		HMENU hmain = LoadMenu(hinst, MAKEINTRESOURCE(IDR_TRAY_MENU));
		if (hmain) {
			HMENU hmenu = GetSubMenu(hmain, 0);
			if (hmenu) {
				uint32_t uFlags = TPM_RIGHTBUTTON;
				if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0) uFlags |= TPM_RIGHTALIGN;
				else uFlags |= TPM_LEFTALIGN;

				bool b = IsTMidiStarted();
				uint32_t cnt = ::GetMenuItemCount(hmenu);
				if (icons.size() != cnt)
					throw new std::runtime_error("bad menu count!");

				for (uint32_t i = 0; i < cnt; i++) {
					switch (::GetMenuItemID(hmenu, i)) {
					case IDM_GO_START: {
						SetItem(hmenu, i, b, true);
						break;
					}
					case IDM_GO_STOP:
					case IDM_GO_MONITOR: {
						SetItem(hmenu, i, !b, true);
						break;
					}
					case 0: {
						break;
					}
					default: {
						SetItem(hmenu, i, b, false);
						break;
					}
					}
				}
				TrackPopupMenuEx(hmenu, uFlags, p.x + 20, p.y, hwnd, NULL);
			}
			DestroyMenu(hmain);
		}
	}
	catch (...) {}
}
