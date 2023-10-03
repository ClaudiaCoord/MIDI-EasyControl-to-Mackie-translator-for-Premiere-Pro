/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol, support USB Open DMX and USB RS485 dongle.
	+ Art-Net protocol, support UDP local network broadcast.
	(c) CC 2023, MIT

	MIDIMTLIGHT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "MIDIMTApi.h"
#include <mmeapi.h>

#define FLAG_EXPORT __declspec(dllexport)

#include "asio.h"
#include "enumser\enumser.h"
#include "DeviceClassInterface.h"
#include "DMXClassInterface.h"
#include "UdpPort.h"
#include "SerialPort.h"
#include "DMXSerial.h"
#include "DMXArtnet.h"
#include "LightBridge.h"

