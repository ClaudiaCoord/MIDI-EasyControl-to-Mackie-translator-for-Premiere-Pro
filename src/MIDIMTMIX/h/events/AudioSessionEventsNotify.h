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

		class AudioSessionEventsNotify : public IAudioSessionNotification
		{
		private:
			LONG crefAll = 1L;
			locker_awaiter lock_{};
			std::function<void(IAudioSessionControl*)> build_cb_;

		public:
			AudioSessionEventsNotify(std::function<void(IAudioSessionControl*)>);

			const bool IsBusy() const;

			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);
			HRESULT STDMETHODCALLTYPE OnSessionCreated(IAudioSessionControl* pSession);
			ULONG   STDMETHODCALLTYPE AddRef();
			ULONG   STDMETHODCALLTYPE Release();
		};
	}
}
