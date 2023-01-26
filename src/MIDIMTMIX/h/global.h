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

#pragma warning( push )
#pragma warning( disable : 4251 )

namespace Common {
    namespace MIXER {
        class AudioSessionItem;
        class AudioSessionList;
        class AudioSessionMixer;
        class AudioSessionEvents;
        class AudioSessionEventsBuilder;
        class FLAG_EXPORT AudioVolumeValue;
    }
}
template class FLAG_EXPORT ::std::basic_string<wchar_t>;
template class FLAG_EXPORT ::std::allocator<Common::MIXER::AudioSessionItem>;
template class FLAG_EXPORT ::std::vector<Common::MIXER::AudioSessionItem*>;

#include "h\global_enum.h"
#include "h\AudioSessionHelper.h"
#include "h\AudioVolumeValue.h"
#include "h\AudioSessionErrors.h"
#include "h\AudioEndPointEvents.h"
#include "h\AudioSessionEventsBuilder.h"
#include "h\AudioSessionItemChange.h"
#include "h\AudioSessionItemMidi.h"
#include "h\AudioSessionItem.h"
#include "h\AudioSessionList.h"
#include "h\AudioSessionEvents.h"
#include "h\AudioSessionEventsNotify.h"
#include "h\AudioSessionMixer.h"

#pragma warning( pop )

