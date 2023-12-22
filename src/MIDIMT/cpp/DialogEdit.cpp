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

		class ConfigName {
		public:
			static constexpr std::wstring_view ALL = L"All"sv;
			static constexpr std::wstring_view INTERNAL = L"Internal"sv;
			static constexpr std::wstring_view PREMIEREPRO = L"Premiere Pro"sv;
			static constexpr std::wstring_view MULTIMEDIAKEY = L"Multimedia Key"sv;

			static const uint16_t IDS[];
			static const uint16_t IDN[];
		};
		const uint16_t ConfigName::IDS[] = { 0, DLG_EDIT_SETUP_NUMBER, DLG_EDIT_SETUP_SCENE,  DLG_EDIT_SETUP_TYPE,  DLG_EDIT_SETUP_TARGET,  DLG_EDIT_SETUP_TARGET_LONG };
		const uint16_t ConfigName::IDN[] = { 0, DLG_EDIT_SETUP_NUMBER, DLG_EDIT_SETUP_SCENEN, DLG_EDIT_SETUP_TYPEN, DLG_EDIT_SETUP_TARGETN, DLG_EDIT_SETUP_TARGET_LONGN };

		static inline void helper_add__(HWND hwnd, uint32_t idx, std::wstring s) {
			int pos = (int)::SendMessageW(hwnd, LB_ADDSTRING, 0, (LPARAM)s.c_str());
			(void) ::SendMessageW(hwnd, LB_SETITEMDATA, pos, (LPARAM)idx);
		}
		static inline HICON get_icon__(uint16_t n) {
			return LangInterface::Get().GetIcon24x24(MAKEINTRESOURCEW(n));
		}

		DialogEdit::DialogEdit() {
			CbEvent::Init(DLG_EVENT_LOG, DLG_EVENT_MONITOR);
			CbEvent::GetHwndCb = [=]() { return hwnd_.get(); };

			try {
				uint16_t status_icos[]{ ICON_APP_ICON_SETUP1, ICON_APP_ICON_SETUP2, ICON_APP_ICON_SETUP3 };
				img_status_.Init(
					status_icos,
					std::size(status_icos),
					&get_icon__,
					true);

				uint16_t btn_mute_icos[]{ ICON_APP_ICON_SETUP1, ICON_APP_ICON_SOUNDON, ICON_APP_ICON_SOUNDOFF };
				btn_mute_.Init(
					btn_mute_icos,
					std::size(btn_mute_icos),
					&get_icon__);

				uint16_t btn_folder_icos[]{ ICON_APP_ICON_FOLDER };
				btn_folder_.Init(
					btn_folder_icos,
					std::size(btn_folder_icos),
					&get_icon__);

				uint16_t btn_app_icos[]{ ICON_APP_ICON_RUNAPP };
				btn_runapp_.Init(
					btn_app_icos,
					std::size(btn_app_icos),
					&get_icon__);

				uint16_t btn_delete_icos[]{ ICON_APP_ICON_REMOVE };
				btn_remove_.Init(
					btn_delete_icos,
					std::size(btn_delete_icos),
					&get_icon__);

				uint16_t btn_info_icos[]{ ICON_APP_ICON_INFO };
				btn_info_.Init(
					btn_info_icos,
					std::size(btn_info_icos),
					&get_icon__);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		DialogEdit::~DialogEdit() {
			dispose_();
		}

		#pragma region Initialize
		void DialogEdit::dispose_() {
			clear_();
			try {
				btn_mute_.Release();
				btn_info_.Release();
				btn_runapp_.Release();
				btn_remove_.Release();
				btn_folder_.Release();
				img_status_.Release();
			} catch (...) {}
		}
		void DialogEdit::clear_() {
			try {
				IO::IOBridge::Get().UnSetCb(*static_cast<CbEvent*>(this));
				ConfigDevice = ConfigStatus::None;
				confpath_ = std::wstring();

				isload_ = false;
				hwnd_.reset();

				if (lv_) {
					lv_->ListViewEnd();
					lv_.reset();
				}

				tb_.reset();

				btn_mute_.Reset();
				btn_info_.Reset();
				btn_folder_.Reset();
				btn_runapp_.Reset();
				btn_remove_.Reset();
				img_status_.Reset();
			} catch (...) {}
		}
		void DialogEdit::init_() {
			try {
				common_config& cnf = common_config::Get();
				const bool isstarted = IO::IOBridge::Get().IsStarted();

				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_EDIT_SETUP_LIST))) {
					clear_();
					return;
				}

				tb_.reset(new RToolBarDialogEdit(hwnd_, L"CONFIG_EDITOR_RIBBON"));
				init_lv_(hi);

				btn_mute_.Init(hwnd_, DLG_EDIT_BTN_MUTE);
				btn_info_.Init(hwnd_, DLG_EDIT_BTN_INFO);
				btn_runapp_.Init(hwnd_, DLG_EDIT_BTN_RUNAPP);
				btn_remove_.Init(hwnd_, DLG_EDIT_BTN_REMOVE);
				btn_folder_.Init(hwnd_, DLG_EDIT_BTN_FOLDER);
				img_status_.Init(hwnd_, DLG_EDIT_SETUP_ICON);

				if (!confpath_.empty() && std::filesystem::exists(confpath_)) {
					auto mmt = std::make_shared<JSON::MMTConfig>();
					try {
						JSON::json_config json{};
						json.Read(mmt, confpath_);

					} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }

					ConfigDevice = mmt->empty() ? ConfigStatus::None : ConfigStatus::LoadFile;
					init_config_(mmt);

				} else {
					std::shared_ptr<JSON::MMTConfig>& mmt = cnf.GetConfig();

					ConfigDevice = mmt->empty() ? ConfigStatus::None : (isstarted ? ConfigStatus::LoadDevice : ConfigStatus::LoadFile);
					if (ConfigDevice == ConfigStatus::None)
						ConfigDevice = (cnf.Load() && !cnf.GetConfig()->empty()) ? ConfigStatus::LoadFile : ConfigStatus::None;
					init_config_(mmt);
				}

				::CheckRadioButton(hwnd_, DLG_EDIT_SETUP_RADIO1, DLG_EDIT_SETUP_RADIO4, DLG_EDIT_SETUP_RADIO1);
				helpcategory_selected_(DLG_EDIT_SETUP_RADIO1);

				isload_ = true;

				tb_->SetEditorNotify(isstarted ? EditorNotify::ReadMidiEnable : EditorNotify::ReadMidiDisable);
				tb_->SetEditorNotify(EditorNotify::ItemEmpty);

				CbEvent::AddToLog(LangInterface::Get().GetString(
					(ConfigDevice == ConfigStatus::None) ? STRING_EDIT_MSG3 :
					((ConfigDevice == ConfigStatus::LoadFile) ? STRING_EDIT_MSG2 : STRING_EDIT_MSG1)
				));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::init_config_(std::shared_ptr<JSON::MMTConfig>& mmt) {
			bool b = (ConfigDevice != ConfigStatus::None);
			if (b) lv_->ListViewLoad(mmt);
			else   lv_->ListViewClear();

			::SetDlgItemTextW(hwnd_, DLG_EDIT_SETUP_COUNT, (b ? std::to_wstring(lv_->ListViewCount()).c_str() : L"0"));

			if (b && !confpath_.empty() && (ConfigDevice == ConfigStatus::LoadFile))
				CbEvent::AddToLog(log_string::format(L"{0}: {1}",
					LangInterface::Get().GetString(STRING_EDIT_MSG2),
					confpath_.c_str()
				));

			if (b) img_status_.SetStatus(static_cast<uint16_t>(ConfigDevice));
		}
		void DialogEdit::init_lv_(HWND h) {
			try {
				if (lv_)
					lv_.reset();
				lv_ = std::make_unique<ListEdit>();
				lv_->ListViewInit(h);
				lv_->ListViewErrorCb(
					std::bind(static_cast<void(DialogEdit::*)(std::wstring)>(&CbEvent::AddToLog), this, _1)
				);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogEdit::ExternalConfig(COPYDATASTRUCT* data) {
			try {
				if (!data) return;
				do {
					if (!hwnd_) break;
					if ((data->dwData != CheckRun::Get().GetMsgId()) || !data->cbData) break;
					wchar_t* cnfs = reinterpret_cast<wchar_t*>(data->lpData);
					if (cnfs) confpath_ = std::wstring(cnfs, cnfs + (data->cbData - 1));
					else	  confpath_ = std::wstring();

					if (confpath_.empty()) break;
					if (!std::filesystem::exists(confpath_)) {
						confpath_ = std::wstring();
						break;
					}
					ConfigDevice = ConfigStatus::LoadFile;

				} while (0);
				if (data->lpData)
					if (data->lpData) delete[] data->lpData;
				delete data;

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		IO::PluginUi* DialogEdit::GetUi() {
			return static_cast<IO::PluginUi*>(this);
		}

		const bool DialogEdit::IsRunOnce() {
			return !hwnd_ && !isload_;
		}
		void DialogEdit::SetFocus() {
			if (hwnd_) (void) ::SetFocus(hwnd_);
		}

		#pragma region Events
		void DialogEdit::event_Log_(CbEventData* data) {
			try {
				if (!data) return;
				CbEventDataDeleter d = data->GetDeleter();
				if (!hwnd_) return;
				CbEvent::ToLog(::GetDlgItem(hwnd_, DLG_EDIT_LOG), d.GetData(), true);
			} catch (...) {}
		}
		void DialogEdit::event_Monitor_(CbEventData* data) {
			try {
				if (!data) return;
				CbEventDataDeleter d = data->GetDeleter();
				if (!hwnd_) return;
				CbEvent::ToMonitor(::GetDlgItem(hwnd_, DLG_EDIT_LOG), d.GetData(), true);

				std::pair<DWORD, MIDI::Mackie::MIDIDATA> p = data->Get<std::pair<DWORD, MIDI::Mackie::MIDIDATA>>();
				ListMixerContainer* cont = new ListMixerContainer(p.second, p.first);
				IO::IOBridge::Get().UnSetCb(*static_cast<CbEvent*>(this));
				tb_->IsReadMidiCheck(false);
				tb_->SetEditorNotify(IO::IOBridge::Get().IsStarted() ? EditorNotify::ReadMidiEnable : EditorNotify::ReadMidiDisable);
				if (lv_->ListViewInsertItem(cont) != -1) tb_edit_notify_(EditorNotify::EditChangeEnable);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::event_DragAndDrop_(std::wstring s) {
			if (s.empty() || !hwnd_ || !lv_) return;
			try {
				confpath_ = s;
				auto mmt = std::make_shared<JSON::MMTConfig>();
				JSON::json_config json{};
				json.Read(mmt, confpath_);

				lv_->ListViewLoad(mmt);
				::SetDlgItemTextW(hwnd_, DLG_EDIT_SETUP_COUNT, std::to_wstring(lv_->ListViewCount()).c_str());
				CbEvent::AddToLog(log_string::format(L"{0}: {1}",
					LangInterface::Get().GetString(STRING_EDIT_MSG2),
					confpath_.c_str()
				));
				img_status_.SetStatus(static_cast<uint16_t>(ConfigStatus::LoadFile));
				tb_->AddRecent(confpath_);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region Change On events
		void DialogEdit::changeOnAppBox_() {
			if (!hwnd_) return;
			try {
				btn_remove_.SetEnable();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::changeOnBtnAppFolder_() {
			try {
				if (!hwnd_) return;

				PWSTR pws{ nullptr };
				IShellItem* item{ nullptr };
				IFileOpenDialog* ptr{ nullptr };

				try {
					std::wstring filter = common_error_code::Get().get_error(common_error_id::err_MIDIMT_CONFFILTER);
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
						h = ptr->Show(hwnd_);
						if (h != S_OK) break;
						h = ptr->GetResult(&item);
						if (h != S_OK) break;
						h = item->GetDisplayName(SIGDN_FILESYSPATH, &pws);
						if (h != S_OK) break;

						std::wstring app = Utils::device_out_name(Utils::to_string(pws), L"");
						if (!app.empty()) {
							HWND hwcl;
							if ((hwcl = ::GetDlgItem(hwnd_, DLG_EDIT_SETUP_APPLIST)))
								(void) ::SendMessageW(hwcl, LB_ADDSTRING, 0, (LPARAM)app.c_str());

							if (last_)
								last_->unit.appvolume.push_back(app);

							btn_remove_.SetEnable();
							tb_edit_notify_(EditorNotify::EditChangeEnable);
						}
					} while (0);
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				if (item != nullptr) item->Release();
				if (ptr != nullptr) ptr->Release();
				if (pws != nullptr) ::CoTaskMemFree(pws);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::changeOnBtnAppRunning_() {
			if (!hwnd_ || !common_config::Get().Local.IsAudioMixerRun()) return;

			try {
				ClassStorage& st = ClassStorage::Get();
				if (!st.IsDialog<MIXER::AudioSessionMixer>()) return;

				HWND hwcl;
				if (!(hwcl = ::GetDlgItem(hwnd_, DLG_EDIT_SETUP_APPLIST))) return;

				MIXER::ASLIST_t& list = st.GetDialog<MIXER::AudioSessionMixer>()->GetSessionList();
				for (auto& a : list) {
					std::wstring app = a->Item.GetName();
					if (!app.empty()) {
						(void) ::SendMessageW(hwcl, LB_ADDSTRING, 0, (LPARAM)app.c_str());
						if (last_)
							last_->unit.appvolume.push_back(app);
					}
				}
				btn_remove_.SetEnable();
				tb_edit_notify_(EditorNotify::EditChangeEnable);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::changeOnBtnAppDelete_() {
			if (!hwnd_) return;

			try {
				int idx;
				HWND hwcl;
				if (!(hwcl = ::GetDlgItem(hwnd_, DLG_EDIT_SETUP_APPLIST))) return;
				if ((idx = ListBox_GetCurSel(hwcl)) == LB_ERR) return;
				if (last_) {
					wchar_t buf[MAX_PATH]{};
					ListBox_GetText(hwcl, idx, buf);
					std::wstring app = Utils::to_string(buf);
					if (!app.empty())
						last_->unit.appvolume.erase(
							std::remove_if(last_->unit.appvolume.begin(), last_->unit.appvolume.end(),
								[app](std::wstring& s) {
						return app._Equal(s);
					})
						);
				}

				(void)ListBox_DeleteString(hwcl, idx);
				if (ListBox_GetCount(hwcl) <= 0) {
					btn_remove_.SetEnable();
					tb_edit_notify_(EditorNotify::EditChangeEnable);
				}
				else btn_remove_.SetEnable(false);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::changeOnBtnMute_() {
			if (!hwnd_ || !last_) return;

			try {
				last_->unit.value.lvalue = !last_->unit.value.lvalue;
				last_->unit.value.value = last_->unit.value.lvalue ? 127 : 0;
				set_muteimage_(last_->unit.value.lvalue);
				tb_edit_notify_(EditorNotify::EditChangeEnable);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::changeOnSlider_() {
			if (!hwnd_ || !last_) return;

			try {
				HWND hwcl = GetDlgItem(hwnd_, DLG_EDIT_SLIDER_VOLUME);
				if (hwcl == nullptr) return;
				last_->unit.value.value = static_cast<uint8_t>(::SendMessageW(hwcl, TBM_GETPOS, 0, 0));
				tb_edit_notify_(EditorNotify::EditChangeEnable);

				if (common_config::Get().Local.IsAudioMixerRun()) {
					ClassStorage& st = ClassStorage::Get();
					if (!st.IsDialog<MIXER::AudioSessionMixer>()) return;
					uint32_t key = static_cast<uint32_t>(last_->unit.key);
					st.GetDialog<MIXER::AudioSessionMixer>()->Volume.ByMidiId.SetVolume(key, last_->unit.value.value);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogEdit::helpcategory_addtarget_(HWND hwnd, uint32_t idx, const std::wstring_view target) {
			log_string ls;
			ls << idx << UI::UiUtils::GetBlank(idx) << L" - [" << target << L"] " << MIDI::MackieHelper::GetTarget(idx).data();
			helper_add__(hwnd, idx, ls.str());
		}
		void DialogEdit::helpcategory_addmmkey_(HWND hwnd, uint32_t idx) {
			log_string ls;
			ls << idx << UI::UiUtils::GetBlank(idx) << L" - [" << ConfigName::MULTIMEDIAKEY << L"] " << MIDI::MackieHelper::GetTranslateMMKey(idx).data();
			helper_add__(hwnd, idx, ls.str());
		}
		void DialogEdit::helpcategory_selected_(uint32_t k) {
			try {
				if (!hwnd_) return;

				HWND hwcl;
				if (!(hwcl = ::GetDlgItem(hwnd_, DLG_EDIT_SETUP_HELPLIST))) return;

				(void) ListBox_ResetContent(hwcl);

				for (uint32_t i = 0, n = 0; i <= static_cast<uint32_t>(MIDI::Mackie::Target::VOLUMEMIX); i++) {
					if ((k == DLG_EDIT_SETUP_RADIO2) || (k == DLG_EDIT_SETUP_RADIO1)) {
						if (((i >= 32) && (i <= 40)) || (i == 2) || (i == 3)) {
							helpcategory_addtarget_(hwcl, i, ConfigName::ALL);
							continue;
						}
						else if (((i >= 23) && (i <= 31)) || ((i >= 41) && (i <= 49)) || (i >= 50) && (i <= 65)) {
							helpcategory_addtarget_(hwcl, i, ConfigName::PREMIEREPRO);
							continue;
						}
					}
					if ((k == DLG_EDIT_SETUP_RADIO3) || (k == DLG_EDIT_SETUP_RADIO1)) {
						if (((i >= 5) && (i <= 22)) || (i == 0) || (i == 1)) {
							helpcategory_addtarget_(hwcl, i, ConfigName::ALL);
							continue;
						}
					}
					if ((k == DLG_EDIT_SETUP_RADIO4) || (k == DLG_EDIT_SETUP_RADIO1)) {
						if ((i >= 66) && (i <= 76)) {
							helpcategory_addtarget_(hwcl, i, ConfigName::PREMIEREPRO);
							helpcategory_addmmkey_(hwcl, i);
							continue;
						}
					}
					if ((k == DLG_EDIT_SETUP_RADIO1) && ((i >= 252) && (i <= 254))) {
						if ((i >= 50) && (i <= 65))
							helpcategory_addtarget_(hwcl, i, ConfigName::PREMIEREPRO);
						else if ((i >= 252) && (i <= 254))
							helpcategory_addtarget_(hwcl, i, ConfigName::INTERNAL);
						else
							helpcategory_addtarget_(hwcl, i, ConfigName::ALL);
					}
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region Setters UI Controls
		void DialogEdit::set_volumeslider_(HWND hwnd, int32_t pos) {
			try {
				HWND hi = ::GetDlgItem(hwnd, DLG_EDIT_SLIDER_VOLUME);
				if (!hi) return;
				(void) ::EnableWindow(hi, (pos >= 0));
				pos = (pos < 0) ? 0 : pos;
				(void) ::SendMessageW(hi, TBM_SETRANGE, static_cast<WPARAM>(FALSE), static_cast<LPARAM>(MAKELONG(0, 127)));
				(void) ::SendMessageW(hi, TBM_SETPOS, static_cast<WPARAM>(TRUE), static_cast<LPARAM>(pos));
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::set_muteimage_(int32_t val) {
			try {
				btn_mute_.SetEnable(val >= 0);
				btn_mute_.SetStatus((val == -1) ? 0 : ((val > 0) ? 1 : 2));
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::load_file_(std::wstring s) {
			try {
				auto mmt = std::make_shared<JSON::MMTConfig>();
				JSON::json_config json{};
				json.Read(mmt, s);

				CbEvent::AddToLog(log_string::format(L"{0}: {1}",
					LangInterface::Get().GetString(STRING_EDIT_MSG2),
					s.c_str()
				));
				ConfigDevice = ConfigStatus::LoadFile;
				img_status_.SetStatus(static_cast<uint16_t>(ConfigDevice));

				lv_->ListViewLoad(mmt);
				::SetDlgItemTextW(hwnd_.get(), DLG_EDIT_SETUP_COUNT, std::to_wstring(lv_->ListViewCount()).c_str());
				confpath_ = s;
				tb_->AddRecent(s);
				tb_edit_notify_(EditorNotify::EditChangeEnable);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::print_itemscount_(LONG count) {
			if (!hwnd_) return;
			try {
				if (LOWORD(count) != -1) {
					log_string ls;
					ls << LOWORD(count) << L"/" << HIWORD(count);
					::SetDlgItemTextW(hwnd_, DLG_EDIT_SETUP_COUNT, ls.str().c_str());
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region List View Control
		void DialogEdit::lv_menu_(LPNMHDR lpmh) {
			if ((lpmh) && lv_ && hwnd_) {
				(void)lv_->ListViewMenu(lpmh);
				tb_edit_notify_(EditorNotify::EditChangeEnable);
			}
		}
		void DialogEdit::lv_menu_(uint32_t id) {
			if (!hwnd_ || !lv_) return;

			try {
				bool b = lv_->ListViewMenu(id);
				if (b) {
					switch (id) {
						case IDM_LV_NEW:
						case IDM_LV_PASTE:
						case IDM_LV_DELETE: {
							tb_edit_notify_(EditorNotify::EditChangeEnable);
							SetDlgItemText(hwnd_, DLG_EDIT_SETUP_COUNT, std::to_wstring(lv_->ListViewCount()).c_str());
							break;
						}
						case IDM_LV_SET_MQTT:
						case IDM_LV_SET_MMKEY:
						case IDM_LV_SET_MIXER: {
							tb_edit_notify_(EditorNotify::EditChangeEnable);
							break;
						}
						case IDM_LV_READ_MIDI_CODE: {
							tb_monitor_();
							tb_edit_notify_(EditorNotify::EditChangeEnable);
							break;
						}
						default: break;
					}
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::lv_sort_(LPNMHDR lpmh) {
			if ((lpmh) && lv_)
				(void)lv_->ListViewSort(lpmh);
		}
		void DialogEdit::lv_edit_(LPNMHDR lpmh) {
			if (!lpmh || !hwnd_ || !lv_) return;
			
			try {
				bool b = lv_->ListViewSetItem(lpmh);
				if (b) tb_edit_notify_(EditorNotify::EditChangeEnable);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::lv_click_(LPNMHDR lpmh) {
			if (!hwnd_ || !lv_ || !lpmh) return;

			try {
				last_.reset(lv_->ListViewGetSelectedRow(lpmh));
				if (!last_ ||
					(last_->unit.key >= 255) ||
					(last_->unit.scene >= 255U) ||
					(last_->unit.type == MIDI::MidiUnitType::UNITNONE))
					return;

				LangInterface& lang = LangInterface::Get();
				tb_edit_notify_(EditorNotify::SelectedEnable);

				for (size_t i = 1; i < lv_->ListViewColumns(); i++) {
					std::wstring ws{}, wn{};
					switch (i) {
						case 1: {
							wn = std::to_wstring(static_cast<uint16_t>(last_->unit.key));
							break;
						}
						case 2: {
							wn = std::to_wstring(static_cast<uint16_t>(last_->unit.scene));
							ws = MIDI::MidiHelper::GetScene(last_->unit.scene);
							break;
						}
						case 3: {
							wn = std::to_wstring(static_cast<uint16_t>(last_->unit.type));
							ws = MIDI::MidiHelper::GetType(last_->unit.type);
							break;
						}
						case 4: {
							wn = std::to_wstring(static_cast<uint16_t>(last_->unit.target));
							ws = lang.GetString(MIDI::MackieHelper::GetTargetID(last_->unit.target));
							break;
						}
						case 5: {
							wn = std::to_wstring(static_cast<uint16_t>(last_->unit.longtarget));
							switch (last_->unit.target) {
								using enum MIDI::Mackie::Target;
								case MEDIAKEY: {
									ws = MIDI::MackieHelper::GetTranslateMMKey(last_->unit.longtarget);
									break;
								}
								case MQTTKEY: {
									ws = lang.GetString(MIDI::MackieHelper::GetSmartHomeTargetID(last_->unit.longtarget));
									break;
								}
								case LIGHTKEY8B:
								case LIGHTKEY16B: {
									ws = lang.GetString(STRING_EDIT_MSG5);
									break;
								}
								default:{
									ws = lang.GetString(MIDI::MackieHelper::GetTargetID(last_->unit.longtarget));
									break;
								}
							}
							break;
						}
						default: continue;
					}
					if (i != 1)
						::SetDlgItemTextW(hwnd_, ConfigName::IDS[i], ws.c_str());
					::SetDlgItemTextW(hwnd_, ConfigName::IDN[i], wn.c_str());
				}

				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_EDIT_SETUP_APPLIST))) return;
				(void)ListBox_ResetContent(hi);

				if (last_->unit.target == MIDI::Mackie::Target::VOLUMEMIX) {
					(void) ::EnableWindow(hi, true);

					if (!last_->unit.appvolume.empty()) {
						for (auto& s : last_->unit.appvolume)
							(void) ::SendMessageW(hi, LB_ADDSTRING, 0, (LPARAM)s.c_str());
					}

					if (common_config::Get().Local.IsAudioMixerRun())
						btn_runapp_.SetEnable();
					btn_folder_.SetEnable();
					btn_remove_.SetEnable(false);

					switch (last_->unit.type) {
						case MIDI::MidiUnitType::BTN:
						case MIDI::MidiUnitType::BTNTOGGLE: {
							set_volumeslider_(hwnd_, -1);
							set_muteimage_(last_->unit.value.lvalue);
							break;
						}
						case MIDI::MidiUnitType::FADER:
						case MIDI::MidiUnitType::FADERINVERT:
						case MIDI::MidiUnitType::SLIDER:
						case MIDI::MidiUnitType::SLIDERINVERT:
						case MIDI::MidiUnitType::KNOB:
						case MIDI::MidiUnitType::KNOBINVERT: {
							set_volumeslider_(hwnd_, last_->unit.value.value);
							set_muteimage_(-1);
							break;
						}
						default: {
							set_volumeslider_(hwnd_, -1);
							set_muteimage_(-1);
							break;
						}
					}
				} else {
					(void) ::EnableWindow(hi, false);


					set_volumeslider_(hwnd_, -1);
					set_muteimage_(-1);

					btn_runapp_.SetEnable(false);
					btn_folder_.SetEnable(false);
					btn_remove_.SetEnable(false);
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::lv_dbclick_(LPNMHDR lpmh) {
			if ((lpmh) && lv_)
				(void) lv_->ListViewEdit(lpmh);
		}
		void DialogEdit::lv_filter_(LPNMHDR lpmh) {
			if (!lpmh || !hwnd_ || !lv_) return;
			try {
				switch (lpmh->code) {
					case HDN_FILTERCHANGE: {
						if (!tb_->IsFilterAutoCommitCheck() || (!reinterpret_cast<LPNMHEADERW>(lpmh))) return;
						break;
					}
					case HDN_FILTERBTNCLICK: {
						if (!reinterpret_cast<LPNMHDFILTERBTNCLICK>(lpmh)) return;
						break;
					}
				}
				print_itemscount_(lv_->ListViewFilter(tb_->IsFilterTypeCheck()));
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region Tool Bar Control
		void DialogEdit::tb_sort_(uint32_t id) {
			if (!lv_) return;
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
				(void)lv_->ListViewSort(col, asc);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::tb_recent_open_(uint32_t id) {
			if (!id || !hwnd_) return;
			try {
				std::wstring s = tb_->GetRecent(id);
				if (s.empty() || !std::filesystem::exists(s)) return;
				load_file_(s);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::tb_filter_embed_() {
			if (!hwnd_ || !tb_) return;
			try {
				tb_->SetEditorNotify(EditorNotify::FilterEmbed);
				lv_->ListViewFilterEmbed(tb_->IsFilterEmbedCheck());
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::tb_filter_type_(uint32_t id) {
			if (!hwnd_ || !tb_) return;
			try {
				EditorNotify evn;
				switch (id) {
					case IDM_LV_FILTER_TYPEOR:  evn = EditorNotify::FilterSetOr; break;
					case IDM_LV_FILTER_TYPEAND: evn = EditorNotify::FilterSetAnd; break;
					default: return;
				}
				tb_->SetEditorNotify(evn);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::tb_edit_digitmode_() {
			if (!hwnd_ || !tb_) return;
			try {
				tb_->SetEditorNotify(EditorNotify::EditDigits);
				lv_->EditAsDigit = tb_->IsEditDigitCheck();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::tb_edit_notify_(EditorNotify id) {
			if (!hwnd_) return;
			try {
				tb_->SetEditorNotify(id);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::tb_setmode_() {
			if (tb_) tb_->SetMode();
		}
		void DialogEdit::tb_save_() {
			if (!hwnd_ || !lv_) return;

			try {
				common_config& cnf = common_config::Get();
				LangInterface& lang = LangInterface::Get();
				bool extsave = !confpath_.empty() &&
					(::MessageBoxW(0,
						log_string::format(lang.GetString(STRING_EDIT_MSG4), confpath_).c_str(),
						lang.GetString(STRING_CHKR_MSG2).c_str(),
						MB_YESNO | MB_ICONQUESTION) == IDYES);

				if (!extsave) {
					if (!lv_->ListViewGetList(cnf.Get().GetConfig())) return;
					(void)cnf.Save();
				}
				else {
					auto mmt = std::make_shared<JSON::MMTConfig>();
					JSON::json_config json{};
					json.Read(mmt, confpath_);
					if (!lv_->ListViewGetList(mmt)) return;
					json.Write(mmt, confpath_, false);
				}
				tb_edit_notify_(EditorNotify::EditChangeDisable);
				::SetDlgItemTextW(hwnd_, DLG_EDIT_SETUP_COUNT, std::to_wstring(lv_->ListViewCount()).c_str());
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::tb_import_() {
			try {
				if (!hwnd_) return;

				PWSTR pws{ nullptr };
				IShellItem* item{ nullptr };
				IFileOpenDialog* ptr{ nullptr };

				try {
					std::wstring filter = common_error_code::Get().get_error(common_error_id::err_MIDIMT_CONFFILTER);
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
						h = ptr->Show(hwnd_);
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
		void DialogEdit::tb_export_() {
			try {
				if (!hwnd_) return;

				PWSTR pws{ nullptr };
				IShellItem* item{ nullptr };
				IFileSaveDialog* ptr{ nullptr };

				try {
					std::wstring filter = common_error_code::Get().get_error(common_error_id::err_MIDIMT_CONFFILTER);
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
						h = ptr->Show(hwnd_);
						if (h != S_OK) break;
						h = ptr->GetResult(&item);
						if (h != S_OK) break;
						h = item->GetDisplayName(SIGDN_FILESYSPATH, &pws);
						if (h != S_OK) break;

						auto mmt = std::make_shared<JSON::MMTConfig>();
						mmt->CopySettings(common_config::Get().GetConfig());
						if (!lv_->ListViewGetList(mmt)) break;

						JSON::json_config json{};
						std::wstring confpath = Utils::to_string(pws);
						json.Write(mmt, confpath, false);
						tb_->AddRecent(confpath);

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
		void DialogEdit::tb_monitor_() {
			try {
				if (!hwnd_) return;

				if (IO::IOBridge::Get().IsStoped()) return;
				if (tb_->IsReadMidiCheck())
					IO::IOBridge::Get().SetCb(*static_cast<CbEvent*>(this));
				else
					IO::IOBridge::Get().UnSetCb(*static_cast<CbEvent*>(this));
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::tb_filters_(uint32_t id) {
			if (!hwnd_ || !lv_) return;
			try {
				LONG count = MAKELONG(-1, 0);

				switch (id) {
					case IDM_LV_FILTER_ON: {
						MIDI::MixerUnit mu = tb_->GetFilters();
						count = lv_->ListViewFilter(mu, static_cast<bool>(mu.id));
						break;
					}
					case IDM_LV_FILTER_OFF: {
						MIDI::MixerUnit mu{};
						mu.ToNull(tb_->IsFilterTypeCheck());
						count = lv_->ListViewFilter(mu, static_cast<bool>(mu.id));
						break;
					}
					case IDM_LV_FILTER_SET: {
						MIDI::MixerUnit mu{};
						mu.ToNull(tb_->IsFilterTypeCheck());
						mu.target = mu.longtarget = MIDI::Mackie::Target::NOTARGET;
						tb_->SetFilters(mu);
						return;
					}
					case IDM_LV_FILTER_CLEAR: {
						MIDI::MixerUnit mu{};
						mu.ToNull(true);
						lv_->ListViewFiltersReset();
						tb_->SetFilters(mu);
						uint16_t cnt = lv_->ListViewCount();
						count = MAKELONG(cnt, cnt);
						break;
					}
					case IDM_LV_FILTER_MQTT:
					case IDM_LV_FILTER_MMKEY:
					case IDM_LV_FILTER_MIXER: {
						MIDI::MixerUnit mu = tb_->GetFilters();
						switch (id) {
							case IDM_LV_FILTER_MQTT:	mu.target = MIDI::Mackie::Target::MQTTKEY; break;
							case IDM_LV_FILTER_MMKEY:	mu.target = MIDI::Mackie::Target::MEDIAKEY; break;
							case IDM_LV_FILTER_MIXER:	mu.target = MIDI::Mackie::Target::VOLUMEMIX; break;
							default: return;
						}
						tb_->SetFilters(mu);
						count = lv_->ListViewFilter(mu, static_cast<bool>(mu.id));
						break;
					}
					default: return;
				}
				if (LOWORD(count) != -1) print_itemscount_(count);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogEdit::tb_filter_auto_commit_() {
			if (!hwnd_ || !tb_) return;
			try {
				tb_->SetEditorNotify(EditorNotify::AutoCommit);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region Override
		HWND DialogEdit::BuildDialog(HWND h) {
			return IO::PluginUi::BuildDialog(LangInterface::Get().GetLangHinstance(), h, MAKEINTRESOURCEW(DLG_EDIT_WINDOW));
		}
		LRESULT DialogEdit::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
			try {
				switch (m) {
					case WM_INITDIALOG: {
						hwnd_.reset(h, static_cast<SUBCLASSPROC>(&PluginUi::DialogProc_), reinterpret_cast<DWORD_PTR>(this), 0);
						init_();
						::ShowWindow(h, SW_SHOW);
						::SetFocus(::GetDlgItem(h, IDCANCEL));
						return static_cast<INT_PTR>(1);
					}
					case WM_DROPFILES: {
						if (!w) break;
						event_DragAndDrop_(UI::UiUtils::GetDragAndDrop(reinterpret_cast<HDROP>(w)));
						return static_cast<INT_PTR>(1);
					}
					case WM_HSCROLL: {
						if (l == 0) break;
						switch (LOWORD(w)) {
							case SB_LINELEFT:
							case SB_LINERIGHT:
							case SB_THUMBPOSITION: {
								changeOnSlider_();
								break;
							}
						}
						break;
					}
					case WM_NOTIFY: {
						LPNMHDR lpmh = (LPNMHDR)l;
						switch (lpmh->idFrom) {
							case DLG_EDIT_SETUP_LIST: {
								switch (lpmh->code) {
									case (UINT)LVN_BEGINLABELEDIT: {
										break;
									}
									case (UINT)LVN_ENDLABELEDIT: {
										lv_edit_(lpmh);
										break;
									}
									case (UINT)NM_DBLCLK: {
										lv_dbclick_(lpmh);
										break;
									}
									case (UINT)NM_CLICK: {
										lv_click_(lpmh);
										break;
									}
									case (UINT)NM_RCLICK: {
										lv_menu_(lpmh);
										break;
									}
									case (UINT)LVN_COLUMNCLICK: {
										lv_sort_(lpmh);
										break;
									}
									default: return static_cast<INT_PTR>(0);
								}
								return static_cast<INT_PTR>(1);
							}
							case 0: {
								switch (lpmh->code) {
									case HDN_FILTERCHANGE:
									case HDN_FILTERBTNCLICK: {
										lv_filter_(lpmh);
										break;
									}
									default: return static_cast<INT_PTR>(0);
								}
								return static_cast<INT_PTR>(1);
							}
							default: break;
						}
						break;
					}
					case WM_HELP: {
						if (!l) break;
						UI::UiUtils::ShowHelpPage(DLG_EDIT_WINDOW, reinterpret_cast<HELPINFO*>(l));
						return static_cast<INT_PTR>(1);
					}
					case WM_COMMAND: {
						if (!isload_) break;
						uint16_t c{ LOWORD(w) };
						switch (c) {
							case DLG_EVENT_LOG: {
								event_Log_(reinterpret_cast<MIDIMT::CbEventData*>(l));
								break;
							}
							case DLG_EVENT_MONITOR: {
								event_Monitor_(reinterpret_cast<MIDIMT::CbEventData*>(l));
								break;
							}
							case DLG_GO_HELP: {
								UI::UiUtils::ShowHelpPage(DLG_EDIT_WINDOW, static_cast<uint16_t>(0U));
								break;
							}

							#pragma region DLG_EDIT_*
							case DLG_EDIT_SETUP_RADIO1:
							case DLG_EDIT_SETUP_RADIO2:
							case DLG_EDIT_SETUP_RADIO3:
							case DLG_EDIT_SETUP_RADIO4: {
								helpcategory_selected_(static_cast<uint32_t>(c));
								break;
							}
							case DLG_EDIT_SETUP_APPLIST: {
								if (HIWORD(w) == LBN_SELCHANGE || HIWORD(w) == LBN_DBLCLK)
									changeOnAppBox_();
								break;
							}
							case DLG_EDIT_BTN_FOLDER: {
								changeOnBtnAppFolder_();
								break;
							}
							case DLG_EDIT_BTN_RUNAPP: {
								changeOnBtnAppRunning_();
								break;
							}
							case DLG_EDIT_BTN_REMOVE: {
								changeOnBtnAppDelete_();
								break;
							}
							case DLG_EDIT_BTN_MUTE: {
								changeOnBtnMute_();
								break;
							}
							case DLG_EDIT_BTN_INFO:
							case IDM_DIALOG_SHOW_MIDI_KEYS: {
								(void) ClassStorage::Get().OpenDialog<DialogEditInfo>(h, false);
								break;
							}
							#pragma endregion

							#pragma region IDM_LV_*
							case IDM_LV_NEW:
							case IDM_LV_COPY:
							case IDM_LV_PASTE:
							case IDM_LV_DELETE:
							case IDM_LV_SET_MQTT:
							case IDM_LV_SET_MMKEY:
							case IDM_LV_SET_MIXER:
							case IDM_LV_SET_LIGHTKEY8B:
							case IDM_LV_SET_LIGHTKEY16B: {
								lv_menu_(LOWORD(w));
								break;
							}
							case IDM_LV_EDIT_MODE: {
								tb_edit_digitmode_();
								break;
							}
							case IDM_DIALOG_SAVE: {
								tb_save_();
								break;
							}
							case IDM_DIALOG_IMPORT: {
								tb_import_();
								break;
							}
							case IDM_DIALOG_EXPORT: {
								tb_export_();
								break;
							}
							case IDM_DIALOG_LAST_OPEN: {
								tb_recent_open_(HIWORD(w));
								break;
							}
							case IDM_LV_READ_MIDI_CODE: {
								tb_monitor_();
								break;
							}
							case IDM_LV_EXT_MODE: {
								tb_setmode_();
								break;
							}
							case IDM_LV_FILTER_EMBED: {
								tb_filter_embed_();
								break;
							}
							case IDM_LV_PASTE_NOTIFY: {
								tb_edit_notify_(static_cast<Common::MIDIMT::EditorNotify>(HIWORD(w)));
								break;
							}
							case IDM_LV_SORTUP_KEY:
							case IDM_LV_SORTUP_SCENE:
							case IDM_LV_SORTUP_TARGET:
							case IDM_LV_SORTUP_TARGETLONG:
							case IDM_LV_SORTDOWN_KEY:
							case IDM_LV_SORTDOWN_SCENE:
							case IDM_LV_SORTDOWN_TARGET:
							case IDM_LV_SORTDOWN_TARGETLONG: {
								tb_sort_(LOWORD(w));
								break;
							}
							case IDM_LV_FILTER_ON:
							case IDM_LV_FILTER_OFF:
							case IDM_LV_FILTER_SET:
							case IDM_LV_FILTER_MQTT:
							case IDM_LV_FILTER_MMKEY:
							case IDM_LV_FILTER_MIXER:
							case IDM_LV_FILTER_CLEAR: {
								tb_filters_(LOWORD(w));
								break;
							}
							case IDM_LV_FILTER_TYPEOR:
							case IDM_LV_FILTER_TYPEAND: {
								tb_filter_type_(LOWORD(w));
								break;
							}
							case IDM_LV_FILTER_AUTOCOMMIT: {
								tb_filter_auto_commit_();
								break;
							}
							#pragma endregion

							case DLG_EXIT:
							case IDCANCEL: {
								IO::IOBridge::Get().UnSetCb(*static_cast<CbEvent*>(this));
								CbEvent::Init(-1);
								clear_();
								return static_cast<INT_PTR>(1);
							}
							default: break;
						}
						break;
					}
					default: break;
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return ::DefSubclassProc(h, m, w, l);
		}
		#pragma endregion

	}
}