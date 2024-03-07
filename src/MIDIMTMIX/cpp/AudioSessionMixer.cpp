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

		using namespace std::placeholders;

		const double AudioSessionMixer::GetVolumeDevider{ KEY_DEVIDER };
		AudioSessionItemTitle AudioSessionMixer::MasterControl(L"master");

		/* template */

		template <typename T1>
		void setValueFromAudioAction_(T1 dev, AudioSessionItemValue& iv, LPCGUID guid) {
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
		void setValueSelector_(ASITEM_t& item, T1 type, T2 v, bool m) {
			CComPtr<ISimpleAudioVolume> ptrav;

			try {
				if (!item || item->IsEmpty()) return;
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
					throw make_common_error(common_error_id::err_CBVALUE_BAD_SELECT);
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
				if (!isinit_ || !asList || asList->IsEmpty() || t == AudioAction::AUDIO_NONE) return;

				ASLIST_t items{};

				switch (kt) {
					case VolumeKeyType::MIDI_ID_KEY: {
						if constexpr (std::is_same_v<T1, uint32_t>)
							items = asList->GetByMidiId(key);
						break;
					}
					case VolumeKeyType::APP_ID_KEY: {
						if constexpr (std::is_same_v<T1, uint32_t>)
							items = asList->GetByAppId(key);
						break;
					}
					case VolumeKeyType::APP_NAME_KEY: {
						if constexpr (std::is_same_v<T1, std::wstring>)
							items = asList->GetByAppName(key);
						break;
					}
					case VolumeKeyType::GUID_ID_KEY: {
						if constexpr (std::is_same_v<T1, GUID>) {
							ASITEM_t& item = asList->GetByGuid(key);
							if (!item || item->IsEmpty()) break;
							items.push_back(item);
						}
						break;
					}
					default: break;
				}
				if (items.empty()) return;
				for (auto& a : items)
					setValueSelector_(a, t, v, m);

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}
		template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<GUID, float>(AudioAction, VolumeKeyType, const GUID, float, bool);
		template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<GUID, uint8_t>(AudioAction, VolumeKeyType, const GUID, uint8_t, bool);
		template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<uint32_t, float>(AudioAction, VolumeKeyType, const uint32_t, float, bool);
		template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<uint32_t, uint8_t>(AudioAction, VolumeKeyType, const uint32_t, uint8_t, bool);
		template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<std::size_t, float>(AudioAction, VolumeKeyType, const std::size_t, float, bool);
		template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<std::size_t, uint8_t>(AudioAction, VolumeKeyType, const std::size_t, uint8_t, bool);
		template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<std::wstring, float>(AudioAction, VolumeKeyType, const std::wstring, float, bool);
		template FLAG_EXPORT void AudioSessionMixer::SetToAudioSession<std::wstring, uint8_t>(AudioAction, VolumeKeyType, const std::wstring, uint8_t, bool);

		template<typename T1>
		std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession(AudioAction t, VolumeKeyType kt, const T1 key) {
			std::tuple<float, bool> tp{};
			do {
				if (!isinit_ || !asList || asList->IsEmpty() || t == AudioAction::AUDIO_NONE) break;

				CComPtr<ISimpleAudioVolume> ptrav;

				try {

					ASLIST_t items{};

					switch (kt) {
						case VolumeKeyType::MIDI_ID_KEY: {
							if constexpr (std::is_same_v<T1, uint32_t>)
								items = asList->GetByMidiId(key);
							break;
						}
						case VolumeKeyType::APP_ID_KEY: {
							if constexpr (std::is_same_v<T1, uint32_t>)
								items = asList->GetByAppId(key);
							break;
						}
						case VolumeKeyType::APP_NAME_KEY: {
							if constexpr (std::is_same_v<T1, std::wstring>)
								items = asList->GetByAppName(key);
							break;
						}
						case VolumeKeyType::GUID_ID_KEY: {
							if constexpr (std::is_same_v<T1, GUID>) {
								ASITEM_t& item = asList->GetByGuid(key);
								if (!item || item->IsEmpty()) break;
								items.push_back(item);
							}
							break;
						}
						default: return tp;
					}
					if (items.empty() || !items[0]) break;
					ASITEM_t& item = items[0];

					if (isfastvalue_)
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
		template FLAG_EXPORT std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<GUID>(AudioAction, VolumeKeyType, const GUID);
		template FLAG_EXPORT std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<uint32_t>(AudioAction, VolumeKeyType, const uint32_t);
		template FLAG_EXPORT std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<std::size_t>(AudioAction, VolumeKeyType, const std::size_t);
		template FLAG_EXPORT std::tuple<float, bool> AudioSessionMixer::GetFromAudioSession<std::wstring>(AudioAction, VolumeKeyType, const std::wstring);

		/* AudioSessionMixer */

		AudioSessionMixer::AudioSessionMixer()
			: cpid_(::GetCurrentProcessId()), Volume(this),
			  PluginCb::PluginCb(this, IO::PluginClassTypes::ClassMixer, (IO::PluginCbType::Out2Cb | IO::PluginCbType::ConfCb | IO::PluginCbType::PidCb)) {
			PluginCb::out2_cb_ = std::bind(static_cast<void(AudioSessionMixer::*)(MIDI::MidiUnit&, DWORD)>(&AudioSessionMixer::cb_out_call_), this, _1, _2);
			PluginCb::cnf_cb_ = std::bind(static_cast<void(AudioSessionMixer::*)(std::shared_ptr<JSON::MMTConfig>&)>(&AudioSessionMixer::set_config_cb_), this, _1);
			PluginCb::pid_cb_ = std::bind(static_cast<uint32_t(AudioSessionMixer::*)(std::wstring&)>(&AudioSessionMixer::internal_pid_by_name_), this, _1);
		}
		AudioSessionMixer::~AudioSessionMixer() { release_(); }

		IO::callGetPid_t AudioSessionMixer::GetCbItemPidByName() {
			return PluginCb::pid_cb_;
		}
		const bool AudioSessionMixer::GetMasterVolumePeak(float* f) {
			try {
				do {
					if (!isinit_ || !ptrAVM) break;
					if (ptrAVM->GetPeakValue(f) != S_OK) break;
					return true;
				} while (0);
			} catch (...) {}
			*f = 0.0f;
			return false;
		}

		/* Private main Actions */

		void AudioSessionMixer::init_(bool iscoinitialize) {
			CComPtr<IMMDevice> pDevice;
			CComPtr<IMMDeviceEnumerator> pDeviceEnumerator;
			iscoinitialize_ = iscoinitialize;

#			pragma warning( push )
#			pragma warning( disable : 4286 )
			try {
				release_();

				if (iscoinitialize_) {
					if (::CoInitializeEx(0, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE) != S_OK)
						throw make_common_error(common_error_id::err_INIT_COINITIALIZE);
				}
				if (pDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)) != S_OK)
					throw make_common_error(common_error_id::err_INIT_COCREATE);

				if (pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice) != S_OK)
					throw make_common_error(common_error_id::err_INIT_DEFAULT_AE);

				if (pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, 0, (VOID**)&ptrASM2) != S_OK)
					throw make_common_error(common_error_id::err_INIT_AS_MANAGER2);

				if (pDevice->Activate(__uuidof(IAudioMeterInformation), CLSCTX_ALL, 0, (VOID**)&ptrAVM) != S_OK)
					throw make_common_error(common_error_id::err_INIT_AS_AUDIOMETER);

				common_config& cnf = common_config::Get();
				isfastvalue_ = cnf.Registry.GetMixerFastValue();
				isduplicateappremoved_ = cnf.Registry.GetMixerDupAppRemove();
				issetaudioleveloldvalue_ = cnf.Registry.GetMixerSetOldLevelValue();

				asList.reset(new AudioSessionList(
					cpid_,
					isduplicateappremoved_,
					issetaudioleveloldvalue_,
					std::bind(static_cast<void(AudioSessionMixer::*)(ASUNIT_t&, ASITEM_t&)>(&AudioSessionMixer::internal_cb_), this, _1, _2),
					std::bind(static_cast<void(AudioSessionMixer::*)(AudioSessionItemChange)>(&AudioSessionMixer::event_send_cb_), this, _1)
				));
				asCbNotify.reset(new AudioSessionEventsNotify(
					[&](IAudioSessionControl* p) { AudioSessionItemBuilder(p); }
				));

				IO::IOBridge& br = Common::IO::IOBridge::Get();
				br.SetCb(*static_cast<IO::PluginCb*>(this));

				isinit_ = true;
				ptrASM2->RegisterSessionNotification(asCbNotify.get());
				int cnt = getSessionList();

				common_config::Get().Local.IsAudioMixerRun(true);
				to_log::Get() << log_string().to_log_format(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_MIXER_STARTOK), cnt);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_MIXER_STARTERROR));
				isinit_ = false;
			}
#			pragma warning( pop )

			if (pDevice) pDevice.Release();
			if (pDeviceEnumerator) pDeviceEnumerator.Release();
			if (!isinit_) {
				if (ptrAVM) ptrAVM.Release();
				if (ptrASM2) ptrASM2.Release();
				if (iscoinitialize_) ::CoUninitialize();
			}
		}
		void AudioSessionMixer::release_() {

			if (!isinit_) return;

			try {

				IO::IOBridge& br = Common::IO::IOBridge::Get();
				br.UnSetCb(*static_cast<IO::PluginCb*>(this));

				if (asCbNotify)
					ptrASM2->UnregisterSessionNotification(asCbNotify.get());

				if (asList) {
					asList->Clear();
					asList.reset();
				}
				if (ptrAVM) ptrAVM.Release();
				if (ptrASM2) ptrASM2.Release();
				if (iscoinitialize_) ::CoUninitialize();
				if (asCbNotify)	asCbNotify.reset();

				onchangevalue_.clear();
				to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_MIXER_STOPOK));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_MIXER_STOPERROR));
			}
			common_config::Get().Local.IsAudioMixerRun(false);
			isinit_ = false;
		}

		/* Event */

		void AudioSessionMixer::event_add(callOnChange_t fn, uint32_t id) {
			try {
				if (isinit_) onchangevalue_.add(fn, id);
				if (!asList || asList->IsEmpty()) return;
				
				bool isfirst = true;
				auto& list = asList->GetSessionList();
				for (auto& a : list) {
					onchangevalue_.send(
						std::move(a->GetSessionItemChange(OnChangeType::OnChangeNew, isfirst))
					);
					isfirst = false;
				}
					

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void AudioSessionMixer::event_remove(uint32_t id) {
			try {
				if (isinit_) onchangevalue_.remove(id);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void AudioSessionMixer::event_send_cb_(AudioSessionItemChange asi) {
			if (isinit_) onchangevalue_.send(asi);
		}

		/* Public main Actions */

		void AudioSessionMixer::Start(bool iscoinitialize) {
			init_(iscoinitialize);
		}
		void AudioSessionMixer::Stop() {
			release_();
		}

		uint32_t AudioSessionMixer::GetCurrentPid() {
			return cpid_;
		}
		std::wstring AudioSessionMixer::SessionListToString() {
			try {
				if (!isinit_)
					return common_error_code::Get().get_error(common_error_id::err_NO_INIT);
				if (!asList || asList->IsEmpty())
					return common_error_code::Get().get_error(common_error_id::err_EMPTY);

				std::wstringstream s;
				s << L"-------------------------------------- " << asList->Count() << L" ------------------------------------------\n";
				s << L" PID                 GUID                 VOLUME/MUTE    NAME" << std::endl;
				s << L"------------------------------------------------------------------------------------\n";
				auto& list = asList->GetSessionList();
				for (auto& item : list) {
					s << item->to_string().str() << L"\n";
				}
				s << L"------------------------------------------------------------------------------------\n";
				return s.str();
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return common_error_code::Get().get_error(common_error_id::err_SESSION_LIST);
		}

		/* Private */

		void  AudioSessionMixer::set_config_cb_(std::shared_ptr<JSON::MMTConfig>& mmt) {
			asList->SetControlUnits(mmt);
		}
		void AudioSessionMixer::cb_out_call_(MIDI::MidiUnit& m, DWORD d) {
			if (!isinit_ || !asList) return;

			try {
				ASLIST_t items = asList->GetByMidiId(m.getMixerId());
				if (items.empty()) return;
				for (auto& a : items) {
					if (a) {
						if (a->Last >= d) continue;
						a->Last = d;
						setValueSelector_(a, m.type, m.value.value, m.value.lvalue);
					}
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}

		void AudioSessionMixer::internal_cb_(ASUNIT_t& u, ASITEM_t& item) {
			if (!isinit_ || !asList) return;

			try {
				setValueSelector_(item, u->GetType(), u->GetVolume(), u->GetMute());
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}
		uint32_t AudioSessionMixer::internal_pid_by_name_(std::wstring& name) {
			try {
				ASLIST_t items = asList->GetByAppName(name);
				if (!items.empty() && (items[0] != nullptr)) return items[0]->Item.GetPid();
			} catch (...) {}
			return 0U;
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
								(void) AudioSessionItemBuilder(ptrsc);
								ptrsc.Release();
							} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
						}
					} while (0);
				} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); cnt = -1; }
				if (ptrse) ptrse.Release();
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__);  cnt = -1; }
			return cnt;
		}
		ASLIST_t& AudioSessionMixer::GetSessionList() {
			return asList->GetSessionList();
		}

		void AudioSessionMixer::AudioSessionItemBuilder(IAudioSessionControl* ptrsc) {

			CComPtr<IAudioSessionControl2> ptrSc2;
			CComPtr<ISimpleAudioVolume> ptrSav;
			LPWSTR desc = nullptr, icon = nullptr;

			try {
				AudioSessionState state;
				if ((ptrsc == nullptr) || (ptrsc->GetState(&state) != S_OK))
					return;

				GUID guid;
				if (ptrsc->GetGroupingParam(&guid) != S_OK) return;

				switch (state) {
					case AudioSessionState::AudioSessionStateExpired: {
						asList->RemoveByGuid(guid);
						return;
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
					if (pid == cpid_) break;
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
					ASITEM_t item = std::shared_ptr<AudioSessionItem>(
						new AudioSessionItem(ti, pid, guid, name, path, std::move(ptr), std::move(ptrae), (LPWSTR)desc, (LPWSTR)icon, vol, static_cast<bool>(mute)),
						audiosessionitem_deleter()
					);
					#if defined (_DEBUG)
					::OutputDebugStringW(item->to_string().c_str());
					#endif
					to_log::Get() << log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_SESSIONCREATED),
						item->Item.App.get<std::wstring>()
					);
					asList->Add(std::move(item));

				} while (0);
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }

			ptrSav.Release();
			ptrSc2.Release();
			if (desc) ::CoTaskMemFree(desc);
			if (icon) ::CoTaskMemFree(icon);
		}
	}
}