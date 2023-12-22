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

namespace Common {
	namespace MIDIMT {

		using namespace std::placeholders;

		static const uint16_t elements_[] {
			DLG_START_OPEN_CONFIG,
			DLG_START_PLUGINS_LIST,
			DLG_START_PLUGINS_RELOAD,
			DLG_START_MIXER_ENABLE,
			DLG_START_MIXER_DUPLICATE,
			DLG_START_MIXER_OLD_VALUE,
			DLG_START_MIXER_FAST_VALUE
		};
		static const uint16_t elements_all_[] {
			DLG_START_OPEN_CONFIG,
			DLG_START_AUTOBOOT_SYS,
			DLG_START_AUTORUN_SYS,
			DLG_START_AUTORUN_CONFIG,
			DLG_START_WRITE_FILELOG,
			DLG_START_MIXER_RIGHT_CLICK,
			DLG_START_MIXER_ENABLE,
			DLG_START_MIXER_FAST_VALUE,
			DLG_START_MIXER_DUPLICATE,
			DLG_START_MIXER_OLD_VALUE,
			DLG_START_PLUGINS_RELOAD,
			DLG_START_PLUGINS_LIST,
			DLG_GO_START,
			DLG_SAVE,
			DLG_GO_ABOUT,
			DLG_GO_UPDATE
		};

		static std::wstring state_bool__(const bool b) {
			LangInterface& lang = LangInterface::Get();
			return b ? lang.GetString(STRING_LANG_YES) : lang.GetString(STRING_LANG_NO);
		}

		DialogStart::DialogStart()
			: open_plugin_(IO::IOBridge::Get().GetEmptyPlugin()), index_plugin_(-1) {
			CbEvent::GetHwndCb = [=]() { return hwnd_.get(); };

			uint16_t status_icos[]{ ICON_PLUGIN_MODULES, ICON_PLUGIN_STARTED };
			img_status_.Init(
				status_icos,
				std::size(status_icos),
				[](uint16_t n) -> HICON {
					return LangInterface::Get().GetIcon256x256(MAKEINTRESOURCEW(n));
				},
				true);
		}
		DialogStart::~DialogStart() {
			dispose_();
			img_status_.Release();
		}

		void DialogStart::dispose_() {
			try {
				if (open_plugin_ && !open_plugin_.get()->empty())
					open_plugin_->GetPluginUi().CloseDialog();

				isload_ = false;

				hwnd_.reset();
				img_status_.Reset();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			isload_ = false;
		}
		void DialogStart::init_() {
			try {
				auto& cnf = common_config::Get();
				bool isstart = IO::IOBridge::Get().IsStarted(),
					 isconfig = false;

				if (cnf.IsNewConfig() || cnf.GetConfig()->empty()) {
					auto ft = std::async(std::launch::async, [=]() ->bool {
						return common_config::Get().Load();
					});
					try {
						isconfig = ft.get();
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
				} else isconfig = !cnf.GetConfig()->empty();
				if (!isconfig)
					to_log::Get() << log_string().to_log_string(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_JSONCONF_EMPTY)
					);

				::CheckDlgButton(hwnd_, DLG_START_AUTOBOOT_SYS, CHECKBTN(cnf.Registry.GetSysAutoBoot()));
				::CheckDlgButton(hwnd_, DLG_START_AUTORUN_SYS,  CHECKBTN(cnf.Registry.GetAutoRun()));

				::CheckDlgButton(hwnd_,  DLG_START_MIXER_ENABLE, CHECKBTN(cnf.Registry.GetMixerEnable()));
				::CheckDlgButton(hwnd_,  DLG_START_MIXER_RIGHT_CLICK, CHECKBTN(cnf.Registry.GetMixerRightClick()));
				::CheckDlgButton(hwnd_,  DLG_START_MIXER_FAST_VALUE, CHECKBTN(cnf.Registry.GetMixerFastValue()));
				::CheckDlgButton(hwnd_,  DLG_START_MIXER_OLD_VALUE, CHECKBTN(cnf.Registry.GetMixerSetOldLevelValue()));
				::CheckDlgButton(hwnd_,  DLG_START_MIXER_DUPLICATE, CHECKBTN(cnf.Registry.GetMixerDupAppRemove()));

				::CheckDlgButton(hwnd_,  DLG_START_WRITE_FILELOG, CHECKBTN(to_log::Get().filelog()));
				::EnableWindow(::GetDlgItem(hwnd_, DLG_SAVE), false);

				changeConfigView_(cnf.GetConfig());
				changeStateActions_(isconfig && !isstart);

				build_LangCombobox_();
				build_PluginListView_(true);

				img_status_.Init(hwnd_, DLG_START_PLACE_PLUGIN);

				::EnableWindow(::GetDlgItem(hwnd_, DLG_SAVE), false);
				::SetFocus(::GetDlgItem(hwnd_, DLG_START_CTRLCOUNT));

				CbEvent::Init(DLG_EVENT_LOG, DLG_EVENT_MONITOR);
				IO::IOBridge::Get().SetCb(*static_cast<CbEvent*>(this));

				isload_ = true;

				CbEvent::AddToLog((log_string() << VER_GUI_EN << L" | " << VER_COPYRIGHT).str());
				{
					std::wstring ws = MIDI::MidiDevices::Get().GetVirtualDriverVersion();
					if (ws.empty())
						CbEvent::AddToLog(log_string().to_log_string(__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_MIDI_EMPTY_DRIVER)
							).str()
						);
					else
						CbEvent::AddToLog(log_string().to_log_format(__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_MIDIMT_VDRV_VERSION),
							ws).str()
						);
				}
				///
				{
					LangInterface& lang = LangInterface::Get();

					if (!cnf.Registry.GetAutoRun())
						CbEvent::AddToLog(log_string().to_log_string(__FUNCTIONW__,
							lang.GetString(STRING_MAIN_MSG3)).str()
						);
					if (!cnf.GetConfig()->auto_start)
						CbEvent::AddToLog(log_string().to_log_string(__FUNCTIONW__,
							lang.GetString(STRING_MAIN_MSG4)).str()
						);
				}

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::start_() {
			try {
				if (!hwnd_) return;
				IO::IOBridge& br = IO::IOBridge::Get();
				if (!br.IsLoaded()) {
					to_log::Get() << log_string().to_log_string(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDIMT_NOPLUGINS)
					);
					return;
				}
				if (br.IsStarted()) return;

				if (open_plugin_ && !open_plugin_.get()->empty())
					open_plugin_->GetPluginUi().CloseAnimateDialog();

				const bool b = ClassStorage::Get().StartAsync();
				changeStateActions_(!b);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::stop_() {
			try {
				if (!hwnd_ || IO::IOBridge::Get().IsStoped()) return;

				const bool b = ClassStorage::Get().StopAsync();
				changeStateActions_(b);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		#pragma region Builds events
		void DialogStart::build_LangCombobox_() {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_START_LANG_COMBO))) return;

				(void) ComboBox_ResetContent(hi);

				HMENU hm{ nullptr }, hmp{ nullptr };
				if ((hm = ::GetMenu(hwnd_)))
					hmp = ::GetSubMenu(hm, 4);

				LangInterface& lang = LangInterface::Get();
				std::forward_list<std::wstring> list = lang.GetLanguages();

				uint32_t mcnt = ((hmp) ? ::GetMenuItemCount(hmp) : 10U),
						 i = DLG_LANG_MENU_0;

				for (auto& name : list) {
					(void) ComboBox_AddString(hi, name.c_str());
					if ((mcnt <= 1) && (hmp)) {
						if (i == DLG_LANG_MENU_0) i++;
						else (void) ::AppendMenuW(hmp, MF_STRING | MF_ENABLED, i++, (LPCWSTR)name.c_str());
					}
				}
				std::tuple<int32_t, std::wstring> t = lang.SelectedLanguage();
				(void) ComboBox_SelectString(hi, 0, std::get<1>(t).c_str());
				if (hmp) (void) ::CheckMenuItem(hmp, (DLG_LANG_MENU_0 + std::get<0>(t)), MF_CHECKED | MF_BYCOMMAND);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::build_PluginListView_(bool isinit) {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_START_PLUGINS_LIST))) return;

				if (isinit) {
					(void)ListView_SetExtendedListViewStyle(hi,
						ListView_GetExtendedListViewStyle(hi)
						| LVS_EX_FULLROWSELECT
						| LVS_EX_AUTOSIZECOLUMNS
						| LVS_EX_CHECKBOXES
						| LVS_EX_SINGLEROW
						| LVS_EX_DOUBLEBUFFER
						| LVS_EX_FLATSB
					);
					ListView_SetView(hi, LV_VIEW_TILE);

					constexpr int width__ = 220;

					LVCOLUMNW lvc{};
					lvc.mask = LVCF_FMT | LVCF_WIDTH;
					lvc.fmt = LVCFMT_LEFT;
					lvc.cx = width__;
					for (int32_t i = 0; i < 3; i++)
						ListView_InsertColumn(hi, i, &lvc);

					LVTILEVIEWINFO lvi{};
					lvi.cbSize = sizeof(LVTILEVIEWINFO);
					lvi.dwMask = LVTVIM_COLUMNS | LVTVIM_TILESIZE | LVTVIM_LABELMARGIN;
					lvi.dwFlags = LVTVIF_FIXEDWIDTH;
					lvi.sizeTile.cx = width__;
					lvi.sizeTile.cy = 0;
					lvi.cLines = 3;
					lvi.rcLabelMargin = RECT(2, 0, 2, 0);
					ListView_SetTileViewInfo(hi, &lvi);

				} else (void)ListView_DeleteAllItems(hi);

				UINT cols[3] = { 1U, 2U, 3U };
				IO::IOBridge& br = IO::IOBridge::Get();
				for (uint16_t i = 0; i < br.PluginCount(); i++) {
					try {
						IO::plugin_t& p = br[i];
						if (p->empty()) continue;

						IO::PluginInfo& pi = p.get()->GetPluginInfo();

						LVITEMW lvi{};
						lvi.mask = LVIF_COLUMNS | LVIF_TEXT | LVIF_STATE;
						lvi.pszText = (LPWSTR)pi.Name().c_str();
						lvi.cColumns = 0;

						int32_t idx;
						if ((idx = ListView_InsertItem(hi, &lvi)) == -1) break;
						if (p->enabled() && p->configure()) ListView_SetCheckState(hi, idx, true);

						std::wstring state = std::vformat(
							std::wstring_view(common_error_code::Get().get_error(common_error_id::err_PLUGIN_LIST_INFO)),
							std::make_wformat_args(
								state_bool__(p->enabled()),
								state_bool__(p->configure()),
								state_bool__(p->started())
							)
						);

						ListView_SetItemText(hi, i, 0, (LPWSTR)pi.Name().c_str());
						ListView_SetItemText(hi, i, 1, (LPWSTR)state.c_str());
						ListView_SetItemText(hi, i, 2, (LPWSTR)pi.Desc().c_str());

						LVTILEINFO lvt{};
						lvt.cbSize = sizeof(LVTILEINFO);
						lvt.cColumns = 3;
						lvt.iItem = i;
						lvt.puColumns = cols;
						ListView_SetTileInfo(hi, &lvt);

					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region Change On events
		void DialogStart::event_Log_(CbEventData* data) {
			try {
				if (!data) return;
				CbEventDataDeleter d = data->GetDeleter();
				if (!hwnd_) return;
				CbEvent::ToLog(::GetDlgItem(hwnd_, DLG_START_LOG), d.GetData(), false);
			} catch (...) {}
		}
		void DialogStart::event_Monitor_(CbEventData* data) {
			try {
				if (!data) return;
				CbEventDataDeleter d = data->GetDeleter();
				if (!hwnd_) return;
				CbEvent::ToMonitor(::GetDlgItem(hwnd_, DLG_START_LOG), d.GetData(), false);
			} catch (...) {}
		}
		void DialogStart::event_DragAndDrop_(std::wstring s) {
			try {
				if (s.empty() || !hwnd_) return;

				common_config& cnf = common_config::Get();

				if (!cnf.Load(s)) {
					to_log::Get() << log_string().to_log_string(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDIMT_CONFEDIT)
					);
					return;
				}
				cnf.Registry.SetConfPath(s);
				changeConfigView_(cnf.GetConfig());
				::EnableWindow(::GetDlgItem(hwnd_, DLG_SAVE), true);

				to_log::Get() << log_string().to_log_format(
					__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_MIDIMT_CONFFROMFILE),
					s.c_str()
				);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::event_PluginsReload_() {
			try {
				if (!hwnd_) return;

				if (open_plugin_ && !open_plugin_.get()->empty()) {
					open_plugin_->GetPluginUi().CloseDialog();
					open_plugin_ = IO::IOBridge::Get().GetEmptyPlugin();
				}

				auto f = std::async(std::launch::async, [=]() -> bool {
					try {
						IO::IOBridge& br = IO::IOBridge::Get();
						if (br.IsStarted()) br.Stop();
						return br.Reload() && br.IsLoaded();
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
					return false;
				});
				const bool b = f.get();
				if (!b)
					to_log::Get() << log_string().to_log_string(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDIMT_NOPLUGINS)
					);

				build_PluginListView_(false);

				changeStateActions_(true);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::event_LanguageChange_(uint16_t idx) {
			try {

				IO::PluginUi::hinst_.reset(LangInterface::Get().GetLangHinstance());

				if (HMENU hm; (hm = ::GetMenu(hwnd_))) {
					uint32_t cnt = ::GetMenuItemCount(hm);
					for (uint32_t i = 0; i < cnt; i++) {
						switch (i) {
							case 4:
							case 6: break;
							default: {
								(void) ::EnableMenuItem(hm, i, MF_GRAYED | MF_BYPOSITION);
								break;
							}
						}
						if (HMENU hmp; (hmp = ::GetSubMenu(hm, 4))) {
							cnt = ::GetMenuItemCount(hmp);
							for (uint32_t i = 0; i < cnt; i++)
								(void) ::CheckMenuItem(hmp, i, ((i == idx) ? MF_CHECKED : MF_UNCHECKED) | MF_BYPOSITION);
						}
					}
					::SetMenu(hwnd_, hm);
				}
				for (uint16_t id : elements_all_)
					(void) ::EnableWindow(::GetDlgItem(hwnd_, id), false);

				if (open_plugin_ && !open_plugin_.get()->empty())
					open_plugin_->GetPluginUi().CloseAnimateDialog();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogStart::changeStateActions_(bool b) {
			try {
				if (!hwnd_) return;
				if (HWND hi; (hi = ::GetDlgItem(hwnd_, DLG_GO_START)))
					::EnableWindow(hi, b);

				HMENU hm;
				if (!(hm = ::GetMenu(hwnd_))) return;
				(void) ::EnableMenuItem(hm, DLG_GO_START, (b  ? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);
				(void) ::EnableMenuItem(hm, DLG_GO_STOP,  (!b ? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);
				(void) ::EnableMenuItem(hm, DLG_SAVE,     (b  ? MF_ENABLED : MF_GRAYED) | MF_BYCOMMAND);

				/* reresh dialog menu hack */
				::SetMenu(hwnd_, hm);

				for (uint16_t id : elements_)
					(void) ::EnableWindow(::GetDlgItem(hwnd_, id), b);
				(void) ::CheckRadioButton(hwnd_, DLG_START_RADIO1, DLG_START_RADIO2, CHECKRADIO(b, DLG_START_RADIO2, DLG_START_RADIO1));

				img_status_.SetAnimateStatus(b ? 0U : 1U);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::changeMovePlugin_() {
			try {
				if (!hwnd_ || !open_plugin_ || open_plugin_.get()->empty()) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_START_PLACE_PLUGIN))) return;

				RECT r{};
				if (::GetWindowRect(hi, &r))
					open_plugin_->GetPluginUi().ChangeDialogPosition(r);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::changeConfigView_(std::shared_ptr<JSON::MMTConfig>& mmt) {
			::CheckDlgButton(hwnd_,  DLG_START_AUTORUN_CONFIG,	CHECKBTN(mmt->auto_start));
			::CheckDlgButton(hwnd_,  DLG_START_ISCONFIG,		CHECKBTN(!mmt->empty()));
			::CheckDlgButton(hwnd_,  DLG_START_ISUNITS,			CHECKBTN(!mmt->units.empty()));
			::SetDlgItemTextW(hwnd_, DLG_START_CTRLCOUNT,		std::to_wstring(mmt->units.size()).c_str());
			::SetDlgItemTextW(hwnd_, DLG_START_CONFIG_NAME,		mmt->config.c_str());
		}
		void DialogStart::changeOnLang_() {
			try {
				if (!hwnd_) return;

				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_START_LANG_COMBO))) return;

				int32_t val = static_cast<int32_t>(::SendMessageW(hi, CB_GETCURSEL, 0, 0));
				if (val == CB_ERR) return;

				wchar_t buf[MAX_PATH]{};
				if ((::SendMessageW(hi, CB_GETLBTEXT, val, (LPARAM)&buf) == CB_ERR) || (buf[0] == L'\0')) return;

				auto& lang = LangInterface::Get();
				if (!lang.SelectLanguage(buf)) return;

				std::tuple<int32_t, std::wstring> t = lang.SelectedLanguage();

				to_log::Get() << log_string().to_log_format(
					__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_MIDIMT_LANG_ACTIVE),
					std::get<1>(t)
				);

				event_LanguageChange_(std::get<0>(t));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::changeOnLang_(uint16_t idx) {
			try {
				if (!hwnd_) return;

				auto& lang = LangInterface::Get();
				if (!lang.SelectLanguage(idx - DLG_LANG_MENU_0)) return;
				
				std::tuple<int32_t, std::wstring> t = lang.SelectedLanguage();

				to_log::Get() << log_string().to_log_format(
					__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_MIDIMT_LANG_ACTIVE),
					std::get<1>(t)
				);

				if (HWND hi; (hi = ::GetDlgItem(hwnd_, DLG_START_LANG_COMBO)))
					(void) ComboBox_SelectString(hi, 0, std::get<1>(t).c_str());

				event_LanguageChange_(std::get<0>(t));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::changeOnConfigFileOpen_() {
			try {
				if (!hwnd_) return;

				PWSTR pws = nullptr;
				IShellItem* item = nullptr;
				IFileOpenDialog* ptr = nullptr;

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

						common_config& cnf = common_config::Get();
						std::wstring s = Utils::to_string(pws);
						if (cnf.Load(s)) {
							auto& cnf = common_config::Get();
							auto& mmt = cnf.GetConfig();

							cnf.RecentConfig.Add(s);
							changeConfigView_(mmt);
							::EnableWindow(::GetDlgItem(hwnd_, DLG_SAVE), true);
						}
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
		void DialogStart::changeOnSaveConfig_() {
			try {
				if (!hwnd_) return;
				::EnableWindow(::GetDlgItem(hwnd_, DLG_SAVE), !common_config::Get().Save());
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogStart::changeOnListViewClick_() {
			try {
				if (!hwnd_ || !isload_) return;

				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_START_PLUGINS_LIST))) return;
				if (!ListView_GetItemCount(hi)) return;

				int32_t idx = ListView_GetNextItem(hi, -1, LVNI_SELECTED);
				if (idx == -1) return;
				if (index_plugin_ == idx) return;

				if (open_plugin_ && !open_plugin_.get()->empty())
					open_plugin_->GetPluginUi().CloseDialog();

				IO::IOBridge& br = IO::IOBridge::Get();
				IO::plugin_t& p = br[idx];

				if (!p.get()->empty()) {

					open_plugin_ = p;
					index_plugin_ = idx;

					HWND hp{ nullptr }, h = ::GetDlgItem(hwnd_, DLG_START_PLACE_PLUGIN);
					uint32_t id = p->GetPluginInfo().DialogId();
					if (id)  hp = p->GetPluginUi().BuildDialog(hinst_, h, MAKEINTRESOURCEW(id));
					else	 hp = p->GetPluginUi().BuildDialog(h);

					if (!hp) return;
					(void) ::SetWindowPos(hp, HWND_TOP, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

					::SetDlgItemTextW(hwnd_, DLG_START_PLUGINS_DESC, p->GetPluginInfo().Desc().c_str());
					::EnableWindow(::GetDlgItem(hwnd_, DLG_SAVE), true);

					(void) ::AnimateWindow(
						hp,
						160,
						(AW_ACTIVATE | AW_BLEND)
					);

				} else {
					open_plugin_ = br.GetEmptyPlugin();
					index_plugin_ = 0;
					::SetDlgItemTextW(hwnd_, DLG_START_PLUGINS_DESC, L"");
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		IO::PluginUi* DialogStart::GetUi() {
			return static_cast<IO::PluginUi*>(this);
		}

		const bool DialogStart::IsRunOnce() {
			return !hwnd_ && !isload_;
		}
		void DialogStart::SetFocus() {
			if (hwnd_) (void) ::SetFocus(hwnd_);
		}

		#pragma region Override
		HWND DialogStart::BuildDialog(HWND h) {
			hinst_.reset(LangInterface::Get().GetLangHinstance());
			return IO::PluginUi::BuildDialog(hinst_, h, MAKEINTRESOURCEW(DLG_START_WINDOW));
		}
		LRESULT DialogStart::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
			try {
				switch (m) {
					case WM_INITDIALOG: {
						hwnd_.reset(h, static_cast<SUBCLASSPROC>(&PluginUi::DialogProc_), reinterpret_cast<DWORD_PTR>(this), 0);
						init_();
						::ShowWindow(h, SW_SHOW);
						::SetFocus(::GetDlgItem(h, IDCANCEL));
						return static_cast<INT_PTR>(1);
					}
					case WM_NOTIFY: {
						if (!isload_ || (!l)) break;

						LPNMHDR lpmh = (LPNMHDR)l;
						switch (lpmh->idFrom) {
							case (UINT)DLG_START_PLUGINS_LIST: {
								#pragma warning( push )
								#pragma warning( disable : 26454 )
								if (lpmh->code == static_cast<UINT>(LVN_ITEMCHANGED))
									changeOnListViewClick_();
								#pragma warning( pop )
								break;
							}
							default: break;
						}
						break;
					}
					case WM_DROPFILES: {
						if (!w) break;
						event_DragAndDrop_(UI::UiUtils::GetDragAndDrop(reinterpret_cast<HDROP>(w)));
						return static_cast<INT_PTR>(1);
					}
					case WM_HELP: {
						if (!l) break;
						UI::UiUtils::ShowHelpPage(DLG_START_WINDOW, reinterpret_cast<HELPINFO*>(l));
						return static_cast<INT_PTR>(1);
					}
					case WM_MOVE: {
						changeMovePlugin_();
						break;
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
							case DLG_SAVE: {
								changeOnSaveConfig_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_GO_HELP: {
								UI::UiUtils::ShowHelpPage(DLG_START_WINDOW, static_cast<uint16_t>(0U));
								return static_cast<INT_PTR>(1);
							}
							case DLG_GO_START: {
								start_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_GO_STOP: {
								stop_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_GO_ABOUT: {
								ClassStorage::Get().OpenDialog<DialogAbout>(h, false);
								return static_cast<INT_PTR>(1);
							}
							case DLG_GO_UPDATE: {
								::ShellExecuteW(0, 0, LangInterface::Get().GetString(STRING_URL_GIT).c_str(), 0, 0, SW_SHOW);
								return static_cast<INT_PTR>(1);
							}
							case DLG_GO_LOG: {
								ClassStorage::Get().OpenDialog<DialogLogView>(nullptr, false);
								return static_cast<INT_PTR>(1);
							}
							case DLG_START_OPEN_CONFIG: {
								changeOnConfigFileOpen_();
								break;
							}
							case DLG_START_LANG_COMBO: {
								if (HIWORD(w) == CBN_SELENDOK) /* CBN_SELCHANGE */
									changeOnLang_();
								break;
							}
							case DLG_START_AUTOBOOT_SYS: {
								common_config::Get().Registry.SetSysAutoBoot(
									UI::UiUtils::GetControlChecked(hwnd_, DLG_START_AUTOBOOT_SYS));
								break;
							}
							case DLG_START_AUTORUN_SYS: {
								common_config::Get().Registry.SetAutoRun(
									UI::UiUtils::GetControlChecked(hwnd_, DLG_START_AUTORUN_SYS));
								break;
							}
							case DLG_START_AUTORUN_CONFIG: {
								common_config::Get().GetConfig()->auto_start =
									UI::UiUtils::GetControlChecked(hwnd_, DLG_START_AUTORUN_CONFIG);
								break;
							}
							case DLG_START_WRITE_FILELOG: {
								common_config::Get().Registry.SetLogWrite(
									UI::UiUtils::GetControlChecked(hwnd_, DLG_START_WRITE_FILELOG));
								break;
							}
							case DLG_START_MIXER_ENABLE: {
								common_config::Get().Registry.SetMixerEnable(
									UI::UiUtils::GetControlChecked(hwnd_, DLG_START_MIXER_ENABLE));
								break;
							}
							case DLG_START_MIXER_FAST_VALUE: {
								common_config::Get().Registry.SetMixerFastValue(
									UI::UiUtils::GetControlChecked(hwnd_, DLG_START_MIXER_FAST_VALUE));
								break;
							}
							case DLG_START_MIXER_RIGHT_CLICK: {
								common_config::Get().Registry.SetMixerRightClick(
									UI::UiUtils::GetControlChecked(hwnd_, DLG_START_MIXER_RIGHT_CLICK));
								break;
							}
							case DLG_START_MIXER_DUPLICATE: {
								common_config::Get().Registry.SetMixerDupAppRemove(
									UI::UiUtils::GetControlChecked(hwnd_, DLG_START_MIXER_DUPLICATE));
								break;
							}
							case DLG_START_MIXER_OLD_VALUE: {
								common_config::Get().Registry.SetMixerSetOldLevelValue(
									UI::UiUtils::GetControlChecked(hwnd_, DLG_START_MIXER_OLD_VALUE));
								break;
							}
							case DLG_START_PLUGINS_RELOAD: {
								event_PluginsReload_();
								break;
							}
							case DLG_EXIT:
							case IDCANCEL: {
								IO::IOBridge::Get().UnSetCb(*static_cast<CbEvent*>(this));
								CbEvent::Init(-1);
								dispose_();
								return static_cast<INT_PTR>(1);
							}
							default: {
								if ((c >= DLG_LANG_MENU_0) && (c <= DLG_LANG_MENU_40))
									changeOnLang_(c);
								break;
							}
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
