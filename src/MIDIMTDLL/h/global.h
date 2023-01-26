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

#pragma warning( push )
#pragma warning( disable : 4251 )

#pragma warning( push )
#pragma warning( disable : 4275 )
#include "runtime_werror.h"
#pragma warning( pop )

class FLAG_EXPORT Timer;
class FLAG_EXPORT worker_background;
class common_event;

#include "seh_exception.h"
#include "handle_ptr.h"
#include "midi\Midi.h"
#include "midi\MidiHelper.h"
#include "common_utils.h"
#include "locker_awaiter.h"
#include "locker_auto.h"
#include "Timer.h"
#include "worker_background.h"
#include "common_event.h"
#include "log_string.h"
#include "tiny_json.h"
#include "midi\Mackie.h"
#include "midi\MackieHelper.h"
#include "midi\MidiData.h"
#include "midi\MidiInstance.h"
#include "midi\JsonConfig.h"
#include "ui_themes.h"
#include "registry.h"
#include "common_config.h"

#pragma warning( pop )

