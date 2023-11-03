/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Viewer/Editor interface.
	(c) CC 2023, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "CommonApi.h"

#define FLAG_EXPORT __declspec(dllimport)

typedef uintptr_t uptr_t;
typedef intptr_t  sptr_t;
typedef sptr_t(*SciFnDirect)(sptr_t, unsigned int, uptr_t, sptr_t);
class SCNotification;

#include "scintilla/include/ILexer.h"
#include "h\ScintillaBox.h"

#undef FLAG_EXPORT


