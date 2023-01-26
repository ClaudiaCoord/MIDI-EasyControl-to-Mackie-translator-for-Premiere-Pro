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
		using namespace std::string_view_literals;

		constexpr std::wstring_view strStart1Ok = L": Mixer audio sessions started, at the time of start, there are "sv;
		constexpr std::wstring_view strStart2Ok = L" running programs available for regulation."sv;
		constexpr std::wstring_view strStopOk = L": The audio sessions Mixer is gracefully stopped."sv;
		constexpr std::wstring_view strStopError = L": The audio sessions mixer could not be stopped correctly, the details of the error should be in the log file."sv;
		constexpr std::wstring_view strStartError = L": The audio sessions mixer could not be started, error details should be in the log file."sv;

		AudioSessionMixer AudioSessionMixer::audiosessionmixer__;
		const double AudioSessionMixer::GetVolumeDevider = KEY_DEVIDER;

		/* template */

		template <typename T1>
		void					setValueFromAudioAction_(AudioAction t, T1 dev, AudioSessionItem*& item, float& v, bool m) {
			switch (t) {
			case AudioAction::AUDIO_VOLUME:
			case AudioAction::AUDIO_VOLUME_INCREMENT: {

				if (t == AudioAction::AUDIO_VOLUME_INCREMENT)
					(void)item->Volume.IncrementVolume(m);
				else
					item->Volume.SetVolume(v);

				if constexpr (std::is_same_v<T1, ISimpleAudioVolume*>)
					dev->SetMasterVolume(item->Volume.GetVolume(), static_cast<LPCGUID>(&item->Guid));
				else if constexpr (std::is_same_v<T1, IAudioEndpointVolume*>)
					dev->SetMasterVolumeLevelScalar(item->Volume.GetVolume(), static_cast<LPCGUID>(&item->Guid));
				break;
			}
			case AudioAction::AUDIO_MUTE: {
				item->Volume.SetMute(m);
				dev->SetMute(item->Volume.GetMute(), static_cast<LPCGUID>(&item->Guid));
				break;
			}
			case AudioAction::AUDIO_NONE:
			default: break;
			}
		}
		template <typename T1>
		std::tuple<float, bool> getValueFromAudioAction_(AudioAction t, T1 dev, AudioSessionItem*& item) {
			switch (t) {
			case AudioAction::AUDIO_VOLUME: {
				float v = 0.0f;
				if constexpr (std::is_same_v<T1, ISimpleAudioVolume*>)
					if (dev->GetMasterVolume(&v) != S_OK) break;
					else if constexpr (std::is_same_v<T1, IAudioEndpointVolume*>)
						if (dev->GetMasterVolumeLevelScalar(&v) != S_OK) break;

				item->Volume.SetVolume(v);
				return std::tuple<float, bool>(v, false);
			}
			case AudioAction::AUDIO_MUTE: {
				BOOL b;
				if (dev->GetMute(&b) != S_OK) break;
				item->Volume.SetMute(b);
				return std::tuple<float, bool>(0.0f, b);
			}
			case AudioAction::AUDIO_NONE:
			default: break;
			}
			return std::tuple<float, bool>();
		}
		template<typename T1>
		void					setValueFromCallBack_(T1& m, AudioSessionItem* item, AudioAction act) {
			float v = 0.0f;
			CComPtr<ISimpleAudioVolume> ptrav;

			try {
				[[maybe_unused]] OnChangeType typechange;

				switch (m.type) {
					case Common::MIDI::MidiUnitType::KNOB:
					case Common::MIDI::MidiUnitType::FADER:
					case Common::MIDI::MidiUnitType::SLIDER:
					case Common::MIDI::MidiUnitType::KNOBINVERT:
					case Common::MIDI::MidiUnitType::FADERINVERT:
					case Common::MIDI::MidiUnitType::SLIDERINVERT: {
						if ((act != AudioAction::AUDIO_VOLUME) && (act != AudioAction::AUDIO_PANORAMA)) return;
						v = static_cast<float>(m.value.value * KEY_DEVIDER);
						typechange = OnChangeType::OnChangeUpdateVolume;
						break;
					}
					case Common::MIDI::MidiUnitType::BTN:
					case Common::MIDI::MidiUnitType::BTNTOGGLE: {
						if (act != AudioAction::AUDIO_MUTE) return;
						typechange = OnChangeType::OnChangeUpdateMute;
						break;
					}
					default: return;
				}
				switch (item->GetType()) {
					case TypeItems::TypeSession: {
						if (!item->IsValidSession()) break;
						if (item->GetSession()->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&ptrav) != S_OK) break;
						setValueFromAudioAction_(act, ptrav.p, item, v, m.value.lvalue);
						break;
					}
					case TypeItems::TypeMaster: {
						if (!item->IsValidEndpointVolume()) return;
						setValueFromAudioAction_(act, item->GetEndpointVolume(), item, v, m.value.lvalue);
						break;
					}
				}
				#if defined(MIXER_EVENT_EXTENDED)
				AudioSessionMixer::Get().event_send(item->GetSessionItemChange(typechange));
				#endif

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			if (ptrav) ptrav.Release();
		}

		/* class */

		AudioSessionMixer::AudioSessionMixer() : cbid__(0U) {
			cpid__ = GetCurrentProcessId();
			type__ = Common::MIDI::ClassTypes::ClassMixer;
			out1__ = [](Common::MIDI::Mackie::MIDIDATA&, DWORD&) { return false; };
			out2__ = std::bind(static_cast<const bool(AudioSessionMixer::*)(Common::MIDI::MidiUnit&, DWORD&)>(&AudioSessionMixer::InCallBack), this, _1, _2);
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

		void AudioSessionMixer::init(bool iscoinitialize) {
			CComPtr<IMMDevice> pDevice;
			CComPtr<IMMDeviceEnumerator> pDeviceEnumerator;
			iscoinitialize__ = iscoinitialize;

#			pragma warning( push )
#			pragma warning( disable : 4286 )
			try {
				release();

				if (iscoinitialize__) {
					if (::CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE) != S_OK)
						throw new runtime_werror(
							AudioSessionErrors::GetMsg(AudioSessionErrors::ERRMSG_INIT_COINITIALIZE)
						);
				}
				if (pDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)) != S_OK)
					throw new runtime_werror(
						AudioSessionErrors::GetMsg(AudioSessionErrors::ERRMSG_INIT_COCREATE)
					);
				if (pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice) != S_OK)
					throw new runtime_werror(
						AudioSessionErrors::GetMsg(AudioSessionErrors::ERRMSG_INIT_DEFAULT_AE)
					);
				if (pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (VOID**)&ptrASM2) != S_OK)
					throw new runtime_werror(
						AudioSessionErrors::GetMsg(AudioSessionErrors::ERRMSG_INIT_AS_MANAGER2)
					);

				issetaudioleveloldvalue__ = common_config::Get().Registry.GetMixerSetOldLevelValue();
				isfastvalue__ = common_config::Get().Registry.GetMixerFastValue();

				asList.reset(new AudioSessionList(
					cpid__,
					issetaudioleveloldvalue__,
					std::bind(static_cast<void(AudioSessionMixer::*)(Common::MIDI::MixerUnit&, AudioSessionItem*&)>(&AudioSessionMixer::InternalCallBack), this, _1, _2)
				));
				asCbNotify.reset(new AudioSessionEventsNotify(
					[&](IAudioSessionControl*& p) { return AudioSessionItemBuilder(p); }
				));

				isinit__ = true;

				ptrASM2->RegisterSessionNotification(asCbNotify.get());
				int cnt = getSessionList();

				cbid__ = Common::common_config::Get().add(std::bind(
					static_cast<void(AudioSessionMixer::*)(std::shared_ptr<Common::MIDI::MidiDevice>&)>(&AudioSessionMixer::SetControlUnits), this, _1));
				Common::common_config::Get().Local.IsAudioMixerRun(true);
				Common::to_log::Get() << (log_string() << __FUNCTIONW__ << strStart1Ok << cnt << strStart2Ok);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				Common::to_log::Get() << (log_string() << __FUNCTIONW__ << strStartError);
				isinit__ = false;
			}
#			pragma warning( pop )

			if (pDevice) pDevice.Release();
			if (pDeviceEnumerator) pDeviceEnumerator.Release();
			if (!isinit__) {
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
				if (ptrASM2)   ptrASM2.Release();
				if (iscoinitialize__) ::CoUninitialize();
				if (asCbNotify)	asCbNotify.reset();
				if (cbid__ > 0U) {
					Common::common_config::Get().remove(cbid__);
					cbid__ = 0U;
				}
				onchangevalue__.clear();
				Common::to_log::Get() << (log_string() << __FUNCTIONW__ << strStopOk);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				Common::to_log::Get() << (log_string() << __FUNCTIONW__ << strStopError);
			}
			Common::common_config::Get().Local.IsAudioMixerRun(false);
			isinit__ = false;
		}

		/* Event */

		void AudioSessionMixer::event_add(callOnChange fn, uint32_t id) {
			try {
				if (isinit__)  onchangevalue__.add(fn, id);
				if (!asList || asList->IsEmpty()) return;
				
				auto& list = asList->GetSessionList();
				for (auto& a : list)
					onchangevalue__.send(a->GetSessionItemChange(OnChangeType::OnChangeNew));

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

		void AudioSessionMixer::Start() {
			init();
		}
		void AudioSessionMixer::Stop() {
			release();
		}

		uint32_t AudioSessionMixer::GetCurrentPid() {
			return cpid__;
		}
		uint32_t AudioSessionMixer::InternalCallItemPidByName(std::wstring& name) {
			try {
				AudioSessionItem* item = asList->Get(name);
				if (item != nullptr)
					return item->GetPid();
			} catch (...) {}
			return 0U;
		}
		std::wstring AudioSessionMixer::SessionListToString() {
			try {
				if (!isinit__)
					return AudioSessionErrors::GetMsg(AudioSessionErrors::ERRMSG_NO_INIT);
				if (!asList || asList->IsEmpty())
					return AudioSessionErrors::GetMsg(AudioSessionErrors::EMPTY);

				std::wstringstream s;
				s << L"-------------------------------------- " << asList->Count() << L" ------------------------------------------" << std::endl;
				s << L" PID                 GUID                 VOLUME/MUTE    NAME" << std::endl;
				s << L"------------------------------------------------------------------------------------" << std::endl;
				auto& list = asList->GetSessionList();
				for (AudioSessionItem* item : list) {
					s << item->ToString() << std::endl;
				}
				s << L"------------------------------------------------------------------------------------" << std::endl;
				return s.str();
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return AudioSessionErrors::GetMsg(AudioSessionErrors::ERRMSG_SESSION_LIST);
		}

		void  AudioSessionMixer::SetControlUnits(std::shared_ptr<Common::MIDI::MidiDevice>& md) {
			asList->SetControlUnits(md);
		}
		void  AudioSessionMixer::IncrementVolume(uint32_t key, bool b) {
			setAudioSession(AudioAction::AUDIO_VOLUME_INCREMENT, key, 0.0f, b);
		}
		void  AudioSessionMixer::SetVolume(uint32_t key, uint8_t& v) {
			uint8_t vu = (v > 127) ? 127 : v;
			float vf = static_cast<float>(vu * KEY_DEVIDER);
			setAudioSession(AudioAction::AUDIO_VOLUME, key, vf, false);
		}
		void  AudioSessionMixer::SetVolume(uint32_t key, float& v) {
			setAudioSession(AudioAction::AUDIO_VOLUME, key, v, false);
		}
		void  AudioSessionMixer::SetMute(uint32_t key, bool m) {
			setAudioSession(AudioAction::AUDIO_MUTE, key, 0.0f, m);
		}
		bool  AudioSessionMixer::GetMute(uint32_t key) {
			std::tuple<float, bool> t = getAudioSession(AudioAction::AUDIO_MUTE, key);
			return std::get<1>(t);
		}
		float AudioSessionMixer::GetVolume(uint32_t key) {
			std::tuple<float, bool> t = getAudioSession(AudioAction::AUDIO_VOLUME, key);
			return std::get<0>(t);
		}

		/* Private */

		const bool AudioSessionMixer::InCallBack(Common::MIDI::MidiUnit& m, DWORD& d) {
			if (!isinit__ || !asList) return false;

			try {
				AudioSessionItem* item = asList->GetByMidiId(m.GetMixerId());
				if ((item == nullptr) || (item->Last >= m.value.time)) return false;
				item->Last = m.value.time;

				setValueFromCallBack_(m, item, item->MidiId.Get(m.GetMixerId()));
				return true;

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}

		void AudioSessionMixer::InternalCallBack(Common::MIDI::MixerUnit& m, AudioSessionItem*& item) {
			if (!isinit__ || !asList) return;

			try {
				setValueFromCallBack_(m, item, item->MidiId.Get(m.id));
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

		void AudioSessionMixer::setAudioSession(AudioAction t, uint32_t key, float v, bool m) {
			if (!isinit__ || !asList || asList->IsEmpty() || t == AudioAction::AUDIO_NONE) return;

			CComPtr<ISimpleAudioVolume> ptrav;

			try {
				AudioSessionItem* item = asList->GetByMidiId(key);
				if (item == nullptr) return;

				switch (t) {
					case AudioAction::AUDIO_VOLUME:
						if (item->Volume.GetVolume() == v) return;
						break;
					case AudioAction::AUDIO_MUTE:
						if (item->Volume.GetMute() == m) return;
						break;
					case AudioAction::AUDIO_VOLUME_INCREMENT:
						break;
					default: return;
				}
				switch (item->Typeitem) {
					case TypeItems::TypeSession: {
						if (!item->IsValidSession()) return;
						if (item->GetSession()->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&ptrav) != S_OK) break;
						setValueFromAudioAction_<ISimpleAudioVolume*>(t, ptrav.p, item, v, m);
						break;
					};
					case TypeItems::TypeMaster: {
						if (!item->IsValidEndpointVolume()) return;
						setValueFromAudioAction_<IAudioEndpointVolume*>(t, item->GetEndpointVolume(), item, v, m);
						return;
					}
					default: return;
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			if (ptrav) ptrav.Release();
		}
		std::tuple<float, bool> AudioSessionMixer::getAudioSession(AudioAction t, uint32_t key) {
			std::tuple<float, bool> ret;
			do {
				if (!isinit__ || !asList || asList->IsEmpty() || t == AudioAction::AUDIO_NONE) break;

				CComPtr<ISimpleAudioVolume> ptrav;

				try {
					AudioSessionItem* item = asList->GetByMidiId(key);
					if (item == nullptr) break;

					if (isfastvalue__)
						return std::tuple<float, bool>(item->Volume.GetVolume(), item->Volume.GetMute());

					switch (item->Typeitem) {
					case TypeItems::TypeSession: {
						if (!item->IsValidSession()) return ret;
						if (item->GetSession()->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&ptrav) != S_OK) break;
						ret = getValueFromAudioAction_<ISimpleAudioVolume*>(t, ptrav.p, item);
						break;
					};
#				if !defined(MASTER_VOLUME_BY_SESSION)
					case TypeItems::TypeMaster: {
						if (!item->IsValidEndpointVolume()) return ret;
						return getValueFromAudioAction_<IAudioEndpointVolume*>(t, item->GetEndpointVolume(), item);
					}
#				endif
					default: return ret;
					}
				} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }

				if (ptrav) ptrav.Release();

			} while (0);
			return ret;
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
						asList->Remove(guid);
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
						std::tuple<bool, std::wstring, std::wstring> t = Common::Utils::pid_to_string(pid);
						if (!std::get<0>(t)) break;

						path = std::get<2>(t);
						std::wstring nm = std::get<1>(t);
						if (nm.empty()) {
							if (!CHECK_LPWSTRING(desc)) break;
							nm = Common::Utils::to_string(desc);
							LPWSTR w = desc;
							::CoTaskMemFree(w);
							desc = nullptr;
						}
						if (nm.empty()) {
							asList->Remove(guid);
							break;
						}
						name = Common::Utils::trim(nm);
					}
					else if (ti == TypeItems::TypeMaster) {
						name = std::wstring(L"master");
						ptrae = getMainEndpointVolume();
					} else {
						asList->Remove(guid);
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