/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	EASYCTRL9

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "MIDIMTApi.h"
#include <mmeapi.h>

#define FLAG_EXPORT __declspec(dllexport)

#pragma warning( push )
#pragma warning( disable : 4251 )

class bridge_out_event;
class bridge_in_event;

#include "h\teVirtualMIDI.h"
#include "h\midi_utils.h"
#include "h\MidiControllerBase.h"
#include "h\bridge_out_event.h"
#include "h\bridge_in_event.h"
#include "h\MidiControllerVirtual.h"
#include "h\MidiControllerIn.h"
#include "h\MidiControllerOut.h"
#include "h\MidiControllerProxy.h"
#include "h\MidiBridge.h"

#pragma warning( pop )