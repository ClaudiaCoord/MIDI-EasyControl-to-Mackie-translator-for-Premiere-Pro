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

		using namespace std::placeholders;

		AudioSessionMixer AudioSessionMixer::audiosessionmixer__;
		const double AudioSessionMixer::GetVolumeDevider = KEY_DEVIDER;
		AudioSessionItemTitle AudioSessionMixer::MasterControl(L"master");

		/* template */

		template <typename T1>
		void					setValueFromAudioAction_(T1 dev, AudioSessionItemValue& iv, LPCGUID guid) {
			switch (iv.get<OnChangeType>()) {
				case OnChangeType::OnChangeUpdatePan:
				case OnChangeType::OnChangeUpdateVolume: {
					if constexpr (std::is_same_v<T1, ISimpleAudioVolume*>)
						dev->SetMasterVolume(iv.get<float>(), guid);
					else if constexpr (std::is_same_v<T1, IAudioEndpointVolume*>)
						dev->SetMasterVolumeLevelScalar(iv.get<float>(), guid);
					break;
				}
				case OnChangeType::OnChangeUpdateMute: {
					dev->SetMute(iv.get<bool>(), guid);
					break;
				}
				default: break;
			}
		}

		template <typename T1>
		std::tuple<float, bool> getValueFromAudioAction_(AudioAction t, T1 dev, AudioSessionItemValue& iv) {
			switch (t) {
				case AudioAction::AUDIO_VOLUME: {
					float v = 0.0f;
					if constexpr (std::is_same_v<T1, ISimpleAudioVolume*>) {
						if (dev->GetMasterVolume(&v) != S_OK) break;
					}
					else if constexpr (std::is_same_v<T1, IAudioEndpointVolume*>) {
						if (dev->GetMasterVolumeLevelScalar(&v) != S_OK) break;
					}
					iv.set(v);
					return std::tuple<float, bool>(v, false);
				}
				case AudioAction::AUDIO_MUTE: {
					BOOL b;
					if (dev->GetMute(&b) != S_OK) break;
					iv.set(b);
					return std::tuple<float, bool>(0.0f, b);
				}
				default: break;
			}
			return std::tuple<float, bool>();
		}

		template <typename T1, typename T2>
		void					setValueSelector_(AudioSessionItem* item, T1 type, T2 v, bool m) {
			CComPtr<ISimpleAudioVolume> ptrav;

			try {
				if (item == nullptr) return;
				if constexpr (std::is_same_v<T1, MIDI::MidiUnitType>) {
					switch (AudioSessionItemId::normalize_type(type)) {
						case MIDI::MidiUnitType::SLIDER: {
							if (item->Item.Volume.get<uint8_t>() == v) return;
							item->Item.Volume.set(v);
							break;
						}
						case MIDI::MidiUnitType::BTN: {
							if (item->Item.Volume.get<bool>() == m) return;
							item->Item.Volume.set(m);
							break;
						}
						default: return;
					}
				}
				else if constexpr (std::is_same_v<T1, AudioAction>) {
					switch (type) {
						case AudioAction::AUDIO_VOLUME: {
							if (item->Item.Volume.get<T2>() == v) return;
							item->Item.Volume.set<T2>(v);
							break;
						}
						case AudioAction::AUDIO_MUTE: {
							if (item->Item.Volume.get<bool>() == m) return;
							item->Item.Volume.set<bool>(m);
							break;
						}
						case AudioAction::AUDIO_VOLUME_INCREMENT: {
							uint8_t val = item->Item.Volume.get<uint8_t>();
							val = m ? (val + 10) : ((val < 10) ? 0 : (val - 10));
							val = (val > 127) ? 127 : val;
							item->Item.Volume.set<uint8_t>(val);
							break;
						}
						default: return;
					}
				}
				else {
					throw_common_error(common_error_id::err_CBVALUE_BAD_SELECT);
				}
				switch (item->Item.GetType()) {
					case TypeItems::TypeSession: {
						if (!item->IsValidSession()) break;
						if (item->GetSession()->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&ptrav) != S_OK) break;
						setValueFromAudioAction_<ISimpleAudioVolume*>(ptrav.p, item->Item.Volume, static_cast<LPCGUID>(&item->Item.App.Guid));
						break;
					}
					case TypeItems::TypeMaster: {
						if (!item->IsValidEndpointVolume()) return;
						setValueFromAudioAction_<IAudioEndpointVolume*>(item->GetEndpointVolume(), item->Item.Volume, static_cast<LPCGUID>(&item->Item.App.Guid));
						break;
					}
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			if (ptrav) ptrav.Release();
		}

		template<typename T1, typename T2>
		void AudioSessionMixer::SetToAudioSession(AudioAction t, VolumeKeyType kt, const T1 key, T2 v, bool m) {
			try {
				if (!isinit__ || !asList || asList->IsEmpty() || t == AudioAction::AUDIO_NONE) return;

				std::vector<AudioSessionItem*> items{};

				switch (kt) {
					case VolumeKeyType::MIDI_ID_KEY:
						if constexpr (std::is_same_v<T1, uint32_t>)
							items = asList->GetByMidiId(key);
						break;
					case VolumeKeyType::APP_ID_KEY:
						if constexpr (std::is_same_v<T1, uint32_t>)
							items = asList->GetByAppId(key);
						break;
					case VolumeKeyType::APP_NAME_KEY:
						if constexpr (std::is_same_v<T1, std::wstring>)
							items = asList->GetByAppName(key);
						break;
					case VolumeKeyType::GUID_ID_KEY:
						if constexpr (std::is_same_v<T1, GUID>) {
							AudioSessionItem* item = asList->GetByGuid(key);
							if (item == nullptr) break;
							items.push_back(item);
						}
						break;
					default: break;
				}
				if (items.empty()) return;
				for (auto& a : items)
					setValueSelector_(a, t, v, m);

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}
		template void AudioSessionMixer::SetToAudioSession<GUID, float>(AudioAction, VolumeKeyType, const GUID, float, bool);
		template void AudioSessionMixer::SetToAudioSession<GUID, uint8_t>(AudioAction, VolumeKeyType, const GUID, uint8_t, bool);
		template void AudioSessionMixer::SetToAudioSession<uint32_t, float>(AudioAction, VolumeKeyType, const uint32_t, float, bool);
		template void AudioSessionMixer::SetToAudioSession<uint32_t, uint8_t>(AudioAction, VolumeKeyType, const uint32_t, uint8_t, bool);
		template void AudioSessionMixer::SetToAudioSession<std::size_t, float>(AudioAction, VolumeKeyType, const std::size_t, float, bool);
		template void AudioSessionMixer::SetToAudioSession<std::size_t, uint8_t>(AudioAction, VolumeKeyType, const std::size_t, uint8_t, bool);
		template void AudioSessionMixer::SetToAudioSession<std::wstring, float>(AudioAction, VolumeKeyType, const std::wstring, float, bool);
		template void AudioSessionMixer::SetToAudioSession<std::wstring, uint8_t>(AudioAction, VolumeKeyType, const std::wstring, uint8_t, bool);

		template<typename T1>
		std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession(AudioAction t, VolumeKeyType kt, const T1 key) {
			std::tuple<float, bool> tp{};
			do {
				if (!isinit__ || !asList || asList->IsEmpty() || t == AudioAction::AUDIO_NONE) break;

				CComPtr<ISimpleAudioVolume> ptrav;

				try {
					std::vector<AudioSessionItem*> items{};
					switch (kt) {
						case VolumeKeyType::MIDI_ID_KEY:
							if constexpr (std::is_same_v<T1, uint32_t>)
								items = asList->GetByMidiId(key);
							break;
						case VolumeKeyType::APP_ID_KEY:
							if constexpr (std::is_same_v<T1, uint32_t>)
								items = asList->GetByAppId(key);
							break;
						case VolumeKeyType::APP_NAME_KEY:
							if constexpr (std::is_same_v<T1, std::wstring>)
								items = asList->GetByAppName(key);
							break;
						case VolumeKeyType::GUID_ID_KEY:
							if constexpr (std::is_same_v<T1, GUID>) {
								AudioSessionItem* i = asList->GetByGuid(key);
								if (i == nullptr) break;
								items.push_back(i);
							}
							break;
						default: return tp;
					}
					if (items.empty() || (items[0] == nullptr)) break;
					AudioSessionItem* item = items[0];

					if (isfastvalue__)
						return std::make_tuple(item->Item.Volume.get<float>(), item->Item.Volume.get<bool>());

					switch (item->Item.GetType()) {
						case TypeItems::TypeSession: {
							if (!item->IsValidSession()) return tp;
							if (item->GetSession()->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&ptrav) != S_OK) break;
							tp = getValueFromAudioAction_<ISimpleAudioVolume*>(t, ptrav.p, item->Item.Volume);
							break;
						};
						case TypeItems::TypeMaster: {
							if (!item->IsValidEndpointVolume()) return tp;
							return getValueFromAudioAction_<IAudioEndpointVolume*>(t, item->GetEndpointVolume(), item->Item.Volume);
						}
						default: return tp;
					}
				} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }

				if (ptrav) ptrav.Release();

			} while (0);
			return tp;
		}
		template std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<GUID>(AudioAction, VolumeKeyType, const GUID);
		template std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<uint32_t>(AudioAction, VolumeKeyType, const uint32_t);
		template std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<std::size_t>(AudioAction, VolumeKeyType, const std::size_t);
		template std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<std::wstring>(AudioAction, VolumeKeyType, const std::wstring);

		/* class */

		AudioSessionMixer::AudioSessionMixer() : cbid__(0U) {
			cpid__ = GetCurrentProcessId();
			type__ = MIDI::ClassTypes::ClassMixer;
			out1__ = [](MIDI::Mackie::MIDIDATA&, DWORD&) { return false; };
			out2__ = std::bind(static_cast<const bool(AudioSessionMixer::*)(MIDI::MidiUnit&, DWORD&)>(&AudioSessionMixer::InCallBack), this, _1, _2);
			cb_pid_by_name__ = std::bind(static_cast<uint32_t(AudioSessionMixer::*)(std::wstring&)>(&AudioSessionMixer::InternalCallItemPidByName), this, _1);
			id__   = Utils::random_hash(this);
		}
		AudioSessionMixer::~AudioSessionMixer() { release(); }
		AudioSessionMixer& AudioSessionMixer::Get() {
			return std::ref(AudioSessionMixer::audiosessionmixer__);
		}
		callPidByName AudioSessionMixer::GetCbItemPidByName() {
			return cb_pid_by_name__;
		}
		const bool AudioSessionMixer::GetMasterVolumePeak(float* f) {
			try {
				do {
					if (!isinit__ || !ptrAVM) break;
					if (ptrAVM->GetPeakValue(f) != S_OK) break;
					return true;
				} while (0);
			} catch (...) {}
			*f = 0.0f;
			return false;
		}

		void AudioSessionMixer::init(bool iscoinitialize) {
			CComPtr<IMMDevice> pDevice;
			CComPtr<IMMDeviceEnumerator> pDeviceEnumerator;
			iscoinitialize__ = iscoinitialize;

#			pragma warning( push )
#			pragma warning( disable : 4286 )
			try {
				release();

				if (iscoinitialize__) {
					if (::CoInitializeEx(0, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE) != S_OK)
						throw_common_error(common_error_id::err_INIT_COINITIALIZE);
				}
				if (pDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)) != S_OK)
					throw_common_error(common_error_id::err_INIT_COCREATE);

				if (pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice) != S_OK)
					throw_common_error(common_error_id::err_INIT_DEFAULT_AE);

				if (pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, 0, (VOID**)&ptrASM2) != S_OK)
					throw_common_error(common_error_id::err_INIT_AS_MANAGER2);

				if (pDevice->Activate(__uuidof(IAudioMeterInformation), CLSCTX_ALL, 0, (VOID**)&ptrAVM) != S_OK)
					throw_common_error(common_error_id::err_INIT_AS_AUDIOMETER);

				isfastvalue__ = common_config::Get().Registry.GetMixerFastValue();
				isduplicateappremoved__ = common_config::Get().Registry.GetMixerDupAppRemove();
				issetaudioleveloldvalue__ = common_config::Get().Registry.GetMixerSetOldLevelValue();

				asList.reset(new AudioSessionList(
					cpid__,
					isduplicateappremoved__,
					issetaudioleveloldvalue__,
					std::bind(static_cast<void(AudioSessionMixer::*)(AudioSessionUnit&, AudioSessionItem*&)>(&AudioSessionMixer::InternalCallBack), this, _1, _2)
				));
				asCbNotify.reset(new AudioSessionEventsNotify(
					[&](IAudioSessionControl*& p) { return AudioSessionItemBuilder(p); }
				));

				isinit__ = true;

				ptrASM2->RegisterSessionNotification(asCbNotify.get());
				int cnt = getSessionList();

				cbid__ = common_config::Get().add(std::bind(
					static_cast<void(AudioSessionMixer::*)(std::shared_ptr<MIDI::MidiDevice>&)>(&AudioSessionMixer::SetControlUnits), this, _1));
				common_config::Get().Local.IsAudioMixerRun(true);
				to_log::Get() << log_string().to_log_fomat(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_MIXER_STARTOK), cnt);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_MIXER_STARTERROR));
				isinit__ = false;
			}
#			pragma warning( pop )

			if (pDevice) pDevice.Release();
			if (pDeviceEnumerator) pDeviceEnumerator.Release();
			if (!isinit__) {
				if (ptrAVM) ptrAVM.Release();
				if (ptrASM2) ptrASM2.Release();
				if (iscoinitialize__) ::CoUninitialize();
			}
		}
		void AudioSessionMixer::release() {

			if (!isinit__) return;

			try {
				if (asCbNotify)
					ptrASM2->UnregisterSessionNotification(asCbNotify.get());

				if (asList) {
					asList->Clear();
					asList.reset();
				}
				if (ptrAVM) ptrAVM.Release();
				if (ptrASM2) ptrASM2.Release();
				if (iscoinitialize__) ::CoUninitialize();
				if (asCbNotify)	asCbNotify.reset();
				if (cbid__ > 0U) {
					common_config::Get().remove(cbid__);
					cbid__ = 0U;
				}
				onchangevalue__.clear();
				to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_MIXER_STOPOK));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_MIXER_STOPERROR));
			}
			common_config::Get().Local.IsAudioMixerRun(false);
			isinit__ = false;
		}

		/* Event */

		void AudioSessionMixer::event_add(callOnChange fn, uint32_t id) {
			try {
				if (isinit__)  onchangevalue__.add(fn, id);
				if (!asList || asList->IsEmpty()) return;
				
				bool isfirst = true;
				auto& list = asList->GetSessionList();
				for (auto& a : list) {
					onchangevalue__.send(
						a->GetSessionItemChange(OnChangeType::OnChangeNew, isfirst)
					);
					isfirst = false;
				}
					

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			
		}
		void AudioSessionMixer::event_remove(uint32_t id) {
			onchangevalue__.remove(id);
		}
		void AudioSessionMixer::event_send(AudioSessionItemChange asi) {
			onchangevalue__.send(asi);
		}

		/* Public */

		void AudioSessionMixer::Start(bool iscoinitialize) {
			init(iscoinitialize);
		}
		void AudioSessionMixer::Stop() {
			release();
		}

		uint32_t AudioSessionMixer::GetCurrentPid() {
			return cpid__;
		}
		uint32_t AudioSessionMixer::InternalCallItemPidByName(std::wstring& name) {
			try {
				std::vector<AudioSessionItem*> items = asList->GetByAppName(name);
				if (!items.empty() && (items[0] != nullptr)) return items[0]->Item.GetPid();
			} catch (...) {}
			return 0U;
		}
		std::wstring AudioSessionMixer::SessionListToString() {
			try {
				if (!isinit__)
					return common_error_code::Get().get_error(common_error_id::err_NO_INIT);
				if (!asList || asList->IsEmpty())
					return common_error_code::Get().get_error(common_error_id::err_EMPTY);

				std::wstringstream s;
				s << L"-------------------------------------- " << asList->Count() << L" ------------------------------------------" << std::endl;
				s << L" PID                 GUID                 VOLUME/MUTE    NAME" << std::endl;
				s << L"------------------------------------------------------------------------------------" << std::endl;
				auto& list = asList->GetSessionList();
				for (AudioSessionItem* item : list) {
					s << item->to_string().str() << std::endl;
				}
				s << L"------------------------------------------------------------------------------------" << std::endl;
				return s.str();
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return common_error_code::Get().get_error(common_error_id::err_SESSION_LIST);
		}
		void  AudioSessionMixer::SetControlUnits(std::shared_ptr<MIDI::MidiDevice>& md) {
			asList->SetControlUnits(md);
		}

		/* Private */

		const bool AudioSessionMixer::InCallBack(MIDI::MidiUnit& m, DWORD& d) {
			if (!isinit__ || !asList) return false;

			try {
				std::vector<AudioSessionItem*> items = asList->GetByMidiId(m.GetMixerId());
				if (items.empty()) return false;
				for (auto& a : items) {
					if (a != nullptr) {
						if (a->Last >= m.value.time) continue;
						a->Last = m.value.time;
						setValueSelector_(a, m.type, m.value.value, m.value.lvalue);
					}
				}
				return true;
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
		void AudioSessionMixer::InternalCallBack(AudioSessionUnit& u, AudioSessionItem*& item) {
			if (!isinit__ || !asList) return;

			try {
				setValueSelector_(item, u.GetType(), u.GetVolume(), u.GetMute());
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}

		CComPtr<IAudioEndpointVolume> AudioSessionMixer::getMainEndpointVolume() {
			CComPtr<IMMDevice> ptrdev;
			CComPtr<IMMDeviceEnumerator> ptrdevenum;
			CComPtr<IAudioEndpointVolume> ptrae;

			try {
				do {
					if (ptrdevenum.CoCreateInstance(__uuidof(MMDeviceEnumerator)) != S_OK) break;
					if (ptrdevenum->GetDefaultAudioEndpoint(eRender, eMultimedia, &ptrdev) != S_OK) break;
					if (ptrdev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (VOID**)&ptrae) != S_OK) break;
				} while (0);
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }

			if (ptrdev) ptrdev.Release();
			if (ptrdevenum) ptrdevenum.Release();
			return ptrae;
		}
		int AudioSessionMixer::getSessionList() {
			int cnt = 0;
			try {
				CComPtr<IAudioSessionEnumerator> ptrse;
				try {
					do {
						if (ptrASM2->GetSessionEnumerator(&ptrse) != S_OK) return -1;
						if ((ptrse->GetCount(&cnt) != S_OK) || (cnt == 0)) break;

						asList->Clear();

						for (int i = 0; i < cnt; i++) {
							CComPtr<IAudioSessionControl> ptrsc;
							try {
								if (ptrse->GetSession(i, &ptrsc) != S_OK) continue;
								(void)AudioSessionItemBuilder(ptrsc);
								ptrsc.Release();
							} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
						}
					} while (0);
				} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); cnt = -1; }
				if (ptrse) ptrse.Release();
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__);  cnt = -1; }
			return cnt;
		}
		std::vector<AudioSessionItem*>& AudioSessionMixer::GetSessionList() {
			return asList->GetSessionList();
		}

		AudioSessionItem* AudioSessionMixer::AudioSessionItemBuilder(IAudioSessionControl* ptrsc) {

			CComPtr<IAudioSessionControl2> ptrSc2;
			CComPtr<ISimpleAudioVolume> ptrSav;
			LPWSTR desc = nullptr, icon = nullptr;
			AudioSessionItem* item = nullptr;

			try {
				AudioSessionState state;
				if ((ptrsc == nullptr) || (ptrsc->GetState(&state) != S_OK))
					return nullptr;

				GUID guid;
				if (ptrsc->GetGroupingParam(&guid) != S_OK) return nullptr;

				switch (state) {
					case AudioSessionState::AudioSessionStateExpired: {
						asList->RemoveByGuid(guid);
						return nullptr;
					}
					case AudioSessionState::AudioSessionStateActive:
					case AudioSessionState::AudioSessionStateInactive:
					default: break;
				}

				do {
					if (ptrsc->QueryInterface(__uuidof(IAudioSessionControl2), (VOID**)&ptrSc2) != S_OK) break;
					if (ptrsc->QueryInterface(__uuidof(ISimpleAudioVolume), (VOID**)&ptrSav) != S_OK) break;

					DWORD pid = 0U;
					BOOL mute;
					float vol;
					std::wstring name{};
					std::wstring path{};
					TypeItems ti = TypeItems::TypeSession;
					CComPtr<IAudioEndpointVolume> ptrae;

					ptrSc2->GetProcessId(&pid);
					if (pid == cpid__) break;
					if (pid == 0U)
						ti = TypeItems::TypeMaster;
					else
						ti = TypeItems::TypeSession;

					if (ptrSav->GetMasterVolume(&vol) != S_OK) break;
					if (ptrSav->GetMute(&mute) != S_OK) break;

					if (ptrsc->GetIconPath(&icon) != S_OK) icon = nullptr;
					if (ptrsc->GetDisplayName(&desc) != S_OK) desc = nullptr;
					if (!CHECK_LPWSTRING(icon))
						if (ptrSc2->GetIconPath(&icon) != S_OK) icon = nullptr;
					if (!CHECK_LPWSTRING(desc))
						if (ptrSc2->GetDisplayName(&desc) != S_OK) desc = nullptr;

					if (ti == TypeItems::TypeSession) {
						std::tuple<bool, std::wstring, std::wstring> t = Utils::pid_to_string(pid);
						if (!std::get<0>(t)) break;

						path = std::get<2>(t);
						std::wstring nm = std::get<1>(t);
						if (nm.empty()) {
							if (!CHECK_LPWSTRING(desc)) break;
							nm = Utils::to_string(desc);
							LPWSTR w = desc;
							::CoTaskMemFree(w);
							desc = nullptr;
						}
						if (nm.empty()) {
							asList->RemoveByGuid(guid);
							break;
						}
						name = Utils::trim(nm);
					}
					else if (ti == TypeItems::TypeMaster) {
						name = AudioSessionMixer::MasterControl.GetName();
						ptrae = getMainEndpointVolume();
					} else {
						asList->RemoveByGuid(guid);
						break;
					}

					CComPtr<IAudioSessionControl> ptr(ptrsc);
					item = new AudioSessionItem(ti, pid, guid, name, path, std::move(ptr), std::move(ptrae), (LPWSTR)desc, (LPWSTR)icon, vol, static_cast<bool>(mute));
					asList->Add(item);

				} while (0);
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }

			ptrSav.Release();
			ptrSc2.Release();
			if (desc != nullptr) ::CoTaskMemFree(desc);
			if (icon != nullptr) ::CoTaskMemFree(icon);
			return item;
		}
	}
}