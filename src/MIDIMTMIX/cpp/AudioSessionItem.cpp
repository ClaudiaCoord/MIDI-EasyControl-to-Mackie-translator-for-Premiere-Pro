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

		AudioSessionItem::AudioSessionItem(TypeItems t, DWORD pid, GUID guid, std::wstring name, CComPtr<IAudioSessionControl> ptr, CComPtr<IAudioEndpointVolume> ptrae, float vol, bool m)
			: Typeitem(t), Pid(pid), Guid(guid), Name(name), ptrse__(ptr), ptrae__(ptrae) {
			Volume.SetVolume(vol), Volume.SetMute(m);
			evbuilder__ = std::make_unique<AudioSessionEventsBuilder>();
		}
		AudioSessionItem::AudioSessionItem(TypeItems t, DWORD pid, GUID guid, std::wstring name, CComPtr<IAudioSessionControl> ptrsc, CComPtr<IAudioEndpointVolume> ptrae, std::wstring desc, float vol, bool m)
			: Typeitem(t), Pid(pid), Guid(guid), Name(name), ptrse__(ptrsc), ptrae__(ptrae), Desc(desc) {
			Volume.SetVolume(vol), Volume.SetMute(m);
			evbuilder__ = std::make_unique<AudioSessionEventsBuilder>();
		}
		AudioSessionItem::AudioSessionItem(TypeItems t, DWORD pid, GUID guid, std::wstring name, CComPtr<IAudioSessionControl> ptrsc, CComPtr<IAudioEndpointVolume> ptrae, std::wstring desc, std::wstring ico, float vol, bool m)
			: Typeitem(t), Pid(pid), Guid(guid), Name(name), ptrse__(ptrsc), ptrae__(ptrae), Desc(desc), Icon(ico) {
			Volume.SetVolume(vol), Volume.SetMute(m);
			evbuilder__ = std::make_unique<AudioSessionEventsBuilder>();
		}
		AudioSessionItem::AudioSessionItem(TypeItems t, DWORD pid, GUID guid, std::wstring name, std::wstring path, CComPtr<IAudioSessionControl> ptrsc, CComPtr<IAudioEndpointVolume> ptrae, LPWSTR desc, LPWSTR ico, float vol, bool m)
			: Typeitem(t), Pid(pid), Guid(guid), Name(name), Path(path), ptrse__(ptrsc), ptrae__(ptrae) {
			Volume.Set(vol, m);
			Desc = Common::Utils::to_string(desc);
			Icon = Common::Utils::to_string(ico);
			evbuilder__ = std::make_unique<AudioSessionEventsBuilder>();
		}
		AudioSessionItem::~AudioSessionItem() {
			try {
				evbuilder__.reset();
				if (ptrse__) ptrse__.Release();
				if (ptrae__) ptrae__.Release();
			} catch (...) {}
		}

		void AudioSessionItem::RegistrySession(bool b, AudioSessionList* aslist) {
			try {
				HRESULT h = S_FALSE;

				if (!b) {
					RegistrySession(b);
					return;
				}

				if (ptrse__) {
					AudioSessionEvents* ev;

					if (b && !isregistredse__) {
						if (evbuilder__ && evbuilder__->IsValidSe()) return;
						ev = evbuilder__->GetSe(aslist, std::bind(&AudioSessionItem::GetGuid, this));
						if (ev != nullptr) {
							h = ptrse__->RegisterAudioSessionNotification(ev);
							isregistredse__ = (h == S_OK);
						}
					} else {
						Common::to_log::Get() << (log_string() << L"Audio session item \"" << Name << "\" - bad \"SC\" registred state: " << isregistredse__ << L"/" << b);
					}
				}
				if (ptrae__) {
					AudioEndPointEvents* ev;

					if (b && !isregistredae__) {
						if (evbuilder__ && evbuilder__->IsValidAe()) return;
						ev = evbuilder__->GetAe(aslist, std::bind(&AudioSessionItem::GetGuid, this));
						if (ev != nullptr) {
							h = ptrae__->RegisterControlChangeNotify(ev);
							isregistredae__ = (h == S_OK);
						}
					} else {
						Common::to_log::Get() << (log_string() << L"Audio end point item \"" << Name << "\" - bad \"AE\" registred state: " << isregistredae__ << L"/" << b);
					}
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}
		void AudioSessionItem::RegistrySession(bool b) {
			try {
				HRESULT h = S_FALSE;

				if (ptrse__) {
					AudioSessionEvents* ev;

					if (!b && isregistredse__) {
						if (!evbuilder__ || !evbuilder__->IsValidSe()) return;
						ev = evbuilder__->GetSe();
						if (ev != nullptr) {
							h = ptrse__->UnregisterAudioSessionNotification(ev);
							isregistredse__ = !(h == S_OK);
						}
					} else {
						Common::to_log::Get() << (log_string() << L"Audio session item \"" << Name << "\" - bad \"SC\" registred state: " << isregistredse__ << L"/" << b);
					}
				}
				if (ptrae__) {
					AudioEndPointEvents* ev;

					if (!b && isregistredae__) {
						if (!evbuilder__ || !evbuilder__->IsValidAe()) return;
						ev = evbuilder__->GetAe();
						if (ev != nullptr) {
							h = ptrae__->UnregisterControlChangeNotify(ev);
							isregistredae__ = !(h == S_OK);
						}
					} else {
						Common::to_log::Get() << (log_string() << L"Audio end point item \"" << Name << "\" - bad \"AE\" registred state: " << isregistredae__ << L"/" << b);
					}
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}

		IAudioSessionControl* AudioSessionItem::GetSession() {
			return ptrse__.p;
		}
		IAudioEndpointVolume* AudioSessionItem::GetEndpointVolume() {
			return ptrae__.p;
		}
		AudioSessionItemChange AudioSessionItem::GetSessionItemChange(OnChangeType t) {
			return AudioSessionItemChange(this, t);
		}
		const bool AudioSessionItem::IsValidSession() const {
			return (ptrse__.p != nullptr);
		}
		const bool AudioSessionItem::IsValidEndpointVolume() const {
			return (ptrae__.p != nullptr);
		}

		const GUID AudioSessionItem::GetGuid() const {
			return Guid;
		}
		const uint32_t AudioSessionItem::GetPid() const {
			return Pid;
		}
		const TypeItems AudioSessionItem::GetType() const {
			return Typeitem;
		}
		const std::wstring AudioSessionItem::GetName() const {
			return Name;
		}
		const std::wstring AudioSessionItem::GetDesc() const {
			return Desc;
		}
		const std::wstring AudioSessionItem::GetIcon() const {
			return Icon;
		}
		const std::wstring AudioSessionItem::GetPath() const {
			return Path;
		}
		const bool AudioSessionItem::IsMidiKey() {
			return MidiId.IsKeys();
		}
		const bool AudioSessionItem::IsEmpty() const {
			switch (Typeitem)
			{
			case TypeItems::TypeSession:
				return (Pid == 0) || (Guid == GUID_NULL) || Name.empty();
			case TypeItems::TypeMaster:
				return (Guid == GUID_NULL) || Name.empty();
			case TypeItems::TypeDevice:
				return (Guid == GUID_NULL) || Name.empty();
			case TypeItems::TypeNone:
			default: break;
			}
			return true;
		}

		const std::wstring AudioSessionItem::ToString() {
			if (IsEmpty()) return AudioSessionErrors::GetMsg(AudioSessionErrors::EMPTY);
			std::wstringstream s;
			s << Pid << L") " << Common::Utils::to_string(Guid);
			s << L" [" << (double)Volume.GetVolume() << L"/" << (bool)Volume.GetMute() << L"] - " << Name;
			if (MidiId.IsKeys())
				s << L", " << MidiId.ToString();
			if (!Desc.empty())
				s << L", [" << Desc << L"]";
			if (!Icon.empty())
				s << L", [" << Icon << L"]";
			if (!Path.empty())
				s << L", [" << Path << L"]";
			return s.str();
		}

		void AudioSessionItem::Copy(Common::MIXER::AudioSessionItem& a) {
			if ((a.Pid != Pid) && (a.Pid > 0)) Pid = a.Pid;
			MidiId.Set(a.MidiId.Get());
			Volume.Copy(a.Volume);
			Name = a.GetName();
			Desc = a.GetDesc();
			Icon = a.GetIcon();
			Path = a.GetPath();
		}
	}
}