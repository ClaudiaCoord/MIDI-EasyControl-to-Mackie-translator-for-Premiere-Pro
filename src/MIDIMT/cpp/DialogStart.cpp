/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"
#include <shobjidl.h>


namespace Common {
	namespace MIDIMT {

		using namespace std::placeholders;

		#define CHECKBTN(A) (A ? BST_CHECKED : BST_UNCHECKED)
		#define CHECKRADIO(A,B,C) (A ? B : C)
		#define NO_HWND

		static const int ids_on_start[] = { IDC_GO_START, IDC_OPEN_CONFIG, IDC_DIALOG_SAVE, IDC_AUTORUN_CONFIG, IDC_PROXY_COMBO, IDC_MANUALPORT_CONFIG };
		static const int ids_on_mixer[] = { IDC_MIXER_FAST_VALUE, IDC_MIXER_OLD_VALUE, IDC_MIXER_DUPLICATE, IDC_MIXER_ENABLE };
		static const int ids_on_mmkey[] = { IDC_MMKEY_ENABLE };
		static const int ids_on_mqtt[]  = { IDC_MQTT_IPADDR, IDC_MQTT_PORT, IDC_MQTT_LOGIN, IDC_MQTT_PASS, IDC_MQTT_PSK, IDC_MQTT_ISSSL, IDC_MQTT_ISSELFSIGN, IDC_MQTT_CAOPEN, IDC_MQTT_PREFIX, IDC_MQTT_LOGLEVEL };

		DialogStart::DialogStart() {
			mcb__.Init(IDC_IEVENT_LOG, IDC_IEVENT_MONITOR);
			mcb__.HwndCb = [=]() { return hwnd__.get(); };
		}
		DialogStart::~DialogStart() {
			dispose_();
		}
		NO_HWND void DialogStart::dispose_() {
			Stop();
			clear_();
			mcb__.Clear();
		}
		NO_HWND void DialogStart::clear_() {
			try {
				if (!hwnd__) return;
				(void) ::PostMessageW(hwnd__.get(), WM_COMMAND, MAKEWPARAM(IDCANCEL, 0), 0);
				hwnd__.reset();
				to_log::Get().unregistred(mcb__.GetCbLog());
			} catch (...) {}
		}

		const bool DialogStart::IsRunOnce() {
			return !hwnd__;
		}
		void DialogStart::SetFocus() {
			if (hwnd__) (void) ::SetFocus(hwnd__.get());
		}

		void DialogStart::InitDialog(HWND hwnd) {
			hwnd__.reset(hwnd);
			try {
				to_log::Get().registred(mcb__.GetCbLog());

				auto& lang = LangInterface::Get();
				auto& cnf = common_config::Get();
				bool ismb = cnf.Local.IsMidiBridgeRun(),
					 ismix = cnf.Local.IsAudioMixerRun(),
					 ismkeys = cnf.Local.IsMMKeyesRun(),
					 isconfig = cnf.IsNewConfig(),
					 islog = to_log::Get().filelog();

				if (isconfig) {
					isconfig = cnf.Load();
					if (!isconfig)
						mcb__.AddToLog(lang.GetString(IDS_DLG_MSG9));
				}
				
				auto& config = cnf.GetConfig();
				std::wstring ws = MIDI::MidiBridge::Get().GetVirtualDriverVersion();
				::CheckDlgButton(hwnd, IDC_INFO_VMDRV_CHECK, CHECKBTN(!ws.empty()));
				::SetDlgItemTextW(hwnd, IDC_INFO_VMDRV_VER, ws.c_str());
				if (ws.empty())
					mcb__.AddToLog(lang.GetString(IDS_DLG_MSG4));
				else
					mcb__.AddToLog(log_string() << lang.GetString(IDS_DLG_MSG10) << ws);

				::CheckDlgButton(hwnd, IDC_AUTOBOOT_SYS, CHECKBTN(cnf.Registry.GetAutoRun()));
				::CheckDlgButton(hwnd, IDC_AUTORUN_SYS, CHECKBTN(cnf.Registry.GetSysAutoStart()));

				::CheckRadioButton(hwnd, IDC_START_RADIO1, IDC_START_RADIO2, CHECKRADIO(ismb, IDC_START_RADIO1, IDC_START_RADIO2));
				::EnableWindow(GetDlgItem(hwnd, IDC_GO_START), (isconfig && !ismb));
				::EnableWindow(GetDlgItem(hwnd, IDC_DIALOG_SAVE), false);


				::CheckDlgButton(hwnd, IDC_MIXER_ENABLE,		CHECKBTN(cnf.Registry.GetMixerEnable()));
				::CheckDlgButton(hwnd, IDC_MMKEY_ENABLE,		CHECKBTN(cnf.Registry.GetMMKeyEnable()));
				::CheckDlgButton(hwnd, IDC_MIXER_RIGHT_CLICK,	CHECKBTN(cnf.Registry.GetMixerRightClick()));
				::CheckDlgButton(hwnd, IDC_MIXER_FAST_VALUE,	CHECKBTN(cnf.Registry.GetMixerFastValue()));
				::CheckDlgButton(hwnd, IDC_MIXER_OLD_VALUE,	CHECKBTN(cnf.Registry.GetMixerSetOldLevelValue()));
				::CheckDlgButton(hwnd, IDC_MIXER_DUPLICATE,	CHECKBTN(cnf.Registry.GetMixerDupAppRemove()));

				::CheckDlgButton(hwnd, IDC_WRITE_FILELOG,		CHECKBTN(islog));

				SetConfigurationInfo(hwnd, config, cnf);
				BuildLangComboBox();
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::EndDialog() {
			if (!hwnd__) return;
			clear_();
		}

		void DialogStart::ConfigSave() {
			if (!hwnd__) return;
			HWND hwnd = hwnd__.get();

			try {

				::EnableWindow(::GetDlgItem(hwnd, IDC_DIALOG_SAVE), false);
				common_config& cnf = common_config::Get();
				auto& config = cnf.GetConfig();

				config->autostart = Gui::GetControlChecked(hwnd, IDC_AUTORUN_CONFIG);
				config->manualport = Gui::GetControlChecked(hwnd, IDC_MANUALPORT_CONFIG);
				config->jogscenefilter = Gui::GetControlChecked(hwnd, IDC_JOGFILTER_CONFIG);

				DWORD pos = GetSliderValue(hwnd, IDC_SLIDER_INT);
				if (pos <= 1000) config->btninterval = pos;
				pos = GetSliderValue(hwnd, IDC_SLIDER_LONGINT);
				if (pos <= 1500) config->btnlonginterval = pos;

				cnf.Registry.SetSysAutoStart(Gui::GetControlChecked(hwnd, IDC_AUTOBOOT_SYS));
				cnf.Registry.SetAutoRun(Gui::GetControlChecked(hwnd, IDC_AUTORUN_SYS));
				cnf.Registry.SetLogWrite(Gui::GetControlChecked(hwnd, IDC_WRITE_FILELOG));

				cnf.Registry.SetMixerEnable(Gui::GetControlChecked(hwnd, IDC_MIXER_ENABLE));
				cnf.Registry.SetMMKeyEnable(Gui::GetControlChecked(hwnd, IDC_MMKEY_ENABLE));
				cnf.Registry.SetSmartHomeEnable(Gui::GetControlChecked(hwnd, IDC_MQTT_ENABLE));

				cnf.Registry.SetMixerFastValue(Gui::GetControlChecked(hwnd, IDC_MIXER_FAST_VALUE));
				cnf.Registry.SetMixerSetOldLevelValue(Gui::GetControlChecked(hwnd, IDC_MIXER_OLD_VALUE));

				if (!cnf.Save())
					::EnableWindow(::GetDlgItem(hwnd, IDC_DIALOG_SAVE), true);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::StartFromUi() {
			if (!hwnd__) return;
			HWND hwnd = hwnd__.get();

			try {
				const size_t sz_on_start = std::size(ids_on_start),
							 sz_on_mixer = std::size(ids_on_mixer),
							 sz_on_mmkey = std::size(ids_on_mmkey),
							 sz_on_mqtt  = std::size(ids_on_mqtt);
				std::vector<int> v(sz_on_start + sz_on_mixer + sz_on_mmkey + sz_on_mqtt + 1);
				std::copy(&ids_on_start[0], &ids_on_start[sz_on_start], back_inserter(v));
				std::copy(&ids_on_mixer[0], &ids_on_mixer[sz_on_mixer], back_inserter(v));
				std::copy(&ids_on_mmkey[0], &ids_on_mmkey[sz_on_mmkey], back_inserter(v));
				std::copy(&ids_on_mqtt[0],  &ids_on_mqtt[sz_on_mqtt],   back_inserter(v));
				v.push_back(IDC_MQTT_ENABLE);

				for (int i : v)
					Gui::SetControlEnable(hwnd, i, false);

				if (Start()) {
					CheckRadioButton(hwnd, IDC_START_RADIO1, IDC_START_RADIO2,
						CHECKRADIO(common_config::Get().Local.IsMidiBridgeRun(), IDC_START_RADIO1, IDC_START_RADIO2));
					return;
				}

				for (int i : v)
					Gui::SetControlEnable(hwnd, i, true);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		NO_HWND void DialogStart::AutoStart() {
			try {
				common_config& cnf = common_config::Get();
				do {
					if (cnf.IsStart()) break;
					if (!cnf.IsConfig() || cnf.IsConfigEmpty()) {
						if (!cnf.Load()) break;
					}
					if (!cnf.IsConfig() || cnf.IsConfigEmpty()) break;
					if (!cnf.Registry.GetAutoRun()) break;
					if (!cnf.GetConfig()->autostart) break;
					Start();
				} while (0);
				
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		NO_HWND bool DialogStart::Start(std::wstring cnfname) {
			try {

				auto& log  = to_log::Get();
				auto& cnf  = common_config::Get();
				auto& lang = LangInterface::Get();
				if (cnf.Local.IsMidiBridgeRun()) return false;

				if (!cnf.IsConfig() || cnf.IsConfigEmpty()) {
					if (!cnf.Load()) {
						log << lang.GetString(IDS_DLG_MSG9);
						return false;
					}
				}

				try {
					MIDI::MidiBridge& mb = MIDI::MidiBridge::Get();

					std::wstring ws = mb.GetVirtualDriverVersion();
					if (ws.empty()) {
						log << lang.GetString(IDS_DLG_MSG4);
						return false;
					}

					if (!cnf.GetConfig()->autostart)
						log << lang.GetString(IDS_DLG_MSG11);

					if (mb.Start(cnfname)) {

						bool mix_enabled  = cnf.Registry.GetMixerEnable(),
							 key_enabled  = cnf.Registry.GetMMKeyEnable(),
							 mqtt_enabled = cnf.Registry.GetSmartHomeEnable();
						
						if (mix_enabled) {
							MIXER::AudioSessionMixer& mix = MIXER::AudioSessionMixer::Get();
							mix.Start();
							mb.SetCallbackOut(mix);
						}
						if (key_enabled) {
							MMKey::MMKBridge& mkey = MMKey::MMKBridge::Get();
							mb.SetCallbackOut(mkey);
							if (mix_enabled) {
								MIXER::AudioSessionMixer& mix = MIXER::AudioSessionMixer::Get();
								mkey.SetPidCb(mix.GetCbItemPidByName());
							}
						}
						if (mqtt_enabled) {
							MQTT::SmartHome& mqtt = MQTT::SmartHome::Get();
							if (mqtt.Start()) {
								mb.SetCallbackOut(mqtt);

								worker_background::Get().to_async(std::async(std::launch::async, [=]() {
									try {
										auto& c = common_config::Get().GetConfig();
										std::vector<MIDI::Mackie::Target> v;
										for (auto& u : c->units)
											if (u.target == MIDI::Mackie::Target::MQTTKEY)
												v.push_back(u.longtarget);
										if (!v.empty())
											MQTT::SmartHome::Get().SetTitle(v);
									} catch (...) {}
								}));
							}
						}
						std::wstring c = cnfname.empty() ? cnf.Registry.GetConfPath() : cnfname;
						if (!c.empty())
							cnf.RecentConfig.Add(c);
					}
					log.flush();
					return cnf.IsStart();
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		NO_HWND void DialogStart::Stop() {
			try {

				auto& cnf = common_config::Get();
				auto& mb = MIDI::MidiBridge::Get();

				if (cnf.Local.IsMidiBridgeRun())
					mb.Stop();
				if (cnf.Local.IsAudioMixerRun()) {
					Common::MIXER::AudioSessionMixer& mix = MIXER::AudioSessionMixer::Get();
					mb.RemoveCallbackOut(mix);
					mix.Stop();
					if (cnf.Registry.GetMixerSetOldLevelValue())
						(void) cnf.Save();
				}
				if (cnf.Local.IsMMKeyesRun()) {
					Common::MMKey::MMKBridge& mkey = MMKey::MMKBridge::Get();
					mb.RemoveCallbackOut(mkey);
					mkey.Stop();
				}
				if (cnf.Local.IsSmartHomeRun()) {
					MQTT::SmartHome& mqtt = MQTT::SmartHome::Get();
					mb.RemoveCallbackOut(mqtt);
					mqtt.Stop();
				}
				to_log::Get().flush();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		NO_HWND bool DialogStart::IsStart() {
			common_config& cnf = common_config::Get();
			return cnf.IsStart();
		}

		///

		void DialogStart::EventLog(CbEventData* data) {
			try {
				if (data == nullptr) return;
				CbEventDataDeleter d = data->GetDeleter();
				if (!hwnd__) return;
				CbEvent::ToLog(GetDlgItem(hwnd__.get(), IDC_LOG), d.GetData(), false);
			} catch (...) {}
		}
		void DialogStart::EventMonitor(CbEventData* data) {
			try {
				if (data == nullptr) return;
				CbEventDataDeleter d = data->GetDeleter();
				if (!hwnd__) return;
				CbEvent::ToMonitor(GetDlgItem(hwnd__.get(), IDC_LOG), d.GetData(), false);
			} catch (...) {}
		}

		/// 

		void DialogStart::ChangeOnJogfilter() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto& devc = common_config::Get().GetConfig();
				devc->jogscenefilter = Gui::GetControlChecked(hwnd, IDC_JOGFILTER_CONFIG);
				Gui::SaveConfigEnabled(hwnd);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnLog() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				to_log::Get().filelog(Gui::GetControlChecked(hwnd, IDC_WRITE_FILELOG));
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnSysAutoStart() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				common_config::Get().Registry.SetSysAutoStart(
					Gui::GetControlChecked(hwnd, IDC_AUTOBOOT_SYS)
				);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnSysAutoRun() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				common_config::Get().Registry.SetAutoRun(
					Gui::GetControlChecked(hwnd, IDC_AUTORUN_SYS)
				);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnAutoRunConfig() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto& devc = common_config::Get().GetConfig();
				devc->autostart = Gui::GetControlChecked(hwnd, IDC_AUTORUN_CONFIG);
				Gui::SaveConfigEnabled(hwnd);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnMixerfastvalue() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				common_config::Get().Registry.SetMixerFastValue(
					Gui::GetControlChecked(hwnd, IDC_MIXER_FAST_VALUE)
				);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnMixeroldvalue() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				common_config::Get().Registry.SetMixerSetOldLevelValue(
					Gui::GetControlChecked(hwnd, IDC_MIXER_OLD_VALUE)
				);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnMixerDupAppRemove() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				common_config::Get().Registry.SetMixerDupAppRemove(
					Gui::GetControlChecked(hwnd, IDC_MIXER_DUPLICATE)
				);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnManualPort() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto& devc = common_config::Get().GetConfig();
				devc->manualport = Gui::GetControlChecked(hwnd, IDC_MANUALPORT_CONFIG);
				Gui::SaveConfigEnabled(hwnd);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnSliders() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto& devc = common_config::Get().GetConfig();
				DWORD pos1 = GetSliderValue(hwnd, IDC_SLIDER_INT);
				if (pos1 <= 1000) {
					devc->btninterval = pos1;
					SetSliderInfo(hwnd, IDC_SLIDER_VAL1, pos1);
				} else {
					pos1 = devc->btninterval;
					SetSliderValues(hwnd, IDC_SLIDER_INT, IDC_SLIDER_VAL1, 10, 1000, pos1);
				}

				DWORD pos2 = GetSliderValue(hwnd, IDC_SLIDER_LONGINT);
				if (pos2 <= 1500) {
					bool b = (pos2 > pos1);
					pos2 = b ? pos2 : (pos1 + 150);
					devc->btnlonginterval = pos2;
					if (!b) SetSliderValues(hwnd, IDC_SLIDER_LONGINT, IDC_SLIDER_VAL2, 100, 1500, pos2);
					else    SetSliderInfo(hwnd, IDC_SLIDER_VAL2, pos2);
				}
				Gui::SaveConfigEnabled(hwnd);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnProxy() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto& lang = LangInterface::Get();
				auto& devc = common_config::Get().GetConfig();
				HWND hwcb = ::GetDlgItem(hwnd, IDC_PROXY_COMBO);
				if (hwcb != nullptr) {
					int32_t idx = static_cast<int32_t>(::SendMessageW(hwcb, CB_GETCURSEL, 0, 0));
					if (idx == CB_ERR) return;
					devc->proxy = idx;
					::CheckDlgButton(hwnd, IDC_INFO_ISPROXY, CHECKBTN(idx > 0));
					if (idx == 0)
						mcb__.AddToLog(lang.GetString(IDS_DLG_MSG6));
					else
						mcb__.AddToLog(log_string() << lang.GetString(IDS_DLG_MSG5) << std::to_wstring(idx));
				} else {
					devc->proxy = 0U;
					::CheckDlgButton(hwnd, IDC_INFO_ISPROXY, CHECKBTN(false));
					mcb__.AddToLog(lang.GetString(IDS_DLG_MSG6));
				}
				Gui::SaveConfigEnabled(hwnd);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnLang() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();
				HWND hwcb = ::GetDlgItem(hwnd, IDC_LANG_COMBO);
				if (hwcb == nullptr)  return;

				int32_t idx = static_cast<int32_t>(::SendMessageW(hwcb, CB_GETCURSEL, 0, 0));
				if (idx == CB_ERR) return;

				wchar_t buf[MAX_PATH]{};
				if (::SendMessageW(hwcb, CB_GETLBTEXT, idx, (LPARAM)&buf) == CB_ERR) return;
				auto& lang = LangInterface::Get();
				lang.SelectLanguage(buf);
				mcb__.AddToLog(log_string() << lang.GetString(IDS_DLG_MSG7) << Utils::to_string(buf));
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnDevice() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				HWND hwcb = ::GetDlgItem(hwnd, IDC_DEVICE_COMBO);
				if (hwcb != nullptr) {
					int32_t idx = static_cast<int32_t>(::SendMessageW(hwcb, CB_GETCURSEL, 0, 0));
					if (idx == CB_ERR) return;

					wchar_t buf[MAX_PATH]{};
					if (::SendMessageW(hwcb, CB_GETLBTEXT, idx, (LPARAM)&buf) == CB_ERR) return;
					auto& devc = common_config::Get().GetConfig()->name = Utils::to_string(buf);
					mcb__.AddToLog(log_string() << LangInterface::Get().GetString(IDS_DLG_MSG8) << Utils::to_string(buf));
					Gui::SaveConfigEnabled(hwnd);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnMmkeyEnable() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				common_config::Get().Registry.SetMMKeyEnable(
					Gui::GetControlChecked(hwnd, IDC_MMKEY_ENABLE)
				);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnMixerEnable() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				bool b = Gui::GetControlChecked(hwnd, IDC_MIXER_ENABLE);
				common_config::Get().Registry.SetMixerEnable(b);
				for (int i : ids_on_mixer)
					Gui::SetControlEnable(hwnd, i, b);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnSmartHouseEnable() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				bool b = Gui::GetControlChecked(hwnd, IDC_MQTT_ENABLE);
				common_config::Get().Registry.SetSmartHomeEnable(b);
				for (int i : ids_on_mqtt)
					Gui::SetControlEnable(hwnd, i, b);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnMixerRightClick() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				common_config::Get().Registry.SetMixerRightClick(
					Gui::GetControlChecked(hwnd, IDC_MIXER_RIGHT_CLICK)
				);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnConfigFileOpen() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				PWSTR pws = nullptr;
				IShellItem* item = nullptr;
				IFileOpenDialog* ptr = nullptr;

				try {
					std::wstring filter = LangInterface::Get().GetString(IDS_START_EXT_FILTER);
					COMDLG_FILTERSPEC extfilter[] = {
						{ filter.c_str(), L"*.cnf"}
					};
					do {
						HRESULT h = CoCreateInstance(
							CLSID_FileOpenDialog,
							NULL, CLSCTX_ALL,
							IID_IFileOpenDialog,
							reinterpret_cast<void**>(&ptr));

						if (h != S_OK) break;

						#pragma warning( push )
						#pragma warning( disable : 4267 )
						h = ptr->SetFileTypes(static_cast<UINT>(std::size(extfilter)), extfilter);
						#pragma warning( pop )

						if (h != S_OK) break;
						h = ptr->Show(hwnd);
						if (h != S_OK) break;
						h = ptr->GetResult(&item);
						if (h != S_OK) break;
						h = item->GetDisplayName(SIGDN_FILESYSPATH, &pws);
						if (h != S_OK) break;

						common_config& cnf = common_config::Get();
						std::wstring s = Utils::to_string(pws);
						if (cnf.Load(s)) {
							auto& cnf = common_config::Get();
							auto& config = cnf.GetConfig();
							SetConfigurationInfo(hwnd, config, cnf);
							Gui::SaveConfigEnabled(hwnd);
							cnf.RecentConfig.Add(s);
						}
					} while (0);
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				if (item != nullptr) item->Release();
				if (ptr  != nullptr) ptr->Release();
				if (pws  != nullptr) CoTaskMemFree(pws);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		///

		void DialogStart::ChangeOnSmartHouseLogLevel() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();
				HWND hwcb = GetDlgItem(hwnd, IDC_MQTT_LOGLEVEL);
				if (hwcb == nullptr) return;

				int32_t val, idx = static_cast<int32_t>(::SendMessageW(hwcb, CB_GETCURSEL, 0, 0));
				if (idx == CB_ERR) return;

				switch (idx) {
					case 0: val = 0; break;
					case 1: val = (1 << 0); break;
					case 2: val = (1 << 1); break;
					case 3: val = (1 << 2); break;
					case 4: val = (1 << 3); break;
					case 5: val = (1 << 4); break;
					default: return;
				}
				auto& conf = common_config::Get().GetConfig();
				conf->mqttconf.loglevel = val;

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnSmartHouseSsl() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto& conf = common_config::Get().GetConfig();
				conf->mqttconf.isssl = Gui::GetControlChecked(hwnd, IDC_MQTT_ISSSL);
				Gui::SaveConfigEnabled(hwnd);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnSmartHouseSelfSign() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto& conf = common_config::Get().GetConfig();
				conf->mqttconf.isselfsigned = Gui::GetControlChecked(hwnd, IDC_MQTT_ISSELFSIGN);
				Gui::SaveConfigEnabled(hwnd);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnSmartHouseIpAddress() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto& conf = common_config::Get().GetConfig();
				conf->mqttconf.host = Gui::GetControlText(hwnd, IDC_MQTT_IPADDR);
				Gui::SaveConfigEnabled(hwnd);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnSmartHousePort() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto txt = Gui::GetControlText(hwnd, IDC_MQTT_CA);
				if (txt.empty()) return;

				auto& config = common_config::Get().GetConfig();
				config->mqttconf.port = std::stoul(txt);
				Gui::SaveConfigEnabled(hwnd);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnSmartHouseLogin() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto& conf = common_config::Get().GetConfig();
				conf->mqttconf.login = Gui::GetControlText(hwnd, IDC_MQTT_LOGIN);
				Gui::SaveConfigEnabled(hwnd);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnSmartHousePass() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto& conf = common_config::Get().GetConfig();
				conf->mqttconf.password = Gui::GetControlText(hwnd, IDC_MQTT_PASS);
				Gui::SaveConfigEnabled(hwnd);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}

		}
		void DialogStart::ChangeOnSmartHousePsk() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto& conf = common_config::Get().GetConfig();
				conf->mqttconf.sslpsk = Gui::GetControlText(hwnd, IDC_MQTT_PSK);
				Gui::SaveConfigEnabled(hwnd);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnSmartHousePrefix() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto& conf = common_config::Get().GetConfig();
				conf->mqttconf.mqttprefix = Gui::GetControlText(hwnd, IDC_MQTT_PREFIX);
				Gui::SaveConfigEnabled(hwnd);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}

		}
		void DialogStart::ChangeOnSmartHouseCa() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				PWSTR pws = nullptr;
				IShellItem* item = nullptr;
				IFileOpenDialog* ptr = nullptr;

				try {
					std::wstring filter = LangInterface::Get().GetString(IDS_MQTT_CA_FILTER);
					COMDLG_FILTERSPEC extfilter[] = {
						{ filter.c_str(), L"*.cert;*.pem"}
					};
					do {
						HRESULT h = CoCreateInstance(
							CLSID_FileOpenDialog,
							NULL, CLSCTX_ALL,
							IID_IFileOpenDialog,
							reinterpret_cast<void**>(&ptr));

						if (h != S_OK) break;

						#pragma warning( push )
						#pragma warning( disable : 4267 )
						h = ptr->SetFileTypes(static_cast<UINT>(std::size(extfilter)), extfilter);
						#pragma warning( pop )

						if (h != S_OK) break;
						h = ptr->Show(hwnd);
						if (h != S_OK) break;
						h = ptr->GetResult(&item);
						if (h != S_OK) break;
						h = item->GetDisplayName(SIGDN_FILESYSPATH, &pws);
						if (h != S_OK) break;

						auto& config = common_config::Get().GetConfig();
						config->mqttconf.certcapath = Utils::to_string(pws);

						std::filesystem::path p(config->mqttconf.certcapath);
						::SetDlgItemTextW(hwnd, IDC_MQTT_CA, p.stem().wstring().c_str());
						Gui::SaveConfigEnabled(hwnd);

					} while (0);
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				if (item != nullptr) item->Release();
				if (ptr != nullptr) ptr->Release();
				if (pws != nullptr) CoTaskMemFree(pws);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		///

		void DialogStart::OpenDragAndDrop(std::wstring s) {
			if (s.empty() || !hwnd__) return;
			HWND hwnd = hwnd__.get();

			try {
				LangInterface& lang = LangInterface::Get();
				common_config& cnf = common_config::Get();

				if (!cnf.Load(s)) {
					mcb__.AddToLog(lang.GetString(IDS_DLG_MSG9));
					return;
				}
				cnf.Registry.SetConfPath(s);
				auto& config = cnf.GetConfig();
				SetConfigurationInfo(hwnd, config, cnf);
				Gui::SaveConfigEnabled(hwnd);

				mcb__.AddToLog(log_string::format(L"{0}: {1}",
					lang.GetString(IDS_SETUP_MSG2),
					s.c_str()
				));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::BuildLangComboBox() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				HWND hwcb = ::GetDlgItem(hwnd, IDC_LANG_COMBO);
				if (hwcb == nullptr) return;

				ComboBox_ResetContent(hwcb);

				LangInterface& lang = LangInterface::Get();
				std::forward_list<std::wstring> list = lang.GetLanguages();
				for (auto& name : list)
					ComboBox_AddString(hwcb, name.c_str());
				ComboBox_SelectString(hwcb, 0, lang.SelectedLanguage().c_str());
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::BuildDeviceComboBox(const std::wstring s) {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				HWND hwcb = ::GetDlgItem(hwnd, IDC_DEVICE_COMBO);
				if (hwcb == nullptr) return;

				ComboBox_ResetContent(hwcb);

				auto& v = MIDI::MidiBridge::Get().GetInputDeviceList();
				if (v.empty()) {
					ComboBox_AddString(hwcb, s.c_str());
					::EnableWindow(hwcb, false);
				} else {
					for (auto& name : v)
						ComboBox_AddString(hwcb, name.c_str());
					::EnableWindow(hwcb, true);
				}
				ComboBox_SelectString(hwcb, 0, s.c_str());
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::BuildProxyComboBox(const uint32_t n) {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();
				HWND hwcb = ::GetDlgItem(hwnd, IDC_PROXY_COMBO);
				if (hwcb == nullptr) return;

				ComboBox_ResetContent(hwcb);

				for (uint32_t i = 0, z = (n >= 5) ? (n + 1) : 5; i < z; i++)
					ComboBox_AddString(hwcb, std::to_wstring(i).c_str());
				ComboBox_SelectString(hwcb, 0, std::to_wstring(n).c_str());
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::BuildSmartHomeLogLevelComboBox(const int32_t n) {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();
				HWND hwcb = ::GetDlgItem(hwnd, IDC_MQTT_LOGLEVEL);
				if (hwcb == nullptr) return;

				ComboBox_ResetContent(hwcb);

				LangInterface& lang = LangInterface::Get();
				for (int32_t i = IDM_MQTT_LOG_NONE; i <= IDM_MQTT_LOG_DEBUG; i++)
					ComboBox_AddString(hwcb, lang.GetString(i).c_str());

				int32_t val;
				switch (n) {
					case 0: val = 0; break;
					case (1 << 0): val = 1; break;
					case (1 << 1): val = 2; break;
					case (1 << 2): val = 3; break;
					case (1 << 3): val = 4; break;
					case (1 << 4): val = 5; break;
					default: return;
				}
				ComboBox_SetCurSel(hwcb, val);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogStart::SetConfigurationInfo(HWND hwnd, std::shared_ptr<Common::MIDI::MidiDevice>& config, Common::common_config& cnf) {

			std::wstring ws = cnf.Registry.GetConfPath();
			if (!ws.empty()) {
				std::filesystem::path p = std::filesystem::path(ws);
				ws = p.stem().wstring();
				::SetDlgItemTextW(hwnd, IDC_FILE_CONFIG_NAME, ws.c_str());
			}

			::CheckDlgButton(hwnd, IDC_INFO_ISCONFIG, CHECKBTN(cnf.IsConfig()));
			::CheckDlgButton(hwnd, IDC_INFO_ISPROXY, CHECKBTN(cnf.IsProxy()));
			::CheckDlgButton(hwnd, IDC_AUTORUN_CONFIG, CHECKBTN(config->autostart));
			::CheckDlgButton(hwnd, IDC_MANUALPORT_CONFIG, CHECKBTN(config->manualport));
			::CheckDlgButton(hwnd, IDC_JOGFILTER_CONFIG, CHECKBTN(config->jogscenefilter));
			::CheckDlgButton(hwnd, IDC_INFO_ISUNITS, CHECKBTN(!config->units.empty()));
			::SetDlgItemTextW(hwnd, IDC_INFO_CTRLCOUNT, std::to_wstring(config->units.size()).c_str());

			BuildProxyComboBox(config->proxy);
			uint32_t ival = (config->btninterval > 0) ? config->btninterval : 50,
					 lval = (config->btnlonginterval > 0) ? config->btnlonginterval : 500;
			SetSliderValues(hwnd, IDC_SLIDER_INT, IDC_SLIDER_VAL1, 10, 1000, ival);
			SetSliderValues(hwnd, IDC_SLIDER_LONGINT, IDC_SLIDER_VAL2, 100, 1500, lval);

			BuildSmartHomeLogLevelComboBox(config->mqttconf.loglevel);

			::SetDlgItemTextW(hwnd, IDC_MQTT_IPADDR, config->mqttconf.host.c_str());
			::SetDlgItemTextW(hwnd, IDC_MQTT_PORT, std::to_wstring(config->mqttconf.port).c_str());
			::SetDlgItemTextW(hwnd, IDC_MQTT_LOGIN, config->mqttconf.login.c_str());
			::SetDlgItemTextW(hwnd, IDC_MQTT_PASS, config->mqttconf.password.c_str());
			::SetDlgItemTextW(hwnd, IDC_MQTT_PSK, config->mqttconf.sslpsk.c_str());
			::SetDlgItemTextW(hwnd, IDC_MQTT_PREFIX, config->mqttconf.mqttprefix.c_str());

			std::filesystem::path p(config->mqttconf.certcapath);
			::SetDlgItemTextW(hwnd, IDC_MQTT_CA, p.stem().wstring().c_str());

			::CheckDlgButton(hwnd, IDC_MQTT_ENABLE, CHECKBTN(cnf.Registry.GetSmartHomeEnable()));
			::CheckDlgButton(hwnd, IDC_MQTT_ISSSL, CHECKBTN(config->mqttconf.isssl));
			::CheckDlgButton(hwnd, IDC_MQTT_ISSELFSIGN, CHECKBTN(config->mqttconf.isselfsigned));

			if (!config->name.empty()) {
				log_string ls;
				ls << config->name << MIDI::MidiHelper::GetSuffixMackieOut();
				::SetDlgItemTextW(hwnd, IDC_MIDI_MACKIE_OUT, ls.str().c_str());
				if (cnf.IsProxy()) {
					ls.reset();
					if (config->proxy == 1) {
						ls << config->name << MIDI::MidiHelper::GetSuffixProxyOut() << L"1";
					} else {
						ls << config->name << MIDI::MidiHelper::GetSuffixProxyOut() << L"(";
						for (uint32_t i = 0, z = config->proxy - 1; i < config->proxy; i++)
							ls << (i + 1U) << ((i < z) ? L"," : L"");
					}
					ls << L")";
					::SetDlgItemTextW(hwnd, IDC_MIDI_PROXY_OUT, ls.str().c_str());
				} else {
					::SetDlgItemTextW(hwnd, IDC_MIDI_PROXY_OUT, L"-");
				}
				BuildDeviceComboBox(config->name);
			}
			if (cnf.IsConfig() && !cnf.IsConfigEmpty())
				::EnableWindow(GetDlgItem(hwnd, IDC_GO_START), true);
		}
		void DialogStart::SetSliderInfo(HWND hwnd, uint32_t id, uint32_t pos) {
			::SetDlgItemTextW(hwnd, id, std::wstring(std::to_wstring(pos)).c_str());
		}
		void DialogStart::SetSliderValues(HWND hwnd, uint32_t id, uint32_t idinfo, uint32_t minv, uint32_t maxv, uint32_t pos) {
			try {
				HWND hwns = GetDlgItem(hwnd, id);
				if (hwns == nullptr) return;
				(void) ::SendMessageW(hwns, TBM_SETRANGE, static_cast<WPARAM>(FALSE), static_cast<LPARAM>(MAKELONG(minv, maxv)));
				(void) ::SendMessageW(hwns, TBM_SETPOS, static_cast<WPARAM>(TRUE), static_cast<LPARAM>(pos));
				SetSliderInfo(hwnd, idinfo, pos);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		uint32_t DialogStart::GetSliderValue(HWND hwnd, uint32_t id) {
			try {
				HWND hwcb = GetDlgItem(hwnd, id);
				if (hwcb != nullptr)
					return static_cast<uint32_t>(SendMessage(hwcb, TBM_GETPOS, 0, 0));
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return (UINT_MAX - 1);
		}
	}
}