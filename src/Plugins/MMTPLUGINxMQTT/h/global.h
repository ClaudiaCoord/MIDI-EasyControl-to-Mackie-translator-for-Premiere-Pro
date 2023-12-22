/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023, MIT

	MMTPLUGINx* DLL (SmartHomeKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "MIDIMTApi.h"
#include "MIDIMTVEUIApi.h"
#include "../../shared/h/dllmacro.h"

#define FLAG_EXPORT __declspec(dllexport)

#include "Broker.h"
#include "SetupDialog.h"
#include "MMTPLUGINxMQTT.h"

