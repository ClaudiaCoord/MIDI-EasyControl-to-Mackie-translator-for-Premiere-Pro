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

        AudioSessionEvents::AudioSessionEvents(AudioSessionList* ptr, std::function<void()> fdel, std::function<const GUID()> fguid)
            : ptrSessionList(ptr), evDeleted(fdel), evGuid(fguid), crefAll(1L) {
            if (ptr == nullptr)
                throw make_common_error(common_error_id::err_LISTISNULL);
        }
        const bool AudioSessionEvents::IsBusy() const {
            return (bool)lock_.IsLock();
        }

        STDMETHODIMP AudioSessionEvents::QueryInterface(REFIID riid, void** ppv)
        {
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
        STDMETHODIMP_(ULONG) AudioSessionEvents::AddRef() {
            return InterlockedIncrement(&crefAll);
        }
        STDMETHODIMP_(ULONG) AudioSessionEvents::Release() {
            LONG cRef = InterlockedDecrement(&crefAll);
            if (cRef == 0) {
                evDeleted();
                delete this;
            }
            return cRef;
        }

        STDMETHODIMP AudioSessionEvents::OnSimpleVolumeChanged(float v, BOOL m, LPCGUID g) {
            lock_.Begin();
            ptrSessionList->VolumeChange(g, v, m);
            lock_.End();
            return S_OK;
        }
        STDMETHODIMP AudioSessionEvents::OnDisplayNameChanged(LPCWSTR s, LPCGUID g) {
            lock_.Begin();
            ptrSessionList->DisplayNameChange(g, s);
            lock_.End();
            return S_OK;
        }
        STDMETHODIMP AudioSessionEvents::OnIconPathChanged(LPCWSTR s, LPCGUID g) {
            lock_.Begin();
            ptrSessionList->IconPathChange(g, s);
            lock_.End();
            return S_OK;
        }
        STDMETHODIMP AudioSessionEvents::OnChannelVolumeChanged(DWORD cnt, float f[], DWORD idx, LPCGUID g) {
            lock_.Begin();
            if ((idx > 0U) && (idx < cnt))
                ptrSessionList->VolumeChange(g, f[idx - 1], false);
            lock_.End();
            return S_OK;
        }
        STDMETHODIMP AudioSessionEvents::OnGroupingParamChanged(LPCGUID gn, LPCGUID go) {
            lock_.Begin();
            ptrSessionList->GuidChange(go, gn);
            lock_.End();
            return S_OK;
        }
        STDMETHODIMP AudioSessionEvents::OnStateChanged(AudioSessionState s) {
            lock_.Begin();
            switch (s) {
                
                case AudioSessionState::AudioSessionStateExpired: {
                    try {
                        const GUID g = evGuid();
                        ptrSessionList->RemoveByGuid(g);
                        
                    } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                    break;
                }
                case AudioSessionState::AudioSessionStateActive:
                case AudioSessionState::AudioSessionStateInactive:
                default: break;
            }
            lock_.End();
            return S_OK;
        }
        STDMETHODIMP AudioSessionEvents::OnSessionDisconnected(AudioSessionDisconnectReason t) {
            to_log::Get() << log_string().to_log_format(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_SESSIONDISCONNECTED), AudioSessionHelper::DisconnectReasonHelper(t));
            return S_OK;
        }
    }
}