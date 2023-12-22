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

		typedef std::function<void(AudioSessionItemChange)> callOnChange_t;

		class FLAG_EXPORT AudioSessionMixer : public IO::PluginCb {
		private:

			CComPtr<IAudioSessionManager2> ptrASM2{};
			CComPtr<IAudioMeterInformation> ptrAVM{};
			std::shared_ptr<AudioSessionList> asList;
			std::unique_ptr<AudioSessionEventsNotify> asCbNotify;

			common_event<callOnChange_t> onchangevalue_{};
			std::atomic<bool> isinit_{ false };
			std::atomic<bool> iscoinitialize_{ false };
			uint32_t cpid_;
			bool isduplicateappremoved_{ false };
			bool issetaudioleveloldvalue_{ true };
			bool isfastvalue_{ false };

			CComPtr<IAudioEndpointVolume> getMainEndpointVolume();
			int getSessionList();
			
			void init_(bool = true);
			void release_();

			void internal_cb_(ASUNIT_t&, ASITEM_t&);
			uint32_t internal_pid_by_name_(std::wstring&);

			void cb_out_call_(MIDI::MidiUnit&, DWORD);
			void set_config_cb_(std::shared_ptr<JSON::MMTConfig>&);
			void event_send_cb_(AudioSessionItemChange);

		public:

			static const double GetVolumeDevider;
			AudioSessionMixerVolumeGroup Volume;

			void event_add(callOnChange_t, uint32_t);
			void event_remove(uint32_t);

			AudioSessionMixer();
			~AudioSessionMixer();

			void Start(bool = false);
			void Stop();

			IO::callGetPid_t GetCbItemPidByName();
			const bool GetMasterVolumePeak(float*);

			ASLIST_t& GetSessionList();
			std::wstring SessionListToString();
			uint32_t GetCurrentPid();
			
			template<typename T1, typename T2>
			void SetToAudioSession(AudioAction t, VolumeKeyType kt, const T1 key, T2 v, bool b);

			template<typename T1>
			std::tuple<float, bool> GetFromAudioSession(AudioAction t, VolumeKeyType kt, const T1 key);

			void AudioSessionItemBuilder(IAudioSessionControl*);
			static AudioSessionItemTitle MasterControl;
		};
		FLAG_EXTERN template FLAG_IMPORT void AudioSessionMixer::SetToAudioSession<GUID, float>(AudioAction, VolumeKeyType, const GUID, float, bool);
		FLAG_EXTERN template FLAG_IMPORT void AudioSessionMixer::SetToAudioSession<GUID, uint8_t>(AudioAction, VolumeKeyType, const GUID, uint8_t, bool);
		FLAG_EXTERN template FLAG_IMPORT void AudioSessionMixer::SetToAudioSession<uint32_t, float>(AudioAction, VolumeKeyType, const uint32_t, float, bool);
		FLAG_EXTERN template FLAG_IMPORT void AudioSessionMixer::SetToAudioSession<uint32_t, uint8_t>(AudioAction, VolumeKeyType, const uint32_t, uint8_t, bool);
		FLAG_EXTERN template FLAG_IMPORT void AudioSessionMixer::SetToAudioSession<std::size_t, float>(AudioAction, VolumeKeyType, const std::size_t, float, bool);
		FLAG_EXTERN template FLAG_IMPORT void AudioSessionMixer::SetToAudioSession<std::size_t, uint8_t>(AudioAction, VolumeKeyType, const std::size_t, uint8_t, bool);
		FLAG_EXTERN template FLAG_IMPORT void AudioSessionMixer::SetToAudioSession<std::wstring, float>(AudioAction, VolumeKeyType, const std::wstring, float, bool);
		FLAG_EXTERN template FLAG_IMPORT void AudioSessionMixer::SetToAudioSession<std::wstring, uint8_t>(AudioAction, VolumeKeyType, const std::wstring, uint8_t, bool);
		FLAG_EXTERN template FLAG_IMPORT std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<GUID>(AudioAction, VolumeKeyType, const GUID);
		FLAG_EXTERN template FLAG_IMPORT std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<uint32_t>(AudioAction, VolumeKeyType, const uint32_t);
		FLAG_EXTERN template FLAG_IMPORT std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<std::size_t>(AudioAction, VolumeKeyType, const std::size_t);
		FLAG_EXTERN template FLAG_IMPORT std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<std::wstring>(AudioAction, VolumeKeyType, const std::wstring);
	}
}
