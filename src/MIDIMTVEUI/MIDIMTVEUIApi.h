/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Viewer/Editor interface.
	(c) CC 2023-2024, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "CommonApi.h"
#include <windowsx.h>

#define FLAG_IMPORT __declspec(dllimport)
#define FLAG_EXPORT __declspec(dllimport)
#define FLAG_EXTERN

typedef uintptr_t uptr_t;
typedef intptr_t  sptr_t;
typedef sptr_t(*SciFnDirect)(sptr_t, unsigned int, uptr_t, sptr_t);
class SCNotification;

#include "scintilla/include/ILexer.h"
#include "h/ScintillaBox.h"
#include "h/ui/UiUtils.h"
#include "h/ui/UiImage.h"
#include "h/ui/Sprite.h"
#include "h/ui/Sprites.h"
#include "h/ui/ToolTipData.h"
#include "h/ui/Panel.h"

#undef FLAG_IMPORT
#undef FLAG_EXPORT
#undef FLAG_EXTERN

