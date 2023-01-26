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

namespace Common {
    namespace MIXER {
		enum class FLAG_EXPORT TypeItems : int {
			TypeSession = 0,
			TypeMaster,
			TypeDevice,
			TypeNone
		};
		enum class FLAG_EXPORT ListState : int {
			StateNone = 0,
			StateFound,
			StatePid,
			StateTitle,
			StateError,
			StateBusy
		};
		enum class FLAG_EXPORT AudioAction : int {
			AUDIO_NONE = 0,
			AUDIO_MUTE,
			AUDIO_VOLUME,
			AUDIO_VOLUME_INCREMENT,
			AUDIO_PANORAMA
		};
		enum class FLAG_EXPORT OnChangeType : int {
			OnChangeNone = 0,
			OnChangeNew,
			OnChangeUpdateData,
			OnChangeUpdateVolume,
			OnChangeUpdatePan,
			OnChangeUpdateMute,
			OnChangeNoCbUpdateData,
			OnChangeNoCbUpdateVolume,
			OnChangeNoCbUpdatePan,
			OnChangeNoCbUpdateMute,
			OnChangeUpdateAllValues,
			OnChangeRemove
		};
    }
}

