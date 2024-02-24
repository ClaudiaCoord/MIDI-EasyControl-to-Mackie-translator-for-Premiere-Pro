/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (Remote Controls Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "MIDIMTApi.h"
#include "MIDIMTVEUIApi.h"
#include <iphlpapi.h>
#include "../../shared/h/dllmacro.h"
#include "../../shared/h/asio/ws.h"
#include "../rc/MMTPLUGINxREMOTExRC.h"

#define FLAG_EXPORT __declspec(dllexport)

#include "SetupDialog.h"
#include "MMTPLUGINxREMOTE.h"

