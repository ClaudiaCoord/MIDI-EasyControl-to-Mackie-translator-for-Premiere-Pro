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

		typedef std::function<void(AudioSessionItemChange)> callOnChange;
		typedef std::function<uint32_t(std::wstring&)> callPidByName;

		class AudioSessionMixer : public Common::MIDI::MidiInstance
		{
		private:
			static AudioSessionMixer audiosessionmixer__;

			CComPtr<IAudioSessionManager2> ptrASM2{};
			CComPtr<IAudioMeterInformation> ptrAVM{};
			std::shared_ptr<AudioSessionList> asList;
			std::unique_ptr<AudioSessionEventsNotify> asCbNotify;

			Common::common_event<callOnChange> onchangevalue__;
			callPidByName cb_pid_by_name__;
			std::atomic<bool> isinit__ = false;
			std::atomic<bool> iscoinitialize__ = false;
			uint32_t cpid__;
			uint32_t cbid__;
			bool isduplicateappremoved__ = false;
			bool issetaudioleveloldvalue__ = true;
			bool isfastvalue__ = false;

			CComPtr<IAudioEndpointVolume> getMainEndpointVolume();
			int getSessionList();

			const bool InCallBack(MIDI::MidiUnit&, DWORD&);
			void InternalCallBack(AudioSessionUnit&, AudioSessionItem*&);
			uint32_t InternalCallItemPidByName(std::wstring&);

		public:

			AudioSessionMixerVolumeGroup Volume{};

			FLAG_EXPORT void event_add(callOnChange, uint32_t);
			FLAG_EXPORT void event_remove(uint32_t);
			FLAG_EXPORT void event_send(AudioSessionItemChange);

			FLAG_EXPORT AudioSessionMixer();
			FLAG_EXPORT ~AudioSessionMixer();
			FLAG_EXPORT void init(bool = true);
			FLAG_EXPORT void release();

			FLAG_EXPORT void Start(bool = false);
			FLAG_EXPORT void Stop();

			FLAG_EXPORT static const double GetVolumeDevider;
			FLAG_EXPORT static AudioSessionMixer& Get();
			FLAG_EXPORT callPidByName GetCbItemPidByName();
			FLAG_EXPORT const bool GetMasterVolumePeak(float*);

			FLAG_EXPORT std::vector<AudioSessionItem*>& GetSessionList();
			FLAG_EXPORT std::wstring SessionListToString();
			FLAG_EXPORT uint32_t GetCurrentPid();
			FLAG_EXPORT void SetControlUnits(std::shared_ptr<Common::MIDI::MidiDevice>&);

			template<typename T1, typename T2>
			FLAG_EXPORT void SetToAudioSession(AudioAction t, VolumeKeyType kt, const T1 key, T2 v, bool b);

			template<typename T1>
			FLAG_EXPORT std::tuple<float, bool> GetFromAudioSession(AudioAction t, VolumeKeyType kt, const T1 key);

			AudioSessionItem* AudioSessionItemBuilder(IAudioSessionControl* ptr);
			static FLAG_EXPORT AudioSessionItemTitle MasterControl;
		};
		extern template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<GUID, float>(AudioAction, VolumeKeyType, const GUID, float, bool);
		extern template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<GUID, uint8_t>(AudioAction, VolumeKeyType, const GUID, uint8_t, bool);
		extern template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<uint32_t, float>(AudioAction, VolumeKeyType, const uint32_t, float, bool);
		extern template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<uint32_t, uint8_t>(AudioAction, VolumeKeyType, const uint32_t, uint8_t, bool);
		extern template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<std::size_t, float>(AudioAction, VolumeKeyType, const std::size_t, float, bool);
		extern template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<std::size_t, uint8_t>(AudioAction, VolumeKeyType, const std::size_t, uint8_t, bool);
		extern template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<std::wstring, float>(AudioAction, VolumeKeyType, const std::wstring, float, bool);
		extern template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<std::wstring, uint8_t>(AudioAction, VolumeKeyType, const std::wstring, uint8_t, bool);
		extern template FLAG_EXPORT std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<GUID>(AudioAction, VolumeKeyType, const GUID);
		extern template FLAG_EXPORT std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<uint32_t>(AudioAction, VolumeKeyType, const uint32_t);
		extern template FLAG_EXPORT std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<std::size_t>(AudioAction, VolumeKeyType, const std::size_t);
		extern template FLAG_EXPORT std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<std::wstring>(AudioAction, VolumeKeyType, const std::wstring);
	}
}
