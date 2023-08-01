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

        AudioSessionEventsNotify::AudioSessionEventsNotify(std::function<AudioSessionItem* (IAudioSessionControl*&)> ptr) : cbBuild(ptr) {}

        const bool                  AudioSessionEventsNotify::IsBusy() const {
            return (bool)lock__.IsLock();
        }
        HRESULT STDMETHODCALLTYPE   AudioSessionEventsNotify::QueryInterface(REFIID riid, void** ppv) {
            if (riid == __uuidof(IUnknown)) {
                (void)AddRef();
                *ppv = (IUnknown*)this;
                return S_OK;
            }
            if (riid == __uuidof(IAudioSessionNotification)) {
                (void)AddRef();
                *ppv = (IAudioSessionNotification*)this;
                return S_OK;
            }
            *ppv = NULL;
            return E_NOINTERFACE;
        }
        ULONG   STDMETHODCALLTYPE   AudioSessionEventsNotify::AddRef() {
            return InterlockedIncrement(&crefAll);
        }
        ULONG   STDMETHODCALLTYPE   AudioSessionEventsNotify::Release() {
            ULONG cRef = InterlockedDecrement(&crefAll);
            if (cRef == 0) {
                delete this;
            }
            return cRef;
        }
        HRESULT STDMETHODCALLTYPE   AudioSessionEventsNotify::OnSessionCreated(IAudioSessionControl* ptrsc) {
            lock__.Begin();
            try {
                if (ptrsc != nullptr) {
                    AudioSessionItem* item = cbBuild(ptrsc);
                    if (item != nullptr)
                        to_log::Get() << (log_string() << AudioSessionErrors::GetMsg(AudioSessionErrors::ERRMSG_SESSIONCREATED) << item->Item.App.get<std::wstring>());
                }
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            lock__.End();
            return S_OK;
        }
    }
}