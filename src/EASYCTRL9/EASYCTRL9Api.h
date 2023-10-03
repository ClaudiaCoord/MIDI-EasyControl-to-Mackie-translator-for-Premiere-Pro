/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	EASYCTRL9 DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "CommonApi.h"
#include <mmeapi.h>

#define FLAG_EXPORT __declspec(dllimport)

class bridge_out_event;
class bridge_in_event;
class MidiDriver;
class MidiControllerBase;
class MidiControllerVirtual;
class MidiControllerIn;
class MidiControllerOut;
class MidiControllerProxy;

#include "h\MidiBridge.h"

#undef FLAG_EXPORT


