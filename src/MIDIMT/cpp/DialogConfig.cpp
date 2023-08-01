/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022-2023, MIT

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
		constexpr std::wstring_view TargetPP  = L"Premiere Pro"sv;
		constexpr std::wstring_view TargetMMK = L"Multimedia Key"sv;

		static void HelperAdd(HWND hwnd, uint32_t idx, std::wstring s) {
			int pos = (int)::SendMessageW(hwnd, LB_ADDSTRING, 0, (LPARAM)s.c_str());
			(void) ::SendMessageW(hwnd, LB_SETITEMDATA, pos, (LPARAM)idx);
		}

		DialogConfig::DialogConfig() {
			try {
				mcb__.Init(IDC_IEVENT_LOG, IDC_IEVENT_MONITOR);
				mcb__.HwndCb = [=]() { return hwnd__.get(); };

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
			dispose_();
		}
		void DialogConfig::dispose_() {
			clear_();
			try {
				tb__.reset();
				btn_mute__.Release();
				btn_info__.Release();
				btn_runapp__.Release();
				btn_remove__.Release();
				btn_folder__.Release();
				img_status__.Release();
				mcb__.Clear();
			} catch (...) {}
		}
		void DialogConfig::clear_() {
			try {
				to_log::Get().unregistred(mcb__.GetCbLog());
				ConfigDevice = ConfigStatus::None;
				confpath__ = std::wstring();
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

		const bool DialogConfig::IsRunOnce() {
			return !hwnd__;
		}
		void DialogConfig::SetFocus() {
			if (hwnd__) (void) ::SetFocus(hwnd__.get());
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
		void DialogConfig::InitDialog(HWND hwnd, COPYDATASTRUCT* data) {
			try {
				to_log::Get().registred(mcb__.GetCbLog());
				common_config& cnf = common_config::Get();
				std::shared_ptr<MIDI::MidiDevice> dev;
				confpath__ = std::wstring();

				tb__.reset(new RToolBarDialogConfig(hwnd, L"CONFIG_EDITOR_RIBBON"));

				if (data) {
					if ((data->dwData == CheckRun::Get().GetMsgId()) && (data->cbData > 0)) {
						wchar_t* cnfs = reinterpret_cast<wchar_t*>(data->lpData);
						if (cnfs) confpath__ = std::wstring(cnfs, cnfs + (data->cbData - 1));
						else	  confpath__ = std::wstring();
					}
					if (data->lpData)
						if (data->lpData) delete [] data->lpData;
					delete data;
				}

				bool externalconfig = !confpath__.empty() && std::filesystem::exists(confpath__);

				if (externalconfig) {
					ConfigDevice = ConfigStatus::LoadFile;
					dev = std::make_shared<MIDI::MidiDevice>();
					try {
						MIDI::json_config json{};
						json.Read(dev, confpath__);
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
				} else {
					ConfigDevice = !cnf.IsConfigEmpty() ? (cnf.Local.IsMidiBridgeRun() ? ConfigStatus::LoadDevice : ConfigStatus::LoadFile) :
						(!cnf.IsConfig() ? ConfigStatus::None : ConfigStatus::LoadFile);
					if (ConfigDevice == ConfigStatus::None)
						ConfigDevice = cnf.Load() ? ((cnf.Local.IsMidiBridgeRun()) ? ConfigStatus::LoadDevice : ConfigStatus::LoadFile) : ConfigStatus::None;
					dev = (ConfigDevice == ConfigStatus::None) ? std::make_shared<MIDI::MidiDevice>() : cnf.GetConfig();
				}

				HWND hwcl;
				if ((hwcl = ::GetDlgItem(hwnd, IDC_SETUP_LIST)) == nullptr)
					return;

				hwnd__.reset(hwnd);
				InitListView();
				lv__->ListViewInit(hwcl);

				if (ConfigDevice != ConfigStatus::None) {
					lv__->ListViewLoad(dev);
					::SetDlgItemTextW(hwnd, IDC_SETUP_COUNT, std::to_wstring(lv__->ListViewCount()).c_str());
				}

				tb__->SetEditorNotify(cnf.Local.IsMidiBridgeRun() ? EditorNotify::ReadMidiEnable : EditorNotify::ReadMidiDisable);
				tb__->SetEditorNotify(EditorNotify::ItemEmpty);

				try {

					btn_mute__.Init(hwnd,   IDC_BTN_MUTE);
					btn_info__.Init(hwnd,	IDC_BTN_INFO);
					btn_runapp__.Init(hwnd, IDC_BTN_RUNAPP);
					btn_remove__.Init(hwnd, IDC_BTN_REMOVE);
					btn_folder__.Init(hwnd, IDC_BTN_FOLDER);
					img_status__.Init(hwnd, IDC_SETUP_ICON);

					if (ConfigDevice != 0) img_status__.SetStatus(ConfigDevice);
					if (externalconfig)
						mcb__.AddToLog(log_string::format(L"{0}: {1}",
							LangInterface::Get().GetString(IDS_SETUP_MSG2),
							confpath__.c_str()
						));
					else
						mcb__.AddToLog(LangInterface::Get().GetString(
							(ConfigDevice == ConfigStatus::None) ? IDS_SETUP_MSG3 :
								((ConfigDevice == ConfigStatus::LoadFile) ? IDS_SETUP_MSG2 : IDS_SETUP_MSG1)
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
		void DialogConfig::EndDialog() {
			if (!hwnd__) return;
			tb__.reset();
			clear_();
		}

		void DialogConfig::OpenDragAndDrop(std::wstring s) {
			if (s.empty() || !hwnd__ || !lv__) return;
			try {

				confpath__ = s;
				std::shared_ptr<MIDI::MidiDevice> dev = std::make_shared<MIDI::MidiDevice>();
				MIDI::json_config json{};
				json.Read(dev, confpath__);

				lv__->ListViewLoad(dev);
				::SetDlgItemTextW(hwnd__, IDC_SETUP_COUNT, std::to_wstring(lv__->ListViewCount()).c_str());
				mcb__.AddToLog(log_string::format(L"{0}: {1}",
					LangInterface::Get().GetString(IDS_SETUP_MSG2),
					confpath__.c_str()
				));
				img_status__.SetStatus(ConfigStatus::LoadFile);
				tb__->AddRecent(confpath__);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogConfig::helpcategory_addtarget_(HWND hwnd, uint32_t idx, const std::wstring_view target) {
			log_string ls;
			ls << idx << Gui::GetBlank(idx) << L" - [" << target << L"] " << MIDI::MackieHelper::GetTarget(idx).data();
			HelperAdd(hwnd, idx, ls.str());
		}
		void DialogConfig::helpcategory_addmmkey_(HWND hwnd, uint32_t idx) {
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
							helpcategory_addtarget_(hwcl, i, TargetAll);
							continue;
						}
						else if (((i >= 23) && (i <= 31)) || ((i >= 41) && (i <= 49)) || (i >= 50) && (i <= 65)) {
							helpcategory_addtarget_(hwcl, i, TargetPP);
							continue;
						}
					}
					if ((k == IDC_SETUP_RADIO3) || (k == IDC_SETUP_RADIO1)) {
						if (((i >= 5) && (i <= 22)) || (i == 0) || (i == 1)) {
							helpcategory_addtarget_(hwcl, i, TargetAll);
							continue;
						}
					}
					if ((k == IDC_SETUP_RADIO4) || (k == IDC_SETUP_RADIO1)) {
						if ((i >= 66) && (i <= 76)) {
							helpcategory_addtarget_(hwcl, i, TargetPP);
							helpcategory_addmmkey_(hwcl, i);
							continue;
						}
					}
					if ((k == IDC_SETUP_RADIO1) && ((i >= 252) && (i <= 254))) {
						if ((i >= 50) && (i <= 65))
							helpcategory_addtarget_(hwcl, i, TargetPP);
						else if ((i >= 252) && (i <= 254))
							helpcategory_addtarget_(hwcl, i, TargetInt);
						else
							helpcategory_addtarget_(hwcl, i, TargetAll);
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
			if ((lpmh != nullptr) && lv__ && hwnd__) {
				(void)lv__->ListViewMenu(lpmh);
				if (hwnd__) ToolBarEditorNotify(EditorNotify::EditChangeEnable);
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
							ToolBarEditorNotify(EditorNotify::EditChangeEnable);
							SetDlgItemText(hwnd, IDC_SETUP_COUNT, std::to_wstring(lv__->ListViewCount()).c_str());
							break;
						}
						case IDM_LV_SET_MQTT:
						case IDM_LV_SET_MMKEY:
						case IDM_LV_SET_MIXER: {
							ToolBarEditorNotify(EditorNotify::EditChangeEnable);
							break;
						}
						case IDM_LV_READ_MIDI_CODE: {
							ToolBarMonitor();
							ToolBarEditorNotify(EditorNotify::EditChangeEnable);
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
				if (b) ToolBarEditorNotify(EditorNotify::EditChangeEnable);
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
				ToolBarEditorNotify(EditorNotify::SelectedEnable);

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
									ws = lang.GetString(MIDI::MackieHelper::GetTargetID(last__->unit.longtarget));
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
							set_volumeslider_(hwnd, -1);
							set_muteimage_(last__->unit.value.lvalue);
							break;
						}
						case MIDI::MidiUnitType::FADER:
						case MIDI::MidiUnitType::FADERINVERT:
						case MIDI::MidiUnitType::SLIDER:
						case MIDI::MidiUnitType::SLIDERINVERT:
						case MIDI::MidiUnitType::KNOB:
						case MIDI::MidiUnitType::KNOBINVERT: {
							set_volumeslider_(hwnd, last__->unit.value.value);
							set_muteimage_(-1);
							break;
						}
						default: {
							set_volumeslider_(hwnd, -1);
							set_muteimage_(-1);
							break;
						}
					}
				} else {
					(void) ::EnableWindow(hwcl, false);


					set_volumeslider_(hwnd, -1);
					set_muteimage_(-1);

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
		void DialogConfig::ListViewFilter(LPNMHDR lpmh) {
			if ((lpmh == nullptr) || !hwnd__ || !lv__) return;
			try {
				switch (lpmh->code) {
					case HDN_FILTERCHANGE: {
						if (!tb__->IsFilterAutoCommitCheck() || (!reinterpret_cast<LPNMHEADERW>(lpmh))) return;
						break;
					}
					case HDN_FILTERBTNCLICK: {
						if (!reinterpret_cast<LPNMHDFILTERBTNCLICK>(lpmh)) return;
						break;
					}
				}
				print_itemscount_(lv__->ListViewFilter(tb__->IsFilterTypeCheck()));
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		/// 

		void DialogConfig::ToolBarSetMode() {
			if (tb__) tb__->SetMode();
		}
		void DialogConfig::ToolBarEditDigitMode() {
			if (!hwnd__ || !tb__) return;
			try {
				tb__->SetEditorNotify(EditorNotify::EditDigits);
				lv__->EditAsDigit = tb__->IsEditDigitCheck();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ToolBarFilterEmbed() {
			if (!hwnd__ || !tb__) return;
			try {
				tb__->SetEditorNotify(EditorNotify::FilterEmbed);
				lv__->ListViewFilterEmbed(tb__->IsFilterEmbedCheck());
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ToolBarFilterType(uint32_t id) {
			if (!hwnd__ || !tb__) return;
			try {
				EditorNotify evn;
				switch (id) {
					case IDM_LV_FILTER_TYPEOR:  evn = EditorNotify::FilterSetOr; break;
					case IDM_LV_FILTER_TYPEAND: evn = EditorNotify::FilterSetAnd; break;
					default: return;
				}
				tb__->SetEditorNotify(evn);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ToolBarFilterAutoCommit() {
			if (!hwnd__ || !tb__) return;
			try {
				tb__->SetEditorNotify(EditorNotify::AutoCommit);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ToolBarSort(uint32_t id) {
			if (!lv__) return;
			try {
				bool asc;
				uint32_t col;

				switch (id) {
					case IDM_LV_SORTUP_KEY: { col = 1; asc = true; break; }
					case IDM_LV_SORTUP_SCENE: { col = 2; asc = true; break; }
					case IDM_LV_SORTUP_TARGET: { col = 4; asc = true; break; }
					case IDM_LV_SORTUP_TARGETLONG: { col = 5; asc = true; break; }
					case IDM_LV_SORTDOWN_KEY: { col = 1; asc = false; break; }
					case IDM_LV_SORTDOWN_SCENE: { col = 2; asc = false; break; }
					case IDM_LV_SORTDOWN_TARGET: { col = 4; asc = false; break; }
					case IDM_LV_SORTDOWN_TARGETLONG: { col = 5; asc = false; break; }
					default: return;
				}
				(void)lv__->ListViewSort(col, asc);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ToolBarRecentOpen(uint32_t id) {
			if (!id || !hwnd__) return;
			try {
				std::wstring s = tb__->GetRecent(id);
				if (s.empty() || !std::filesystem::exists(s)) return;
				load_file_(s);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ToolBarImport() {
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

						std::wstring s = Utils::to_string(pws);
						if (s.empty() || !std::filesystem::exists(s)) break;
						load_file_(s);
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
		void DialogConfig::ToolBarSave() {
			if (!hwnd__ || !lv__) return;
			HWND hwnd = hwnd__.get();

			try {
				common_config& cnf = common_config::Get();
				LangInterface& lang = LangInterface::Get();
				bool mainsave = !confpath__.empty() &&
					(::MessageBoxW(0,
						log_string::format(lang.GetString(IDS_DLG_MSG14), confpath__).c_str(),
						lang.GetString(IDS_START_EXT_FILTER).c_str(),
						MB_YESNO | MB_ICONQUESTION) == IDYES);

				if (!mainsave) {
					if (!lv__->ListViewGetList(cnf.Get().GetConfig())) return;
					(void) cnf.Save();
				} else {
					std::shared_ptr<MIDI::MidiDevice> dev = std::make_shared<MIDI::MidiDevice>();
					MIDI::json_config json{};
					json.Read(dev, confpath__);
					if (!lv__->ListViewGetList(dev)) return;
					json.Write(dev, confpath__, false);
				}
				ToolBarEditorNotify(EditorNotify::EditChangeDisable);
				::SetDlgItemTextW(hwnd, IDC_SETUP_COUNT, std::to_wstring(lv__->ListViewCount()).c_str());
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ToolBarExport() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				PWSTR pws = nullptr;
				IShellItem* item = nullptr;
				IFileSaveDialog* ptr = nullptr;

				try {
					std::wstring filter = LangInterface::Get().GetString(IDS_START_EXT_FILTER);
					COMDLG_FILTERSPEC extfilter[] = {
						{ filter.c_str(), L"*.cnf"}
					};
					do {
						HRESULT h = CoCreateInstance(
							CLSID_FileSaveDialog,
							NULL, CLSCTX_ALL,
							IID_IFileSaveDialog,
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

						std::shared_ptr<MIDI::MidiDevice> dev = std::make_shared<MIDI::MidiDevice>();
						dev->CopySettings(common_config::Get().GetConfig());
						if (!lv__->ListViewGetList(dev)) break;

						MIDI::json_config json{};
						std::wstring confpath = Utils::to_string(pws);
						json.Write(dev, confpath, false);
						tb__->AddRecent(confpath);

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
		void DialogConfig::ToolBarMonitor() {
			try {
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				if (!common_config::Get().Local.IsMidiBridgeRun()) return;
				if (tb__->IsReadMidiCheck())
					MIDI::MidiBridge::Get().SetCallbackOut(std::ref(mcb__));
				else
					MIDI::MidiBridge::Get().RemoveCallbackOut(std::ref(mcb__));
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogConfig::ToolBarFilters(uint32_t id) {
			if (!hwnd__ || !lv__) return;
			HWND hwnd = hwnd__.get();

			try {
				LONG count = MAKELONG(-1, 0);

				switch (id) {
					case IDM_LV_FILTER_ON: {
						MIDI::MixerUnit mu = tb__->GetFilters();
						count = lv__->ListViewFilter(mu, static_cast<bool>(mu.id));
						break;
					}
					case IDM_LV_FILTER_OFF: {
						MIDI::MixerUnit mu{};
						mu.ToNull(tb__->IsFilterTypeCheck());
						count = lv__->ListViewFilter(mu, static_cast<bool>(mu.id));
						break;
					}
					case IDM_LV_FILTER_SET: {
						MIDI::MixerUnit mu{};
						mu.ToNull(tb__->IsFilterTypeCheck());
						mu.target = mu.longtarget = MIDI::Mackie::Target::NOTARGET;
						tb__->SetFilters(mu);
						return;
					}
					case IDM_LV_FILTER_CLEAR: {
						MIDI::MixerUnit mu{};
						mu.ToNull(true);
						lv__->ListViewFiltersReset();
						tb__->SetFilters(mu);
						uint16_t cnt = lv__->ListViewCount();
						count = MAKELONG(cnt, cnt);
						break;
					}
					case IDM_LV_FILTER_MQTT:
					case IDM_LV_FILTER_MMKEY:
					case IDM_LV_FILTER_MIXER: {
						MIDI::MixerUnit mu = tb__->GetFilters();
						switch (id) {
							case IDM_LV_FILTER_MQTT:	mu.target = MIDI::Mackie::Target::MQTTKEY; break;
							case IDM_LV_FILTER_MMKEY:	mu.target = MIDI::Mackie::Target::MEDIAKEY; break;
							case IDM_LV_FILTER_MIXER:	mu.target = MIDI::Mackie::Target::VOLUMEMIX; break;
							default: return;
						}
						tb__->SetFilters(mu);
						count = lv__->ListViewFilter(mu, static_cast<bool>(mu.id));
						break;
					}
					default: return;
				}
				if (LOWORD(count) != -1) print_itemscount_(count);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::ToolBarEditorNotify(EditorNotify id) {
			if (!hwnd__) return;
			try {
				tb__->SetEditorNotify(id);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		///

		void DialogConfig::EventLog(CbEventData* data) {
			try {
				if (data == nullptr) return;
				CbEventDataDeleter d = data->GetDeleter();
				if (!hwnd__) return;
				CbEvent::ToLog(GetDlgItem(hwnd__.get(), IDC_LOG), d.GetData(), true);
			} catch (...) {}
		}
		void DialogConfig::EventMonitor(CbEventData* data) {
			try {
				if (data == nullptr) return;
				CbEventDataDeleter d = data->GetDeleter();
				
				if (!hwnd__) return;
				HWND hwnd = hwnd__.get();

				CbEvent::ToMonitor(GetDlgItem(hwnd, IDC_LOG), d.GetData(), true);
				std::pair<DWORD, MIDI::Mackie::MIDIDATA> p = data->Get<std::pair<DWORD, MIDI::Mackie::MIDIDATA>>();

				ListMixerContainer* cont = new ListMixerContainer(p.second, p.first);
				MIDI::MidiBridge::Get().RemoveCallbackOut(std::ref(mcb__));
				tb__->IsReadMidiCheck(false);

				if (lv__->ListViewInsertItem(cont) != -1) ToolBarEditorNotify(EditorNotify::EditChangeEnable);
				tb__->SetEditorNotify(common_config::Get().Local.IsMidiBridgeRun() ? EditorNotify::ReadMidiEnable : EditorNotify::ReadMidiDisable);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		/// 

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

							btn_remove__.SetEnable();
							ToolBarEditorNotify(EditorNotify::EditChangeEnable);
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
					std::wstring app = a->Item.GetName();
					if (!app.empty()) {
						(void) ::SendMessageW(hwcl, LB_ADDSTRING, 0, (LPARAM)app.c_str());
						if (last__)
							last__->unit.appvolume.push_back(app);
					}
				}
				btn_remove__.SetEnable();
				ToolBarEditorNotify(EditorNotify::EditChangeEnable);
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
					btn_remove__.SetEnable();
					ToolBarEditorNotify(EditorNotify::EditChangeEnable);
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
				set_muteimage_(last__->unit.value.lvalue);
				ToolBarEditorNotify(EditorNotify::EditChangeEnable);
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
				ToolBarEditorNotify(EditorNotify::EditChangeEnable);

				if (common_config::Get().Local.IsAudioMixerRun()) {
					uint32_t key = static_cast<uint32_t>(last__->unit.key);
					MIXER::AudioSessionMixer::Get().Volume.ByMidiId.SetVolume(key, last__->unit.value.value);
				}
					
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogConfig::set_volumeslider_(HWND hwnd, int32_t pos) {
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
		void DialogConfig::set_muteimage_(int32_t val) {
			try {
				btn_mute__.SetEnable(val >= 0);
				btn_mute__.SetStatus((val == -1) ? 0 : ((val > 0) ? 1 : 2));
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogConfig::load_file_(std::wstring s) {
			try {
				std::shared_ptr<MIDI::MidiDevice> dev = std::make_shared<MIDI::MidiDevice>();
				MIDI::json_config json{};
				json.Read(dev, s);

				mcb__.AddToLog(log_string::format(L"{0}: {1}",
					LangInterface::Get().GetString(IDS_SETUP_MSG2),
					s.c_str()
				));
				ConfigDevice = ConfigStatus::LoadFile;
				img_status__.SetStatus(ConfigDevice);

				lv__->ListViewLoad(dev);
				::SetDlgItemTextW(hwnd__.get(), IDC_SETUP_COUNT, std::to_wstring(lv__->ListViewCount()).c_str());
				confpath__ = s;
				tb__->AddRecent(s);
				ToolBarEditorNotify(EditorNotify::EditChangeEnable);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogConfig::print_itemscount_(LONG count) {
			if (!hwnd__) return;
			try {
				if (LOWORD(count) != -1) {
					log_string ls;
					ls << LOWORD(count) << L"/" << HIWORD(count);
					::SetDlgItemTextW(hwnd__, IDC_SETUP_COUNT, ls.str().c_str());
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
	}
}