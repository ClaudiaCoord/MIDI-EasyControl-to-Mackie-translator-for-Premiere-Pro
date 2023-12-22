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

#include "CommonApi.h"

#define FLAG_IMPORT __declspec(dllimport)
#define FLAG_EXPORT __declspec(dllimport)
#define FLAG_EXTERN

namespace Common {
    namespace MIXER {
        class FLAG_EXPORT AudioSessionMixer;
        class AudioSessionUnit;
        class AudioSessionItem;
        class AudioSessionList;
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

struct IAudioSessionControl;
struct IAudioEndpointVolume;
struct IAudioSessionManager2;
struct IAudioMeterInformation;

#include "h\global_enum.h"
#include "h\item\AudioSessionItemId.h"
#include "h\item\AudioSessionItemApp.h"
#include "h\item\AudioSessionItemValue.h"
#include "h\item\AudioSessionItemBase.h"
#include "h\item\AudioSessionItemChange.h"
#include "h\item\AudioSessionItem.h"
#include "h\AudioSessionUnit.h"
#include "h\AudioSessionMixerVolume.h"
#include "h\AudioSessionMixer.h"
#include "h\AudioSessionHelper.h"

#undef FLAG_EXPORT
#undef FLAG_IMPORT
#undef FLAG_EXTERN


