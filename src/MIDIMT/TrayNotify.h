/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once
#include "MIDIMT.h"

class TrayNotify
{
	GUID guid;
	NOTIFYICONDATA data = { sizeof(data) };

public:

	TrayNotify();
	~TrayNotify();

	void Init(HINSTANCE hinst, HWND hwnd, uint32_t id, const wchar_t* title);
	void Install();
	void UnInstall();
	void Show(HINSTANCE hinst);
};

