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
			std::atomic<bool> isregistredse__ = false;
			std::atomic<bool> isregistredae__ = false;
			std::unique_ptr<AudioSessionEventsBuilder> evbuilder__;
			CComPtr<IAudioSessionControl> ptrse__;
			CComPtr<IAudioEndpointVolume> ptrae__;

		public:

			/* Session type */
			TypeItems Typeitem = TypeItems::TypeNone;

			/* Session keys */
			GUID Guid = GUID_NULL;
			uint32_t Pid = 0U;
			uint32_t Last = 0U;
			std::wstring Name;
			std::wstring Desc;
			std::wstring Icon;
			std::wstring Path;

			/* Volume values */
			AudioVolumeValue Volume{};

			/* MIDI keys */
			AudioSessionItemMidi MidiId;

			AudioSessionItem() = delete;
			FLAG_EXPORT ~AudioSessionItem();
			AudioSessionItem(TypeItems i, DWORD pid, GUID guid, std::wstring name, CComPtr<IAudioSessionControl> ptrsc, CComPtr<IAudioEndpointVolume> ptrae, float vol, bool m);
			AudioSessionItem(TypeItems i, DWORD pid, GUID guid, std::wstring name, CComPtr<IAudioSessionControl> ptrsc, CComPtr<IAudioEndpointVolume> ptrae, std::wstring desc, float vol, bool m);
			AudioSessionItem(TypeItems i, DWORD pid, GUID guid, std::wstring name, CComPtr<IAudioSessionControl> ptscr, CComPtr<IAudioEndpointVolume> ptrae, std::wstring desc, std::wstring ico, float vol, bool m);
			AudioSessionItem(TypeItems i, DWORD pid, GUID guid, std::wstring name, std::wstring path, CComPtr<IAudioSessionControl> ptrsc, CComPtr<IAudioEndpointVolume> ptrae, LPWSTR desc, LPWSTR ico, float vol, bool m);

			void RegistrySession(bool b);
			void RegistrySession(bool b, AudioSessionList* aslist);
			IAudioSessionControl* GetSession();
			IAudioEndpointVolume* GetEndpointVolume();
			AudioSessionItemChange GetSessionItemChange(OnChangeType);
			const bool IsValidSession() const;
			const bool IsValidEndpointVolume() const;

			FLAG_EXPORT const bool IsEmpty() const;
			FLAG_EXPORT const bool IsMidiKey();

			FLAG_EXPORT const GUID GetGuid() const;
			FLAG_EXPORT const uint32_t GetPid() const;
			FLAG_EXPORT const TypeItems GetType() const;
			FLAG_EXPORT const std::wstring GetName() const;
			FLAG_EXPORT const std::wstring GetDesc() const;
			FLAG_EXPORT const std::wstring GetIcon() const;
			FLAG_EXPORT const std::wstring GetPath() const;

			FLAG_EXPORT const std::wstring ToString();
			FLAG_EXPORT void  Copy(AudioSessionItem& a);
		};
	}
}