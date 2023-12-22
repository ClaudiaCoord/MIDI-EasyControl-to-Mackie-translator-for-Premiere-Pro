/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023, MIT

	MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "MIDIMTApi.h"
#define FLAG_EXPORT __declspec(dllexport)

class bridge_out_event;
class bridge_in_event;

#include "bridge_out_event.h"
#include "bridge_in_event.h"
#include "bridge_sys.h"
#include "Plugins.h"
#include "IOBridge.h"
