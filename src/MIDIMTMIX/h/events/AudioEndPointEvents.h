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
        class AudioEndPointEvents : public IAudioEndpointVolumeCallback
        {
		private:
			LONG crefAll;
			locker_awaiter lock_{};
			std::function<void()> evDeleted;
			std::function<const GUID()> evGuid;
			AudioSessionList* ptrSessionList;

		public:

			AudioEndPointEvents(AudioSessionList* ptr, std::function<void()> fdel, std::function<const GUID()> fguid);
			const bool IsBusy() const;

			STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
			STDMETHODIMP_(ULONG) AddRef();
			STDMETHODIMP_(ULONG) Release();

			HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA);
        };
    }
}

