/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

struct ICONDATA {
	HICON icon;
	ICONINFO info{};
	ICONDATA() : icon(nullptr) {}
	ICONDATA(HICON hi) : icon(hi) {
		if (!GetIconInfo(hi, &info))
			Reset();
	}
	void Reset() {
		if (icon != nullptr)
			::DestroyIcon(icon);
		icon = nullptr;
		info = ICONINFO();
	}
};

class TrayMenu
{
	std::vector<ICONDATA*> icons{};
	void Dispose();
	void SetItem(HMENU hm, uint32_t id, bool b, bool isstatus);
public:
	TrayMenu(HINSTANCE hinst);
	~TrayMenu();
	void EndDialog();
	void Show(HINSTANCE hinst, HWND hwnd, const POINT p);
};

