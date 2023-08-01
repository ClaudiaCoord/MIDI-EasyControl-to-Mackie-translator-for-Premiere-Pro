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

#include "seh_exception.h"
#include "common_error_id.h"
#include "common_error.h"
#include "handle_ptr.h"
#include "midi\Midi.h"
#include "midi\MidiHelper.h"
#include "common_utils.h"
#include "locker_awaiter.h"
#include "locker_auto.h"
#include "common_timer.h"
#include "worker_background.h"
#include "common_event.h"
#include "log_string.h"
#include "tiny_json.h"
#include "mqtt\BrokerConfig.h"
#include "midi\Mackie.h"
#include "midi\MackieHelper.h"
#include "midi\MidiData.h"
#include "midi\MidiInstance.h"
#include "midi\json_config.h"
#include "ui_themes.h"
#include "registry.h"
#include "common_config.h"

