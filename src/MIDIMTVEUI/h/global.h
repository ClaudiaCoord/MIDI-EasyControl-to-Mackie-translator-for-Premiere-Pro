/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ UI/Viewer/Editor interface.
	(c) CC 2023-2024, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!

	---------------------------------------------
	Custom define DLL/APP to EXPORT/IMPORT:

		FLAG_IMPORT: DLL -> empty,		App -> dllimport
		FLAG_EXPORT: DLL -> dllexport,	App -> dllimport
		FLAG_EXTERN: DLL -> extern,		App -> empty
*/

#pragma once

#include "MIDIMTApi.h"
#include <windowsx.h>
#include <shellapi.h>

#define FLAG_IMPORT
#define FLAG_EXPORT __declspec(dllexport)
#define FLAG_EXTERN extern

#include "scintilla/include/Scintilla.h"
#include "scintilla/include/ILexer.h"
#include "scintilla/include/Lexilla.h"
#include "scintilla/include/SciLexer.h"

#include "ScintillaBox.h"
#include "ScintillaHelper.h"
#include "ext/lodepng.h"
#include "ui/UiUtils.h"
#include "ui/UiImage.h"
#include "ui/Sprite.h"
#include "ui/Sprites.h"
#include "ui/ToolTipData.h"
#include "ui/Panel.h"
#include "ui/WindowToTop.h"

