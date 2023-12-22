/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023, MIT

	MMTPLUGINx* DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "MIDIMTApi.h"
#include "EASYCTRL9Api.h"
#include "MIDIMTVEUIApi.h"
#include "../../shared/h/dllmacro.h"

#define FLAG_EXPORT __declspec(dllexport)

#include "MidiControllerIn.h"
#include "MidiControllerOut.h"
#include "MidiControllerProxy.h"
#include "SetupDialog.h"
#include "MMTPLUGINxMIDICTRL.h"

