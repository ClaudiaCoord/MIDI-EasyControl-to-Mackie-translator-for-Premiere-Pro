/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
    namespace MIXER {

        AudioEndPointEvents::AudioEndPointEvents(AudioSessionList* ptr, std::function<void()> fdel, std::function<const GUID()> fguid)
            : ptrSessionList(ptr), evDeleted(fdel), evGuid(fguid), crefAll(1L) {
            if (ptr == nullptr)
                throw_common_error(common_error_id::err_LISTISNULL);
        }
        const bool AudioEndPointEvents::IsBusy() const {
            return (bool)lock__.IsLock();
        }

        STDMETHODIMP AudioEndPointEvents::QueryInterface(REFIID riid, void** ppv) {
            if (riid == __uuidof(IUnknown)) {
                (void)AddRef();
                *ppv = (IUnknown*)this;
                return S_OK;
            }
            if (riid == __uuidof(IAudioSessionEvents)) {
                (void)AddRef();
                *ppv = (IAudioSessionEvents*)this;
                return S_OK;
            }
            *ppv = NULL;
            return E_NOINTERFACE;
        }
        STDMETHODIMP_(ULONG) AudioEndPointEvents::AddRef() {
            return InterlockedIncrement(&crefAll);
        }
        STDMETHODIMP_(ULONG) AudioEndPointEvents::Release() {
            LONG cRef = InterlockedDecrement(&crefAll);
            if (cRef == 0) {
                evDeleted();
                delete this;
            }
            return cRef;
        }
        HRESULT STDMETHODCALLTYPE AudioEndPointEvents::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA data) {
            if (data == nullptr) return E_INVALIDARG;
            lock__.Begin();

            try {
                const GUID g = evGuid();
                if (data->guidEventContext != g) return S_OK;
                ptrSessionList->VolumeChange(static_cast<LPCGUID>(&data->guidEventContext), data->fMasterVolume, data->bMuted);
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }

            lock__.End();
            return S_OK;
        }
    }
}
