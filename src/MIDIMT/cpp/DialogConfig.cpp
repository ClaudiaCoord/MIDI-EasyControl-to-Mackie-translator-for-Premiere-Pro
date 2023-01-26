/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2023, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"
#include <shobjidl.h>
#include <CommCtrl.h>

namespace Common {
	namespace MIDIMT {

		using namespace std::placeholders;
		using namespace std::string_view_literals;

		static const uint16_t ids[] = { 0, IDC_SETUP_NUMBER, IDC_SETUP_SCENE, IDC_SETUP_TYPE, IDC_SETUP_TARGET, IDC_SETUP_TARGET_LONG };
		static const uint16_t idn[] = { 0, IDC_SETUP_NUMBER, IDC_SETUP_SCENEN, IDC_SETUP_TYPEN, IDC_SETUP_TARGETN, IDC_SETUP_TARGET_LONGN };

		constexpr std::wstring_view TargetAll = L"All"sv;
		constexpr std::wstring_view TargetInt = L"Internal"sv;
		constexpr std::wstring_view TargetPP = L"Premiere Pro"sv;
		constexpr std::wstring_view TargetMMK = L"Multimedia Key"sv;

		static void HelperAdd(HWND hwnd, uint32_t idx, std::wstring s) {
			int pos = (int)::SendMessageW(hwnd, LB_ADDSTRING, 0, (LPARAM)s.c_str());
			(void) ::SendMessageW(hwnd, LB_SETITEMDATA, pos, (LPARAM)idx);
		}

		DialogConfig::DialogConfig() {
			try {
				mcb__.LogNotify = [=]() {
					if (hwnd__)
						PostMessageW(hwnd__.get(), WM_COMMAND, MAKEWPARAM(IDC_IEVENT_LOG, 0), 0);
				};
				mcb__.MonitorNotify = [=]() {
					if (hwnd__)
						PostMessageW(hwnd__.get(), WM_COMMAND, MAKEWPARAM(IDC_IEVENT_MONITOR, 0), 0);
				};
				mcb__.MonitorData = [=](MIDI::Mackie::MIDIDATA& m, DWORD& t) -> bool {
					if (!hwnd__) return false;
					try {
						HWND hwnd = hwnd__.get();
						ListMixerContainer* cont = new ListMixerContainer(m, t);
						Common::MIDI::MidiBridge::Get().RemoveCallbackOut(std::ref(mcb__));

						if (lv__->ListViewInsertItem(cont))
							Gui::SaveConfigEnabled(hwnd);
						if (common_config::Get().Local.IsMidiBridgeRun())
							Gui::SetControlEnable(hwnd, IDC_SETUP_CODE);
					}
					catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
					return true;
				};
				mcb__.IsLogOneLine = mcb__.IsMonitorOneLine = true;

				uint16_t status_icos[]{ IDI_ICON_SETUP1, IDI_ICON_SETUP2, IDI_ICON_SETUP3 };
				img_status__.Init(status_icos, std::size(status_icos), true);

				uint16_t btn_mute_icos[]{ IDI_ICON_SETUP1, IDI_ICON_SOUNDON, IDI_ICON_SOUNDOFF };
				btn_mute__.Init(btn_mute_icos, std::size(btn_mute_icos));

				uint16_t btn_folder_icos[]{ IDI_ICON_FOLDER };
				btn_folder__.Init(btn_folder_icos, std::size(btn_folder_icos));

				uint16_t btn_app_icos[]{ IDI_ICON_RUNAPP };
				btn_runapp__.Init(btn_app_icos, std::size(btn_app_icos));

				uint16_t btn_delete_icos[]{ IDI_ICON_REMOVE };
				btn_remove__.Init(btn_delete_icos, std::size(btn_delete_icos));

				uint16_t btn_info_icos[]{ IDI_ICON_INFO };
				btn_info__.Init(btn_info_icos, std::size(btn_info_icos));
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		DialogConfig::~DialogConfig() {
			Dispose();
		}

		const bool DialogConfig::IsRunOnce() {
			return !hwnd__;
		}
		void DialogConfig::SetFocus() {
			if (hwnd__) (void) ::SetFocus(hwnd__.get());
		}

		void DialogConfig::Clear() {
			try {
				ConfigDevice = ConfigStatus::None;
				if (hwnd__)
					(void) ::PostMessageW(hwnd__.get(), WM_COMMAND, MAKEWPARAM(IDCANCEL, 0), 0);
				hwnd__.reset();
				if (lv__) {
					lv__->ListViewEnd();
					lv__.reset();
				}
				btn_mute__.Reset();
				btn_info__.Reset();
				btn_folder__.Reset();
				btn_runapp__.Reset();
				btn_remove__.Reset();
				img_status__.Reset();
			} catch (...) {}
		}
		void DialogConfig::Dispose() {
			Clear();
			try {
				btn_mute__.Release();
				btn_info__.Release();
				btn_runapp__.Release();
				btn_remove__.Release();
				btn_folder__.Release();
				img_status__.Release();
			} catch (...) {}
		}
		void DialogConfig::InitListView() {
			try {
				if (lv__)
					lv__.reset();
				lv__ = std::make_unique<ListEdit>();
				lv__->ListViewErrorCb(mcb__.GetCbLog());
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::EndDialog() {
			try {
				to_log::Get().unregistred(mcb__.GetCbLog());
				Clear();
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::InitDialog(HWND hwnd) {
			try {
				mcb__.Clear();
				to_log::Get().registred(mcb__.GetCbLog());
				common_config& cnf = common_config::Get();

				ConfigDevice = !cnf.IsConfigEmpty() ? (cnf.Local.IsMidiBridgeRun() ? ConfigStatus::LoadDevice : ConfigStatus::LoadFile) :
														(!cnf.IsConfig() ? ConfigStatus::None : ConfigStatus::LoadFile);
				if (ConfigDevice == ConfigStatus::None) {
					ConfigDevice = cnf.Load() ? ((cnf.Local.IsMidiBridgeRun()) ? ConfigStatus::LoadDevice : ConfigStatus::LoadFile) : ConfigStatus::None;
				}

				HWND hwcl;
				if ((hwcl = ::GetDlgItem(hwnd, IDC_SETUP_LIST)) == nullptr)
					return;

				hwnd__.reset(hwnd);
				InitListView();
				lv__->ListViewInit(hwcl);

				if (ConfigDevice != ConfigStatus::None) {
					lv__->ListViewLoad(cnf.GetConfig());
					SetDlgItemText(hwnd, IDC_SETUP_COUNT, std::to_wstring(lv__->ListViewCount()).c_str());
				}

				if (cnf.Local.IsMidiBridgeRun())
					Gui::SetControlEnable(hwnd, IDC_SETUP_CODE);

				try {

					btn_mute__.Init(hwnd,   IDC_BTN_MUTE);
					btn_info__.Init(hwnd,	IDC_BTN_INFO);
					btn_runapp__.Init(hwnd, IDC_BTN_RUNAPP);
					btn_remove__.Init(hwnd, IDC_BTN_REMOVE);
					btn_folder__.Init(hwnd, IDC_BTN_FOLDER);
					img_status__.Init(hwnd, IDC_SETUP_ICON);

					if (ConfigDevice != 0) img_status__.SetStatus(ConfigDevice);
					mcb__.AddToLog(LangInterface::Get().GetString(
						(ConfigDevice == 0) ? IDS_SETUP_MSG3 : ((ConfigDevice == 1) ? IDS_SETUP_MSG2 : IDS_SETUP_MSG1)
					));

				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}

				CheckRadioButton(hwnd, IDC_SETUP_RADIO1, IDC_SETUP_RADIO4, IDC_SETUP_RADIO1);
				HelpCategorySelected(IDC_SETUP_RADIO1);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogConfig::HelpCategoryAddTarget(HWND hwnd, uint32_t idx, const std::wstring_view target) {
			log_string ls;
			ls << idx << Gui::GetBlank(idx) << L" - [" << target << L"] " << MIDI::MackieHelper::GetTarget(idx).data();
			HelperAdd(hwnd, idx, ls.str());
		}
		void DialogConfig::HelpCategoryAddMMKey(HWND hwnd, uint32_t idx) {
			log_string ls;
			ls << idx << Gui::GetBlank(idx) << L" - [" << TargetMMK << L"] " << MIDI::MackieHelper::GetTranslateMMKey(idx).data();
			HelperAdd(hwnd, idx, ls.str());
		}
		void DialogConfig::HelpCategorySelected(uint32_t k) {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				HWND hwcl;
				if ((hwcl = ::GetDlgItem(hwnd, IDC_SETUP_HELPLIST)) == nullptr)
					return;

				(void) ListBox_ResetContent(hwcl);

				for (uint32_t i = 0, n = 0; i <= static_cast<uint32_t>(MIDI::Mackie::Target::VOLUMEMIX); i++) {
					if ((k == IDC_SETUP_RADIO2) || (k == IDC_SETUP_RADIO1)) {
						if (((i >= 32) && (i <= 40)) || (i == 2) || (i == 3)) {
							HelpCategoryAddTarget(hwcl, i, TargetAll);
							continue;
						}
						else if (((i >= 23) && (i <= 31)) || ((i >= 41) && (i <= 49)) || (i >= 50) && (i <= 65)) {
							HelpCategoryAddTarget(hwcl, i, TargetPP);
							continue;
						}
					}
					if ((k == IDC_SETUP_RADIO3) || (k == IDC_SETUP_RADIO1)) {
						if (((i >= 5) && (i <= 22)) || (i == 0) || (i == 1)) {
							HelpCategoryAddTarget(hwcl, i, TargetAll);
							continue;
						}
					}
					if ((k == IDC_SETUP_RADIO4) || (k == IDC_SETUP_RADIO1)) {
						if ((i >= 66) && (i <= 76)) {
							HelpCategoryAddTarget(hwcl, i, TargetPP);
							HelpCategoryAddMMKey(hwcl, i);
							continue;
						}
					}
					if ((k == IDC_SETUP_RADIO1) && ((i >= 252) && (i <= 254))) {
						if ((i >= 50) && (i <= 65))
							HelpCategoryAddTarget(hwcl, i, TargetPP);
						else if ((i >= 252) && (i <= 254))
							HelpCategoryAddTarget(hwcl, i, TargetInt);
						else
							HelpCategoryAddTarget(hwcl, i, TargetAll);
					}
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogConfig::ListViewSort(LPNMHDR lpmh) {
			if ((lpmh != nullptr) && (lv__))
				(void) lv__->ListViewSort(lpmh);
		}
		void DialogConfig::ListViewMenu(LPNMHDR lpmh) {
			if ((lpmh != nullptr) && (lv__)) {
				(void)lv__->ListViewMenu(lpmh);
				if (hwnd__) Gui::SaveConfigEnabled(hwnd__.get());
			}
		}
		void DialogConfig::ListViewMenu(uint32_t id) {
			if (!hwnd__ || !lv__) return;
			HWND hwnd = hwnd__.get();

			try {
				bool b = lv__->ListViewMenu(id);
				if (b) {
					switch (id) {
						case IDM_LV_NEW:
						case IDM_LV_PASTE:
						case IDM_LV_DELETE: {
							Gui::SaveConfigEnabled(hwnd);
							SetDlgItemText(hwnd, IDC_SETUP_COUNT, std::to_wstring(lv__->ListViewCount()).c_str());
							break;
						}
						case IDM_LV_SET_MQTT:
						case IDM_LV_SET_MMKEY:
						case IDM_LV_SET_MIXER: {
							Gui::SaveConfigEnabled(hwnd);
							break;
						}
						case IDM_LV_READ_MIDI_CODE: {
							ButtonMonitor();
							Gui::SaveConfigEnabled(hwnd);
							break;
						}
						default: break;
					}
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ListViewEdit(LPNMHDR lpmh) {
			if (!hwnd__ || (lpmh == nullptr) || !lv__) return;
			HWND hwnd = hwnd__.get();
			
			try {
				bool b = lv__->ListViewSetItem(lpmh);
				if (b)
					Gui::SaveConfigEnabled(hwnd);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ListViewClick(LPNMHDR lpmh) {
			if (!hwnd__ || (lpmh == nullptr) || !lv__) return;
			HWND hwnd = hwnd__.get();

			try {
				last__.reset(lv__->ListViewGetSelectedRow(lpmh));
				if (!last__ ||
					(last__->unit.key >= 255) ||
					(last__->unit.scene >= 255U) ||
					(last__->unit.type == MIDI::MidiUnitType::UNITNONE))
					return;

				LangInterface& lang = LangInterface::Get();

				for (size_t i = 1; i < lv__->ListViewColumns(); i++) {
					std::wstring ws{}, wn{};
					switch (i) {
						case 1: {
							wn = std::to_wstring(static_cast<uint32_t>(last__->unit.key));
							break;
						}
						case 2: {
							wn = std::to_wstring(static_cast<uint32_t>(last__->unit.scene));
							ws = MIDI::MidiHelper::GetScene(last__->unit.scene);
							break;
						}
						case 3: {
							wn = std::to_wstring(static_cast<uint32_t>(last__->unit.type));
							ws = MIDI::MidiHelper::GetType(last__->unit.type);
							break;
						}
						case 4: {
							wn = std::to_wstring(static_cast<uint32_t>(last__->unit.target));
							ws = lang.GetString(MIDI::MackieHelper::GetTargetID(last__->unit.target));
							break;
						}
						case 5: {
							wn = std::to_wstring(static_cast<uint32_t>(last__->unit.longtarget));
							switch (last__->unit.target) {
								case MIDI::Mackie::Target::MEDIAKEY: {
									ws = MIDI::MackieHelper::GetTranslateMMKey(last__->unit.longtarget);
									break;
								}
								case MIDI::Mackie::Target::MQTTKEY: {
									ws = lang.GetString(MIDI::MackieHelper::GetSmartHomeTargetID(last__->unit.longtarget));
									break;
								}
								default:{
									ws = LangInterface::Get().GetString(MIDI::MackieHelper::GetTargetID(last__->unit.longtarget));
									break;
								}
							}
							break;
						}
						default: continue;
					}
					if (i != 1)
						::SetDlgItemTextW(hwnd, ids[i], ws.c_str());
					::SetDlgItemTextW(hwnd, idn[i], wn.c_str());
				}

				HWND hwcl;
				if ((hwcl = ::GetDlgItem(hwnd, IDC_SETUP_APPLIST)) == nullptr) return;
				(void)ListBox_ResetContent(hwcl);

				if (last__->unit.target == MIDI::Mackie::Target::VOLUMEMIX) {
					(void) ::EnableWindow(hwcl, true);

					if (!last__->unit.appvolume.empty()) {
						for (auto& s : last__->unit.appvolume)
							(void) ::SendMessageW(hwcl, LB_ADDSTRING, 0, (LPARAM)s.c_str());
					}

					if (common_config::Get().Local.IsAudioMixerRun())
						btn_runapp__.SetEnable();
					btn_folder__.SetEnable();
					btn_remove__.SetEnable(false);

					switch (last__->unit.type) {
						case MIDI::MidiUnitType::BTN:
						case MIDI::MidiUnitType::BTNTOGGLE: {
							SetVolumeSlider(hwnd, -1);
							SetMuteImage(last__->unit.value.lvalue);
							break;
						}
						case MIDI::MidiUnitType::FADER:
						case MIDI::MidiUnitType::FADERINVERT:
						case MIDI::MidiUnitType::SLIDER:
						case MIDI::MidiUnitType::SLIDERINVERT:
						case MIDI::MidiUnitType::KNOB:
						case MIDI::MidiUnitType::KNOBINVERT: {
							SetVolumeSlider(hwnd, last__->unit.value.value);
							SetMuteImage(-1);
							break;
						}
						default: {
							SetVolumeSlider(hwnd, -1);
							SetMuteImage(-1);
							break;
						}
					}
				} else {
					(void) ::EnableWindow(hwcl, false);


					SetVolumeSlider(hwnd, -1);
					SetMuteImage(-1);

					btn_runapp__.SetEnable(false);
					btn_folder__.SetEnable(false);
					btn_remove__.SetEnable(false);
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ListViewDbClick(LPNMHDR lpmh) {
			if ((lpmh != nullptr) && (lv__))
				(void) lv__->ListViewEdit(lpmh);
		}

		void DialogConfig::ButtonSave() {
			if (!hwnd__ || !lv__) return;
			HWND hwnd = hwnd__.get();

			try {
				common_config& cnf = common_config::Get();
				bool b = lv__->ListViewGetList(cnf.Get().GetConfig());
				if (!b) return;

				Gui::SetControlEnable(hwnd, IDC_DIALOG_SAVE, false);
				::SetDlgItemTextW(hwnd, IDC_SETUP_COUNT, std::to_wstring(lv__->ListViewCount()).c_str());
				(void) cnf.Save();
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ButtonMonitor() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				if (!Gui::CheckControlEnable(hwnd, IDC_SETUP_CODE)) return;
				if (!Common::common_config::Get().Local.IsMidiBridgeRun()) return;
				Gui::SetControlEnable(hwnd, IDC_SETUP_CODE, false);
				Common::MIDI::MidiBridge::Get().SetCallbackOut(std::ref(mcb__));
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogConfig::EventLog() {
			try {
				if (hwnd__) mcb__.LogLoop(hwnd__.get(), IDC_LOG);
			} catch (...) {}
		}
		void DialogConfig::EventMonitor() {
			try {
				if (hwnd__) mcb__.MonitorLoop(hwnd__.get(), IDC_LOG);
			} catch (...) {}
		}

		void DialogConfig::ChangeOnAppBox() {
			if (!hwnd__) return;
			try {
				btn_remove__.SetEnable();
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ChangeOnBtnAppFolder() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				PWSTR pws = nullptr;
				IShellItem* item = nullptr;
				IFileOpenDialog* ptr = nullptr;

				try {
					std::wstring filter = LangInterface::Get().GetString(IDS_CONFIG_EXE_FILTER);
					COMDLG_FILTERSPEC extfilter[] = {
						{ filter.c_str(), L"*.exe"}
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

						std::wstring app = Utils::device_out_name(Utils::to_string(pws), L"");
						if (!app.empty()) {
							HWND hwcl;
							if ((hwcl = ::GetDlgItem(hwnd, IDC_SETUP_APPLIST)) != nullptr)
								(void) ::SendMessageW(hwcl, LB_ADDSTRING, 0, (LPARAM)app.c_str());

							if (last__)
								last__->unit.appvolume.push_back(app);

							Gui::SaveConfigEnabled(hwnd);
							btn_remove__.SetEnable();
						}
					} while (0);
				}
				catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				if (item != nullptr) item->Release();
				if (ptr != nullptr) ptr->Release();
				if (pws != nullptr) ::CoTaskMemFree(pws);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ChangeOnBtnAppRunning() {
			if (!hwnd__ || !common_config::Get().Local.IsAudioMixerRun()) return;
			HWND hwnd = hwnd__.get();

			try {
				HWND hwcl;
				if ((hwcl = ::GetDlgItem(hwnd, IDC_SETUP_APPLIST)) == nullptr) return;

				MIXER::AudioSessionMixer& mix = MIXER::AudioSessionMixer::Get();
				std::vector<MIXER::AudioSessionItem*>& list = mix.GetSessionList();
				for (auto& a : list) {
					std::wstring app = a->GetName();
					if (!app.empty()) {
						(void) ::SendMessageW(hwcl, LB_ADDSTRING, 0, (LPARAM)app.c_str());
						if (last__)
							last__->unit.appvolume.push_back(app);
					}
				}
				Gui::SaveConfigEnabled(hwnd);
				btn_remove__.SetEnable();
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ChangeOnBtnAppDelete() {
			if (!hwnd__) return;
			HWND hwnd = hwnd__.get();

			try {
				int idx;
				HWND hwcl;
				if ((hwcl = ::GetDlgItem(hwnd, IDC_SETUP_APPLIST)) == nullptr) return;
				if ((idx = ListBox_GetCurSel(hwcl)) == LB_ERR) return;
				if (last__) {
					wchar_t buf[MAX_PATH]{};
					ListBox_GetText(hwcl, idx, buf);
					std::wstring app = Utils::to_string(buf);
					if (!app.empty())
						last__->unit.appvolume.erase(
							std::remove_if(last__->unit.appvolume.begin(), last__->unit.appvolume.end(),
								[app](std::wstring& s) {
									return app._Equal(s);
								})
						);
				}

				(void) ListBox_DeleteString(hwcl, idx);
				if (ListBox_GetCount(hwcl) <= 0) {
					Gui::SaveConfigEnabled(hwnd);
					btn_remove__.SetEnable();
				} else {
					btn_remove__.SetEnable(false);
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ChangeOnBtnMute() {
			if (!hwnd__ || !last__) return;
			HWND hwnd = hwnd__.get();

			try {
				last__->unit.value.lvalue = !last__->unit.value.lvalue;
				last__->unit.value.value = last__->unit.value.lvalue ? 127 : 0;
				SetMuteImage(last__->unit.value.lvalue);
				Gui::SaveConfigEnabled(hwnd);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ChangeOnSlider() {
			if (!hwnd__ || !last__) return;
			HWND hwnd = hwnd__.get();

			try {
				HWND hwcl = GetDlgItem(hwnd, IDC_SLIDER_VOLUME);
				if (hwcl == nullptr) return;
				last__->unit.value.value = static_cast<uint8_t>(SendMessageW(hwcl, TBM_GETPOS, 0, 0));
				Gui::SaveConfigEnabled(hwnd);
				if (common_config::Get().Local.IsAudioMixerRun())
					MIXER::AudioSessionMixer::Get().SetVolume(last__->unit.key, last__->unit.value.value);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogConfig::SetVolumeSlider(HWND hwnd, int32_t pos) {
			try {
				HWND hwcl = GetDlgItem(hwnd, IDC_SLIDER_VOLUME);
				if (hwcl == nullptr) return;
				(void) ::EnableWindow(hwcl, (pos >= 0));
				pos = (pos < 0) ? 0 : pos;
				(void) ::SendMessageW(hwcl, TBM_SETRANGE, static_cast<WPARAM>(FALSE), static_cast<LPARAM>(MAKELONG(0, 127)));
				(void) ::SendMessageW(hwcl, TBM_SETPOS, static_cast<WPARAM>(TRUE), static_cast<LPARAM>(pos));
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::SetMuteImage(int32_t val) {
			try {
				btn_mute__.SetEnable(val >= 0);
				btn_mute__.SetStatus((val == -1) ? 0 : ((val > 0) ? 1 : 2));
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
	}
}