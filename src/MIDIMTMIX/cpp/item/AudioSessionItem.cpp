/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIXER {

		AudioSessionItem::AudioSessionItem(TypeItems t, DWORD pid, GUID guid, std::wstring name, CComPtr<IAudioSessionControl> ptr, CComPtr<IAudioEndpointVolume> ptrae, float vol, bool m)
			: ptrse_(ptr), ptrae_(ptrae) {
			Item.SetType(t);
			Item.Volume.setall(vol, m);
			Item.App.set(name, guid, pid, L"", L"", L"");
			evbuilder_ = std::make_unique<AudioSessionEventsBuilder>();
		}
		AudioSessionItem::AudioSessionItem(TypeItems t, DWORD pid, GUID guid, std::wstring name, CComPtr<IAudioSessionControl> ptrsc, CComPtr<IAudioEndpointVolume> ptrae, std::wstring desc, float vol, bool m)
			: ptrse_(ptrsc), ptrae_(ptrae) {
			Item.SetType(t);
			Item.Volume.setall(vol, m);
			Item.App.set(name, guid, pid, L"", Utils::to_string(desc), L"");
			evbuilder_ = std::make_unique<AudioSessionEventsBuilder>();
		}
		AudioSessionItem::AudioSessionItem(TypeItems t, DWORD pid, GUID guid, std::wstring name, CComPtr<IAudioSessionControl> ptrsc, CComPtr<IAudioEndpointVolume> ptrae, std::wstring desc, std::wstring ico, float vol, bool m)
			: ptrse_(ptrsc), ptrae_(ptrae) {
			Item.SetType(t);
			Item.Volume.setall(vol, m);
			Item.App.set(name, guid, pid, L"", Utils::to_string(desc), Utils::to_string(ico));
			evbuilder_ = std::make_unique<AudioSessionEventsBuilder>();
		}
		AudioSessionItem::AudioSessionItem(TypeItems t, DWORD pid, GUID guid, std::wstring name, std::wstring path, CComPtr<IAudioSessionControl> ptrsc, CComPtr<IAudioEndpointVolume> ptrae, LPWSTR desc, LPWSTR ico, float vol, bool m)
			: ptrse_(ptrsc), ptrae_(ptrae) {
			Item.SetType(t);
			Item.Volume.setall(vol, m);
			Item.App.set(name, guid, pid, path, Utils::to_string(desc), Utils::to_string(ico));
			evbuilder_ = std::make_unique<AudioSessionEventsBuilder>();
		}
		AudioSessionItem::~AudioSessionItem() {
			try {
				evbuilder_.reset();
				if (ptrse_) ptrse_.Release();
				if (ptrae_) ptrae_.Release();
			} catch (...) {}
		}

		/* Sessions */

		void AudioSessionItem::RegistrySession(bool b, AudioSessionList* aslist) {
			try {
				HRESULT h = S_FALSE;

				if (!b) {
					RegistrySession(b);
					return;
				}

				if (ptrse_) {
					AudioSessionEvents* ev;

					if (b && !isregistred_se_) {
						if (evbuilder_ && evbuilder_->IsValidSe()) return;
						ev = evbuilder_->GetSe(aslist, std::bind(&AudioSessionItemBase::GetGuid, &this->Item));
						if (ev != nullptr) {
							h = ptrse_->RegisterAudioSessionNotification(ev);
							isregistred_se_ = (h == S_OK);
						}
					} else {
						to_log::Get() << log_string().to_log_format(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_ITEM_SC_AE),
							Item.App.get<std::wstring>(), L"SC",
							isregistred_se_.load(), b
						);
					}
				}
				if (ptrae_) {
					AudioEndPointEvents* ev;

					if (b && !isregistred_ae_) {
						if (evbuilder_ && evbuilder_->IsValidAe()) return;
						ev = evbuilder_->GetAe(aslist, std::bind(&AudioSessionItemBase::GetGuid, &this->Item));
						if (ev != nullptr) {
							h = ptrae_->RegisterControlChangeNotify(ev);
							isregistred_ae_ = (h == S_OK);
						}
					} else {
						to_log::Get() << log_string().to_log_format(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_ITEM_SC_AE),
							Item.App.get<std::wstring>(), L"AE",
							isregistred_ae_.load(), b
						);
					}
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}
		void AudioSessionItem::RegistrySession(bool b) {
			try {
				HRESULT h = S_FALSE;

				if (ptrse_) {
					AudioSessionEvents* ev;

					if (!b && isregistred_se_) {
						if (!evbuilder_ || !evbuilder_->IsValidSe()) return;
						ev = evbuilder_->GetSe();
						if (ev != nullptr) {
							h = ptrse_->UnregisterAudioSessionNotification(ev);
							isregistred_se_ = !(h == S_OK);
						}
					} else {
						to_log::Get() << log_string().to_log_format(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_ITEM_SC_AE),
							Item.App.get<std::wstring>(), L"SC",
							isregistred_se_.load(), b
						);
					}
				}
				if (ptrae_) {
					AudioEndPointEvents* ev;

					if (!b && isregistred_ae_) {
						if (!evbuilder_ || !evbuilder_->IsValidAe()) return;
						ev = evbuilder_->GetAe();
						if (ev != nullptr) {
							h = ptrae_->UnregisterControlChangeNotify(ev);
							isregistred_ae_ = !(h == S_OK);
						}
					} else {
						to_log::Get() << log_string().to_log_format(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_ITEM_SC_AE),
							Item.App.get<std::wstring>(), L"AE",
							isregistred_ae_.load(), b
						);
					}
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}

		IAudioSessionControl* AudioSessionItem::GetSession() {
			return ptrse_.p;
		}
		IAudioEndpointVolume* AudioSessionItem::GetEndpointVolume() {
			return ptrae_.p;
		}
		AudioSessionItemChange AudioSessionItem::GetSessionItemChange(OnChangeType t, bool isevent) {
			return AudioSessionItemChange(this, t, isevent);
		}
		const bool AudioSessionItem::IsEmpty() const {
			return ((!Item.GetPid() && (Item.GetType() != TypeItems::TypeMaster)) || (Item.GetGuid() == GUID_NULL) || (Item.GetType() == TypeItems::TypeNone));
		}
		const bool AudioSessionItem::IsValidSession() const {
			return (ptrse_.p != nullptr);
		}
		const bool AudioSessionItem::IsValidEndpointVolume() const {
			return (ptrae_.p != nullptr);
		}

		log_string AudioSessionItem::to_string() {
			try {
				log_string s{};
				if (Item.IsEmptyApp()) {
					s << common_error_code::Get().get_error(common_error_id::err_EMPTY);
				}
				else {
					s << Item.App.to_string();
					s << L", " << Item.Volume.to_string();
					if (Item.Id) s << L"\n" << Item.Id.to_string();
				}
				return s;
			} catch (...) {}
			return log_string();
		}

		void AudioSessionItem::Copy(AudioSessionItem& a) {
			Item.SetType(a.Item.GetType());
			Item.App.copy(a);
			Item.Id.copy(a);
			Item.Volume.copy(a.Item.Volume, a.Item.Volume.get<OnChangeType>());
		}
	}
}