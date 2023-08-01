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

		template<typename T, VolumeKeyType KT>
		class AudioSessionMixerVolume {
		private:
			AudioSessionMixer& mix__;

		public:

			AudioSessionMixerVolume();

			FLAG_EXPORT void  SetVolume(const T key, uint8_t v);
			FLAG_EXPORT void  SetVolume(const T key, float v);
			FLAG_EXPORT void  SetVolume(const T key, float v, bool b);
			FLAG_EXPORT void  SetVolume(const T key, uint8_t v, bool b);
			FLAG_EXPORT void  SetMute(const T key, bool b);
			FLAG_EXPORT bool  GetMute(const T key);
			FLAG_EXPORT float GetVolume(const T key);
			FLAG_EXPORT void  IncrementVolume(const T key, bool b);
		};

		class FLAG_EXPORT AudioSessionMixerVolumeGroup {
		public:
			AudioSessionMixerVolume<GUID,			VolumeKeyType::GUID_ID_KEY>		ByGuid{};
			AudioSessionMixerVolume<uint32_t,		VolumeKeyType::MIDI_ID_KEY>		ByMidiId{};
			AudioSessionMixerVolume<std::size_t,	VolumeKeyType::APP_ID_KEY>		ByAppId{};
			AudioSessionMixerVolume<std::wstring,	VolumeKeyType::APP_NAME_KEY>	ByAppName{};
		};

		extern template AudioSessionMixerVolume<GUID,			VolumeKeyType::GUID_ID_KEY>;
		extern template AudioSessionMixerVolume<uint32_t,		VolumeKeyType::MIDI_ID_KEY>;
		extern template AudioSessionMixerVolume<std::size_t,	VolumeKeyType::APP_ID_KEY>;
		extern template AudioSessionMixerVolume<std::wstring,	VolumeKeyType::APP_NAME_KEY>;
	}
}

