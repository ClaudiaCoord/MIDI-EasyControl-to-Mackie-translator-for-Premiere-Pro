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

			CComPtr<IAudioSessionManager2> ptrASM2;
			std::shared_ptr<AudioSessionList> asList;
			std::unique_ptr<AudioSessionEventsNotify> asCbNotify;

			Common::common_event<callOnChange> onchangevalue__;
			callPidByName cb_pid_by_name__;
			std::atomic<bool> isinit__ = false;
			std::atomic<bool> iscoinitialize__ = false;
			uint32_t cpid__;
			uint32_t cbid__;
			bool issetaudioleveloldvalue__ = true;
			bool isfastvalue__ = false;

			CComPtr<IAudioEndpointVolume> getMainEndpointVolume();
			int getSessionList();
			void setAudioSession(AudioAction t, uint32_t key, float v, bool b);
			std::tuple<float, bool> getAudioSession(AudioAction t, uint32_t key);

			const bool InCallBack(Common::MIDI::MidiUnit&, DWORD&);
			void InternalCallBack(Common::MIDI::MixerUnit&, AudioSessionItem*&);
			uint32_t InternalCallItemPidByName(std::wstring&);

		public:

			FLAG_EXPORT void event_add(callOnChange, uint32_t);
			FLAG_EXPORT void event_remove(uint32_t);
			FLAG_EXPORT void event_send(AudioSessionItemChange);

			FLAG_EXPORT AudioSessionMixer();
			FLAG_EXPORT ~AudioSessionMixer();
			FLAG_EXPORT void init(bool = true);
			FLAG_EXPORT void release();

			FLAG_EXPORT void Start();
			FLAG_EXPORT void Stop();

			FLAG_EXPORT static const double GetVolumeDevider;
			FLAG_EXPORT static AudioSessionMixer& Get();
			FLAG_EXPORT callPidByName GetCbItemPidByName();
			FLAG_EXPORT void  SetVolume(uint32_t key, uint8_t& v);
			FLAG_EXPORT void  SetVolume(uint32_t key, float& v);
			FLAG_EXPORT void  SetMute(uint32_t key, bool b);
			FLAG_EXPORT bool  GetMute(uint32_t key);
			FLAG_EXPORT float GetVolume(uint32_t key);
			FLAG_EXPORT void  IncrementVolume(uint32_t key, bool b);
			FLAG_EXPORT std::vector<AudioSessionItem*>& GetSessionList();
			FLAG_EXPORT std::wstring SessionListToString();
			FLAG_EXPORT uint32_t GetCurrentPid();
			FLAG_EXPORT void SetControlUnits(std::shared_ptr<Common::MIDI::MidiDevice>& md);

			AudioSessionItem* AudioSessionItemBuilder(IAudioSessionControl* ptr);
		};
	}
}
