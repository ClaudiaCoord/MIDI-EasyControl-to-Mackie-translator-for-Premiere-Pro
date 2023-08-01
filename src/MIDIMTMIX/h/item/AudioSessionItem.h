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

		class AudioSessionItem
		{
		private:
			/* Notify event builder */
			std::atomic<bool> isregistred_se__{ false };
			std::atomic<bool> isregistred_ae__{ false };
			std::unique_ptr<AudioSessionEventsBuilder> evbuilder__;
			CComPtr<IAudioSessionControl> ptrse__;
			CComPtr<IAudioEndpointVolume> ptrae__;

		public:

			/* Session keys */
			uint32_t Last = 0U;

			AudioSessionItemBase Item{};

			/* Session type:		TypeItems Typeitem = TypeItems::TypeNone; */
			/* Application values:	AudioSessionItemApp App; */
			/* Volume values:		AudioSessionItemValue Volume; */
			/* MIDI keys:			AudioSessionItemId Id; */

			AudioSessionItem() = delete;
			FLAG_EXPORT ~AudioSessionItem();
			AudioSessionItem(TypeItems i, DWORD pid, GUID guid, std::wstring name, CComPtr<IAudioSessionControl> ptrsc, CComPtr<IAudioEndpointVolume> ptrae, float vol, bool m);
			AudioSessionItem(TypeItems i, DWORD pid, GUID guid, std::wstring name, CComPtr<IAudioSessionControl> ptrsc, CComPtr<IAudioEndpointVolume> ptrae, std::wstring desc, float vol, bool m);
			AudioSessionItem(TypeItems i, DWORD pid, GUID guid, std::wstring name, CComPtr<IAudioSessionControl> ptscr, CComPtr<IAudioEndpointVolume> ptrae, std::wstring desc, std::wstring ico, float vol, bool m);
			AudioSessionItem(TypeItems i, DWORD pid, GUID guid, std::wstring name, std::wstring path, CComPtr<IAudioSessionControl> ptrsc, CComPtr<IAudioEndpointVolume> ptrae, LPWSTR desc, LPWSTR ico, float vol, bool m);

			FLAG_EXPORT void  Copy(AudioSessionItem& a);

			FLAG_EXPORT log_string to_string();

			/* Sessions */

			void RegistrySession(bool b);
			void RegistrySession(bool b, AudioSessionList* aslist);
			IAudioSessionControl* GetSession();
			IAudioEndpointVolume* GetEndpointVolume();
			AudioSessionItemChange GetSessionItemChange(OnChangeType, bool);
			const bool IsValidSession() const;
			const bool IsValidEndpointVolume() const;

		};
	}
}