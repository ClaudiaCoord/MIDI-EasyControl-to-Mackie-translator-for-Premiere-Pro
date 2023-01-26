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

#define FLAG_EXPORT __declspec(dllimport)

#pragma warning( push )
#pragma warning( disable : 4251 )

namespace Common {
    namespace MIXER {
        class AudioSessionList;
        class AudioSessionMixer;
        class AudioSessionItem;
        class AudioSessionEvents;
        class AudioSessionEventsBuilder;
        class AudioSessionEventsNotify;
        class FLAG_EXPORT AudioVolumeValue;
    }
}

struct IAudioSessionControl;
struct IAudioEndpointVolume;
struct IAudioSessionManager2;
template class FLAG_EXPORT::std::basic_string<wchar_t>;
template class FLAG_EXPORT::std::allocator<Common::MIXER::AudioSessionItem>;
template class FLAG_EXPORT::std::vector<Common::MIXER::AudioSessionItem*>;

#include "h\global_enum.h"
#include "h\AudioVolumeValue.h"
#include "h\AudioSessionItemChange.h"
#include "h\AudioSessionItemMidi.h"
#include "h\AudioSessionItem.h"
#include "h\AudioSessionMixer.h"

#pragma warning( pop )

#undef FLAG_EXPORT


