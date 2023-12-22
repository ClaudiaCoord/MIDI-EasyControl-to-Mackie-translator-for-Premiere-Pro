/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	EASYCTRL9 MIDI driver

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "MIDIMTApi.h"
#include <mmeapi.h>

#define FLAG_EXPORT __declspec(dllexport)

#ifndef _LOAD_MIDIDRV_DYNAMIC
#include "h\teVirtualMIDI.h"
#endif

#include "MidiDriver.h"
#include "MidiControllerBase.h"
#include "MidiControllerVirtual.h"
#include "MidiDevices.h"

