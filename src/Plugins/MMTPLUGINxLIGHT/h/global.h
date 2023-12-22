/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023, MIT

	MMTPLUGINx* DLL (LightKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "MIDIMTApi.h"
#include "MIDIMTVEUIApi.h"
#include "../../shared/h/dllmacro.h"

#define FLAG_EXPORT __declspec(dllexport)

#include "asio.h"
#include "enumser\enumser.h"
#include "DeviceClassInterface.h"
#include "DMXClassInterface.h"
#include "UdpPort.h"
#include "SerialPort.h"
#include "DMXSerial.h"
#include "DMXArtnet.h"
#include "SetupDialog.h"
#include "MMTPLUGINxLIGHT.h"

