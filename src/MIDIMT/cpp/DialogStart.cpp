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

		static const int ids_on_start[] = { IDC_GO_START, IDC_OPEN_CONFIG, IDC_CONFIG_SAVE, IDC_AUTORUN_CONFIG, IDC_PROXY_COMBO };
		static const int ids_on_mixer[] = { IDC_MIXER_FAST_VALUE, IDC_MIXER_OLD_VALUE };

		DialogStart::DialogStart() {
			mcb__.LogNotify = [=]() {
				if (hwnd__)
					::PostMessageW(hwnd__.get(), WM_COMMAND, MAKEWPARAM(IDC_IEVENT_LOG, 0), 0);
			};
			mcb__.MonitorNotify = [=]() {
				if (hwnd__)
					::PostMessageW(hwnd__.get(), WM_COMMAND, MAKEWPARAM(IDC_IEVENT_MONITOR, 0), 0);
			};
			mcb__.IsLogOneLine = mcb__.IsMonitorOneLine = false;
		}
		DialogStart::~DialogStart() {
			Dispose();
		}

		const bool DialogStart::IsRunOnce() {
			return !hwnd__;
		}
		void DialogStart::SetFocus() {
			if (hwnd__) (void) ::SetFocus(hwnd__.get());
		}

		NO_HWND void DialogStart::Dispose() {
			Stop();
			mcb__.Clear();
			hwnd__.reset();
		}
		void DialogStart::EndDialog() {
			try {
				to_log::Get().unregistred(mcb__.GetCbLog());
				hwnd__.reset();
				mcb__.Clear();
			} catch (...) {}
		}
		void DialogStart::InitDialog(HWND hwnd) {
			hwnd__.reset(hwnd);
			try {
				mcb__.Clear();
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
				
				auto& devc = cnf.GetConfig();
				std::wstring ws = MIDI::MidiBridge::Get().GetVirtualDriverVersion();
				CheckDlgButton(hwnd, IDC_INFO_VMDRV_CHECK, CHECKBTN(!ws.empty()));
				SetDlgItemTextW(hwnd, IDC_INFO_VMDRV_VER, ws.c_str());
				if (ws.empty())
					mcb__.AddToLog(lang.GetString(IDS_DLG_MSG4));
				else
					mcb__.AddToLog(log_string() << lang.GetString(IDS_DLG_MSG10) << ws);

				CheckDlgButton(hwnd, IDC_AUTOBOOT_SYS, CHECKBTN(cnf.Registry.GetAutoRun()));
				CheckDlgButton(hwnd, IDC_AUTORUN_SYS, CHECKBTN(cnf.Registry.GetSysAutoStart()));

				CheckRadioButton(hwnd, IDC_START_RADIO1, IDC_START_RADIO2, CHECKRADIO(ismb, IDC_START_RADIO1, IDC_START_RADIO2));
				EnableWindow(GetDlgItem(hwnd, IDC_GO_START), (isconfig && !ismb));
				EnableWindow(GetDlgItem(hwnd, IDC_CONFIG_SAVE), false);


				CheckDlgButton(hwnd, IDC_MIXER_ENABLE, CHECKBTN(cnf.Registry.GetMixerEnable()));
				CheckDlgButton(hwnd, IDC_MMKEY_ENABLE, CHECKBTN(cnf.Registry.GetMMKeyEnable()));
				CheckDlgButton(hwnd, IDC_MIXER_RIGHT_CLICK, CHECKBTN(cnf.Registry.GetMixerRightClick()));
				CheckDlgButton(hwnd, IDC_MIXER_FAST_VALUE, CHECKBTN(cnf.Registry.GetMixerFastValue()));
				CheckDlgButton(hwnd, IDC_MIXER_OLD_VALUE, CHECKBTN(cnf.Registry.GetMixerSetOldLevelValue()));

				CheckDlgButton(hwnd, IDC_WRITE_FILELOG, CHECKBTN(islog));

				SetConfigurationInfo(hwnd, devc, cnf);
				BuildLangComboBox();
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogStart::ConfigSave() {
			if (!hwnd__) return;
			HWND hwnd = hwnd__.get();

			try {

				EnableWindow(GetDlgItem(hwnd, IDC_CONFIG_SAVE), false);
				common_config& cnf = common_config::Get();
				auto& devc = cnf.GetConfig();

				devc->autostart = (IsDlgButtonChecked(hwnd, IDC_AUTORUN_CONFIG) == BST_CHECKED);
				devc->manualport = (IsDlgButtonChecked(hwnd, IDC_MANUALPORT_CONFIG) == BST_CHECKED);
				devc->jogscenefilter = (IsDlgButtonChecked(hwnd, IDC_JOGFILTER_CONFIG) == BST_CHECKED);

				DWORD pos = GetSliderValue(hwnd, IDC_SLIDER_INT);
				if (pos <= 1000) devc->btninterval = pos;
				pos = GetSliderValue(hwnd, IDC_SLIDER_LONGINT);
				if (pos <= 1500) devc->btnlonginterval = pos;

				cnf.Registry.SetSysAutoStart(IsDlgButtonChecked(hwnd, IDC_AUTOBOOT_SYS) == BST_CHECKED);
				cnf.Registry.SetAutoRun(IsDlgButtonChecked(hwnd, IDC_AUTORUN_SYS) == BST_CHECKED);
				cnf.Registry.SetLogWrite(IsDlgButtonChecked(hwnd, IDC_WRITE_FILELOG) == BST_CHECKED);

				cnf.Registry.SetMixerEnable(IsDlgButtonChecked(hwnd, IDC_MIXER_ENABLE) == BST_CHECKED);
				cnf.Registry.SetMMKeyEnable(IsDlgButtonChecked(hwnd, IDC_MMKEY_ENABLE) == BST_CHECKED);

				cnf.Registry.SetMixerFastValue(IsDlgButtonChecked(hwnd, IDC_MIXER_FAST_VALUE) == BST_CHECKED);
				cnf.Registry.SetMixerSetOldLevelValue(IsDlgButtonChecked(hwnd, IDC_MIXER_OLD_VALUE) == BST_CHECKED);

				if (!cnf.Save())
					EnableWindow(GetDlgItem(hwnd, IDC_CONFIG_SAVE), true);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::StartFromUi() {
			if (!hwnd__) return;
			HWND hwnd = hwnd__.get();

			try {
				for (int i : ids_on_start)
					Gui::SetControlEnable(hwnd, i, false);

				if (Start()) {
					CheckRadioButton(hwnd, IDC_START_RADIO1, IDC_START_RADIO2,
						CHECKRADIO(common_config::Get().Local.IsMidiBridgeRun(), IDC_START_RADIO1, IDC_START_RADIO2));
					return;
				}

				for (int i : ids_on_start)
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

				auto& log = to_log::Get();
				auto& cnf = common_config::Get();
				auto& lang = LangInterface::Get();
				if (cnf.Local.IsMidiBridgeRun()) return false;

				if (!cnf.IsConfig() || cnf.IsConfigEmpty()) {
					if (!cnf.Load()) {
						log << lang.GetString(IDS_DLG_MSG9);
						return false;
					}
				}
				if (!cnf.GetConfig()->autostart)
					log << lang.GetString(IDS_DLG_MSG11);

				try {
					mcb__.Clear();
					MIDI::MidiBridge& mb = MIDI::MidiBridge::Get();

					std::wstring ws = mb.GetVirtualDriverVersion();
					if (ws.empty()) {
						log << lang.GetString(IDS_DLG_MSG4);
						return false;
					}

					if (mb.Start(cnfname)) {

						bool mix_enabled = cnf.Registry.GetMixerEnable(),
							 key_enabled = cnf.Registry.GetMMKeyEnable();
						
						if (mix_enabled) {
							MIXER::AudioSessionMixer& mix = MIXER::AudioSessionMixer::Get();
							mix.init(false);
							mb.SetCallbackOut(mix);
						}
						if (key_enabled) {
							MMKey::MMKBridge& mkey = Common::MMKey::MMKBridge::Get();
							mb.SetCallbackOut(mkey);
							if (mix_enabled) {
								MIXER::AudioSessionMixer& mix = MIXER::AudioSessionMixer::Get();
								mkey.SetPidCb(mix.GetCbItemPidByName());
							}
						}
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

		void DialogStart::EventLog() {
			try {
				if (hwnd__) mcb__.LogLoop(hwnd__.get(), IDC_LOG);
			} catch (...) {}
		}
		void DialogStart::EventMonitor() {
			try {
				if (hwnd__) mcb__.MonitorLoop(hwnd__.get(), IDC_LOG);
			} catch (...) {}
		}

		/// 

		void DialogStart::ChangeOnJogfilter() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				auto& devc = common_config::Get().GetConfig();
				devc->jogscenefilter = (IsDlgButtonChecked(hwnd, IDC_JOGFILTER_CONFIG) == BST_CHECKED);
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

				to_log::Get().filelog((IsDlgButtonChecked(hwnd, IDC_WRITE_FILELOG) == BST_CHECKED));
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
					(IsDlgButtonChecked(hwnd, IDC_AUTOBOOT_SYS) == BST_CHECKED)
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
					(IsDlgButtonChecked(hwnd, IDC_AUTORUN_SYS) == BST_CHECKED)
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
				devc->autostart = (IsDlgButtonChecked(hwnd, IDC_AUTORUN_CONFIG) == BST_CHECKED);
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
					(IsDlgButtonChecked(hwnd, IDC_MIXER_FAST_VALUE) == BST_CHECKED)
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
					(IsDlgButtonChecked(hwnd, IDC_MIXER_OLD_VALUE) == BST_CHECKED)
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
				devc->manualport = (IsDlgButtonChecked(hwnd, IDC_MANUALPORT_CONFIG) == BST_CHECKED);
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
				HWND hwcb = GetDlgItem(hwnd, IDC_PROXY_COMBO);
				if (hwcb != nullptr) {
					int32_t idx = static_cast<int32_t>(::SendMessageW(hwcb, CB_GETCURSEL, 0, 0));
					if (idx == CB_ERR) return;
					devc->proxy = idx;
					CheckDlgButton(hwnd, IDC_INFO_ISPROXY, CHECKBTN(idx > 0));
					if (idx == 0)
						mcb__.AddToLog(lang.GetString(IDS_DLG_MSG6));
					else
						mcb__.AddToLog(log_string() << lang.GetString(IDS_DLG_MSG5) << std::to_wstring(idx));
				} else {
					devc->proxy = 0U;
					CheckDlgButton(hwnd, IDC_INFO_ISPROXY, CHECKBTN(false));
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

				HWND hwcb = GetDlgItem(hwnd, IDC_LANG_COMBO);
				if (hwcb != nullptr) {
					int32_t idx = static_cast<int32_t>(::SendMessageW(hwcb, CB_GETCURSEL, 0, 0));
					if (idx == CB_ERR) return;

					wchar_t buf[MAX_PATH]{};
					if (::SendMessageW(hwcb, CB_GETLBTEXT, idx, (LPARAM)&buf) == CB_ERR) return;
					auto& lang = LangInterface::Get();
					lang.SelectLanguage(buf);
					mcb__.AddToLog(log_string() << lang.GetString(IDS_DLG_MSG7) << Utils::to_string(buf));
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnDevice() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				HWND hwcb = GetDlgItem(hwnd, IDC_DEVICE_COMBO);
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
					(IsDlgButtonChecked(hwnd, IDC_MMKEY_ENABLE) == BST_CHECKED)
				);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnMixerEnable() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				bool b = (IsDlgButtonChecked(hwnd, IDC_MIXER_ENABLE) == BST_CHECKED);
				common_config::Get().Registry.SetMixerEnable(b);
				for (int i : ids_on_mixer)
					Gui::SetControlEnable(hwnd, i, b);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::ChangeOnMixerRightClick() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				common_config::Get().Registry.SetMixerRightClick(
					(IsDlgButtonChecked(hwnd, IDC_MIXER_RIGHT_CLICK) == BST_CHECKED)
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
						if (cnf.Load(Utils::to_string(pws))) {
							auto& cnf = common_config::Get();
							auto& devc = cnf.GetConfig();
							SetConfigurationInfo(hwnd, devc, cnf);
							Gui::SaveConfigEnabled(hwnd);
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

		void DialogStart::BuildLangComboBox() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				HWND hwcb = GetDlgItem(hwnd, IDC_LANG_COMBO);
				if (hwcb == nullptr) return;

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

				HWND hwcb = GetDlgItem(hwnd, IDC_DEVICE_COMBO);
				if (hwcb == nullptr) return;

				auto& v = MIDI::MidiBridge::Get().GetInputDeviceList();
				if (v.empty()) {
					ComboBox_AddString(hwcb, s.c_str());
				} else {
					for (auto& name : v)
						ComboBox_AddString(hwcb, name.c_str());
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

				HWND hwcb = GetDlgItem(hwnd, IDC_PROXY_COMBO);
				if (hwcb == nullptr) return;

				for (uint32_t i = 0, z = (n > 5) ? (n + 1) : 5; i < z; i++)
					ComboBox_AddString(hwcb, std::to_wstring(i).c_str());
				ComboBox_SelectString(hwcb, 0, std::to_wstring(n).c_str());
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogStart::SetConfigurationInfo(HWND hwnd, std::shared_ptr<Common::MIDI::MidiDevice>& devc, Common::common_config& cnf) {

			std::wstring ws = cnf.Registry.GetConfPath();
			if (!ws.empty()) {
				std::filesystem::path p = std::filesystem::path(ws);
				ws = p.stem().wstring();
				SetDlgItemTextW(hwnd, IDC_FILE_CONFIG_NAME, ws.c_str());
			}

			CheckDlgButton(hwnd, IDC_INFO_ISCONFIG, CHECKBTN(cnf.IsConfig()));
			CheckDlgButton(hwnd, IDC_INFO_ISPROXY, CHECKBTN(cnf.IsProxy()));
			CheckDlgButton(hwnd, IDC_AUTORUN_CONFIG, CHECKBTN(devc->autostart));
			CheckDlgButton(hwnd, IDC_MANUALPORT_CONFIG, CHECKBTN(devc->manualport));
			CheckDlgButton(hwnd, IDC_JOGFILTER_CONFIG, CHECKBTN(devc->jogscenefilter));
			CheckDlgButton(hwnd, IDC_INFO_ISUNITS, CHECKBTN(!devc->units.empty()));
			SetDlgItemTextW(hwnd, IDC_INFO_CTRLCOUNT, std::to_wstring(devc->units.size()).c_str());

			BuildProxyComboBox(devc->proxy);
			uint32_t ival = (devc->btninterval > 0) ? devc->btninterval : 50,
					 lval = (devc->btnlonginterval > 0) ? devc->btnlonginterval : 500;
			SetSliderValues(hwnd, IDC_SLIDER_INT, IDC_SLIDER_VAL1, 10, 1000, ival);
			SetSliderValues(hwnd, IDC_SLIDER_LONGINT, IDC_SLIDER_VAL2, 100, 1500, lval);

			if (!devc->name.empty()) {
				log_string ls;
				ls << devc->name << MIDI::MidiHelper::GetSuffixMackieOut();
				::SetDlgItemTextW(hwnd, IDC_MIDI_MACKIE_OUT, ls.str().c_str());
				if (cnf.IsProxy()) {
					ls.reset();
					if (devc->proxy == 1) {
						ls << devc->name << MIDI::MidiHelper::GetSuffixProxyOut() << L"1";
					} else {
						ls << devc->name << MIDI::MidiHelper::GetSuffixProxyOut() << L"(";
						for (uint32_t i = 0, z = devc->proxy - 1; i < devc->proxy; i++) {
							ls << std::to_string(i + 1U);
							if (i < z) ls << L",";
						}
						ls << L")";
					}
					::SetDlgItemTextW(hwnd, IDC_MIDI_PROXY_OUT, ls.str().c_str());
				} else {
					::SetDlgItemTextW(hwnd, IDC_MIDI_PROXY_OUT, L"-");
				}
				BuildDeviceComboBox(devc->name);
			}
			if (cnf.IsConfig() && !cnf.IsConfigEmpty())
				EnableWindow(GetDlgItem(hwnd, IDC_GO_START), true);
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