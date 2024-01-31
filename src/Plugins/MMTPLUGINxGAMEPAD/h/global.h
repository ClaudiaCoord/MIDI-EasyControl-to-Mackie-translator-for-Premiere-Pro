/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Joystick support.
	+ GamePad support.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (GamePad/Joistick Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "MIDIMTApi.h"
#include "MIDIMTVEUIApi.h"
#include <joystickapi.h>
#include "../../shared/h/dllmacro.h"

#define FLAG_EXPORT __declspec(dllexport)

#include "JoistickData.h"
#include "JoistickInput.h"
#include "JoistickControl.h"
#include "JoisticksBase.h"
#include "SetupDialog.h"
#include "MMTPLUGINxGAMEPAD.h"

