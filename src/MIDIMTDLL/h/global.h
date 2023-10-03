/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2023, MIT

	MIDIMTLIB DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "CommonApi.h"

#define FLAG_EXPORT __declspec(dllexport)

class FLAG_EXPORT Timer;
class FLAG_EXPORT worker_background;
class FLAG_EXPORT common_error;
class FLAG_EXPORT log_string;
class common_event;

#include "..\..\Common\h\seh_exception.h"
#include "..\..\Common\h\common_error_id.h"
#include "..\..\Common\h\common_error.h"
#include "..\..\Common\h\handle_ptr.h"
#include "..\..\Common\h\midi\Midi.h"
#include "..\..\Common\h\midi\MidiHelper.h"
#include "..\..\Common\h\common_utils.h"
#include "..\..\Common\h\locker_awaiter.h"
#include "..\..\Common\h\locker_auto.h"
#include "..\..\Common\h\common_timer.h"
#include "..\..\Common\h\worker_background.h"
#include "..\..\Common\h\common_event.h"
#include "..\..\Common\h\log_string.h"
#include "..\..\Common\h\tiny_json.h"
#include "..\..\Common\h\light\DMXPacket.h"
#include "..\..\Common\h\light\ArtnetPacket.h"
#include "..\..\Common\h\light\DMXSerialPortConfig.h"
#include "..\..\Common\h\light\ArtnetInterfaceConfig.h"
#include "..\..\Common\h\mqtt\BrokerConfig.h"
#include "..\..\Common\h\midi\Mackie.h"
#include "..\..\Common\h\midi\MackieHelper.h"
#include "..\..\Common\h\midi\MidiData.h"
#include "..\..\Common\h\midi\MidiInstance.h"
#include "..\..\Common\h\midi\json_config.h"
#include "..\..\Common\h\json_recent.h"
#include "..\..\Common\h\ui_themes.h"
#include "..\..\Common\h\registry.h"
#include "..\..\Common\h\common_config.h"

