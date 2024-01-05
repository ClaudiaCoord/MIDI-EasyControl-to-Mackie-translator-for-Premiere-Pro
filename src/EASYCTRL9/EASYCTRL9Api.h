/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	EASYCTRL9 DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "CommonApi.h"
#include <mmeapi.h>

#define FLAG_EXPORT __declspec(dllimport)

class MidiDriver;
class MidiControllerBase;
class MidiControllerVirtual;

#include "h\MidiDriver.h"
#include "h\MidiControllerBase.h"
#include "h\MidiControllerVirtual.h"
#include "h\MidiDevices.h"

#undef FLAG_EXPORT


