/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIXER {

		AudioSessionMixerVolumeGroup::AudioSessionMixerVolumeGroup(AudioSessionMixer* m)
			: ByGuid(m), ByMidiId(m), ByAppId(m), ByAppName(m) {
		}

		template<typename T, VolumeKeyType KT>
		AudioSessionMixerVolume<T, KT>::AudioSessionMixerVolume(AudioSessionMixer* m) : mix_(m) {

		}

		template<typename T, VolumeKeyType KT>
		void  AudioSessionMixerVolume<T,KT>::SetVolume(const T key, uint8_t v) {
			mix_->SetToAudioSession<T, uint8_t>(AudioAction::AUDIO_VOLUME, KT, key, v, false);
		}

		template<typename T, VolumeKeyType KT>
		void  AudioSessionMixerVolume<T, KT>::SetVolume(const T key, float v) {
			mix_->SetToAudioSession<T, float>(AudioAction::AUDIO_VOLUME, KT, key, v, false);
		}

		template<typename T, VolumeKeyType KT>
		void  AudioSessionMixerVolume<T, KT>::SetVolume(const T key, float v, bool b) {
			mix_->SetToAudioSession<T, float>(AudioAction::AUDIO_ALL, KT, key, v, b);
		}

		template<typename T, VolumeKeyType KT>
		void  AudioSessionMixerVolume<T, KT>::SetVolume(const T key, uint8_t v, bool b) {
			mix_->SetToAudioSession<T, uint8_t>(AudioAction::AUDIO_ALL, KT, key, v, b);
		}

		template<typename T, VolumeKeyType KT>
		void  AudioSessionMixerVolume<T, KT>::IncrementVolume(const T key, bool b) {
			mix_->SetToAudioSession<T, float>(AudioAction::AUDIO_VOLUME_INCREMENT, KT, key, 0.0f, b);
		}

		template<typename T, VolumeKeyType KT>
		void  AudioSessionMixerVolume<T, KT>::SetMute(const T key, bool b) {
			mix_->SetToAudioSession<T, float>(AudioAction::AUDIO_MUTE, KT, key, 0.0f, b);
		}

		template<typename T, VolumeKeyType KT>
		bool  AudioSessionMixerVolume<T, KT>::GetMute(const T key) {
			std::tuple<float, bool> t = mix_->GetFromAudioSession<T>(AudioAction::AUDIO_MUTE, KT, key);
			return std::get<1>(t);
		}

		template<typename T, VolumeKeyType KT>
		float AudioSessionMixerVolume<T, KT>::GetVolume(const T key) {
			std::tuple<float, bool> t = mix_->GetFromAudioSession<T>(AudioAction::AUDIO_VOLUME, KT, key);
			return std::get<0>(t);
		}

		template AudioSessionMixerVolume<GUID,		VolumeKeyType::GUID_ID_KEY>;
		template AudioSessionMixerVolume<uint32_t,	VolumeKeyType::MIDI_ID_KEY>;
		template AudioSessionMixerVolume<std::size_t,	VolumeKeyType::APP_ID_KEY>;
		template AudioSessionMixerVolume<std::wstring,	VolumeKeyType::APP_NAME_KEY>;
	}
}
