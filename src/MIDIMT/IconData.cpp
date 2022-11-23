/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2023, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "framework.h"
#include "MIDIMT.h"

ICONDATA::ICONDATA() : icon(nullptr) {}
ICONDATA::ICONDATA(HBITMAP hi) : icon(hi) {}
HBITMAP ICONDATA::Get() { return icon; }
void ICONDATA::Reset() {
	HBITMAP i = icon;
	icon = nullptr;
	if (i != nullptr) ::DeleteObject(i);
}

