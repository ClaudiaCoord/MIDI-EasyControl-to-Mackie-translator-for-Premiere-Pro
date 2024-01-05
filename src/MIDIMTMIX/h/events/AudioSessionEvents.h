/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace MIXER {

        class AudioSessionEvents : public IAudioSessionEvents
        {
        private:
            LONG crefAll;
            locker_awaiter lock_{};
            AudioSessionList* ptrSessionList;
            std::function<void()> evDeleted;
            std::function<const GUID()> evGuid;

        public:

            AudioSessionEvents(AudioSessionList* ptr, std::function<void()> fdel, std::function<const GUID()> fguid);

            STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
            STDMETHODIMP_(ULONG) AddRef();
            STDMETHODIMP_(ULONG) Release();

            const bool IsBusy() const;

            STDMETHODIMP OnChannelVolumeChanged(DWORD, float[], DWORD, LPCGUID);
            STDMETHODIMP OnDisplayNameChanged(LPCWSTR, LPCGUID);
            STDMETHODIMP OnGroupingParamChanged(LPCGUID, LPCGUID);
            STDMETHODIMP OnIconPathChanged(LPCWSTR, LPCGUID);
            STDMETHODIMP OnSessionDisconnected(AudioSessionDisconnectReason);
            STDMETHODIMP OnSimpleVolumeChanged(float, BOOL, LPCGUID);
            STDMETHODIMP OnStateChanged(AudioSessionState);
        };
    }
}