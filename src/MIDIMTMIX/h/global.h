/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "MIDIMTApi.h"

#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>

#define FLAG_EXPORT __declspec(dllexport)
#define KEY_DEVIDER 0.007874

namespace Common {
    namespace MIXER {
        class AudioSessionItem;
        class AudioSessionList;
        class AudioSessionMixer;
        class AudioSessionEvents;
        class AudioSessionEventsNotify;
        class AudioSessionEventsBuilder;
        class FLAG_EXPORT AudioSessionItemId;
        class FLAG_EXPORT AudioSessionItemApp;
        class FLAG_EXPORT AudioSessionItemValue;
        class FLAG_EXPORT AudioSessionItemBase;
        class FLAG_EXPORT AudioSessionItemChange;
    }
}

#include "h\global_enum.h"
#include "h\AudioSessionHelper.h"
#include "h\item\AudioSessionItemId.h"
#include "h\item\AudioSessionItemApp.h"
#include "h\item\AudioSessionItemValue.h"
#include "h\item\AudioSessionItemBase.h"
#include "h\item\AudioSessionItemChange.h"
#include "h\events\AudioEndPointEvents.h"
#include "h\events\AudioSessionEventsBuilder.h"
#include "h\item\AudioSessionItem.h"
#include "h\AudioSessionUnit.h"
#include "h\AudioSessionList.h"
#include "h\events\AudioSessionEvents.h"
#include "h\events\AudioSessionEventsNotify.h"
#include "h\AudioSessionMixerVolume.h"
#include "h\AudioSessionMixer.h"
