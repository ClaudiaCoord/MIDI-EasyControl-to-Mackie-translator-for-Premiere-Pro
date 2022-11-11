/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

struct ICONDATA {
	HBITMAP icon;
	ICONDATA() : icon(nullptr) {}
	ICONDATA(HBITMAP hi) : icon(hi) {}
	void Reset() {
		HBITMAP i = icon;
		icon = nullptr;
		if (i != nullptr)
			::DeleteObject(i);
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

