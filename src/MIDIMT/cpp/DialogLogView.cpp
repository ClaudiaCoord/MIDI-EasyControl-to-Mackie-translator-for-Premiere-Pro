/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"

namespace Common {
	namespace MIDIMT {

		DialogLogView::DialogLogView() {
			CbEvent::GetHwndCb = [=]() { return hwed_.get(); };
		}

		void DialogLogView::dispose_() {
			try {
				IO::IOBridge::Get().UnSetCb(*static_cast<CbEvent*>(this));
				CbEvent::Init(-1);

				editor_.reset();
				hwed_.reset();
				hwnd_.reset();

			} catch(...) {}
			isload_.store(false);
		}
		void DialogLogView::init_() {
			try {
				editor_ = std::make_unique<UI::ScintillaBox>();

				RECT r{};
				::GetClientRect(hwnd_, &r);

				HINSTANCE hi = LangInterface::Get().GetMainHinstance();
				hwed_.reset(
					::CreateWindowExW(0,
						L"Scintilla", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN,
						r.left, r.top, r.right - r.left, r.bottom - r.top,
						hwnd_, 0, hi, 0),
					&DialogLogView::event_edit_,
					reinterpret_cast<DWORD_PTR>(this), 1U
				);

				if (!hwed_) return;

				std::wstring path{};
				{
					std::filesystem::path p = std::filesystem::path(Utils::app_dir(LangInterface::Get().GetMainHinstance())).parent_path();
					if (!p.empty()) {
						p.append(to_log::Get().logname());
						if (std::filesystem::exists(p))
							path = p.wstring();
					}
				}

				editor_->init(hwed_, path);
				build_MenuPluginList_();

				isload_.store(true);

				CbEvent::Init(DLG_EVENT_LOG, DLG_EVENT_MONITOR);
				IO::IOBridge::Get().SetCb(*static_cast<CbEvent*>(this));

				if (IO::IOBridge::Get().IsStoped())
					editor_->set(LangInterface::Get().GetString(STRING_LOGV_MSG1));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogLogView::build_MenuPluginList_() {
			try {
				if (!hwnd_) return;

				HMENU hm, hmp;
				if (!(hm = ::GetMenu(hwnd_)) || !(hmp = ::GetSubMenu(hm, 7))) return;

				IO::IOBridge& br = IO::IOBridge::Get();
				for (uint16_t i = 0, n = 0; i < br.PluginCount(); i++) {
					try {
						IO::plugin_t& p = br[i];
						if (p->empty()) continue;

						IO::PluginInfo& pi = p.get()->GetPluginInfo();
						uint16_t cmd = (DLG_PLUGSTAT_MENU_0 + n++);

						(void) ::AppendMenuW(hmp, MF_STRING | MF_ENABLED, cmd, (LPCWSTR)pi.Name().c_str());
						if (!p->started())
							(void) ::EnableMenuItem(hmp, cmd, MF_GRAYED | MF_BYCOMMAND);

					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogLogView::zoomin_() {
			if (editor_) editor_->zoomin();
		}
		void DialogLogView::zoomout_() {
			if (editor_) editor_->zoomout();
		}
		void DialogLogView::gostart_() {
			if (editor_) editor_->gostart();
		}
		void DialogLogView::goend_() {
			if (editor_) editor_->goend();
		}
		void DialogLogView::clear_() {
			if (editor_) editor_->clear();
		}

		IO::PluginUi* DialogLogView::GetUi() {
			return static_cast<IO::PluginUi*>(this);
		}

		const bool DialogLogView::IsRunOnce() {
			return !hwnd_ && !isload_;
		}
		void DialogLogView::SetFocus() {
			if (hwnd_) (void) ::SetFocus(hwnd_);
		}

		#pragma region Change On events
		void DialogLogView::event_Log_(CbEventData* data) {
			event_Text_(data, CbHWNDType::TYPE_CB_LOG);
		}
		void DialogLogView::event_Monitor_(CbEventData* data) {
			event_Text_(data, CbHWNDType::TYPE_CB_MON);
		}
		void DialogLogView::event_Text_(CbEventData* data, CbHWNDType t) {
			try {
				if (!data) return;
				CbEventDataDeleter d = data->GetDeleter();
				if (!hwnd_ || !editor_ || (d.GetData()->GetType() != t)) return;
				log_string ls{};

				switch (t) {
					using enum MIDIMT::CbHWNDType;
					case TYPE_CB_LOG: {
						std::wstring ws = CbEvent::ToLog(d.GetData());
						if (ws.empty()) return;
						ls << ws.c_str() << L"\n";
						break;
					}
					case TYPE_CB_MON: {
						std::wstring ws = CbEvent::ToMonitor(d.GetData());
						if (ws.empty()) return;
						ls << L"-\t" << LangInterface::Get().GetString(STRING_LOGV_MSG9) << L"\t\t" << ws.c_str() << L"\n";
						break;
					}
					default: return;
				}
				
				editor_->set(ls.str());
			} catch (...) {}
		}
		void DialogLogView::event_Config_(const std::wstring& title, const std::wstring& s) {
			try {
				if (s.empty() || !hwed_ || !editor_) return;
				editor_->append(title);
				editor_->append(s);
			} catch (...) {}
		}
		void DialogLogView::event_Config_Show_(const std::wstring& t, const std::wstring& s) {
			try {
				log_string ls{};
				ls << L"\n[" << t.c_str() << L"]\n";
				event_Config_(ls.str(), s);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogLogView::event_Scint_(LPNMHDR hdr) {
			try {
				if (!hdr || !hwed_ || !editor_ || (hdr->hwndFrom != hwed_)) return;
				editor_->sc_event(hdr);
			} catch (...) {}
		}
		void DialogLogView::event_Stat_(uint16_t idx) {
			try {
				if (!hwed_ || !editor_ || (idx < DLG_PLUGSTAT_MENU_0)) return;

				IO::IOBridge& br = IO::IOBridge::Get();
				IO::plugin_t& p = br[(idx - DLG_PLUGSTAT_MENU_0)];
				if (!p || p->empty()) return;
				event_Stat_(p);

			} catch (...) {}
		}
		void DialogLogView::event_Stat_(IO::plugin_t& p) {
			try {
				if (!p || p->empty()) return;

				IO::PluginInfo& pi = p.get()->GetPluginInfo();
				log_string ls{};
				ls << L"\n[" << pi.Name().c_str() << L"]\n";

				auto& list = p.get()->GetDeviceList();
				if (list.empty())
					ls << L"\t\t" << LangInterface::Get().GetString(IDS_ERRORID_EMPTY) << L"\n";
				else {
					uint16_t i = 1;
					for (auto& a : list)
						ls << L"\t" << i++ << L") [" << a.first << L"] - " << a.second.c_str() << L"\n";
				}
				editor_->append(ls.str());

			} catch (...) {}
		}
		void DialogLogView::event_Stats_() {
			try {
				if (!hwed_ || !editor_) return;

				GUID guid{ GUID_NULL };
				IO::IOBridge& br = IO::IOBridge::Get();

				for (uint16_t i = DLG_PLUGSTAT_MENU_0; i <= DLG_PLUGSTAT_MENU_9; i++) {
					IO::plugin_t& p = br[(i - DLG_PLUGSTAT_MENU_0)];
					if (!p || p->empty()) continue;

					const GUID& pguid = p.get()->GetPluginInfo().Guid();
					if (guid == pguid) continue;
					guid = pguid;
					event_Stat_(p);
				}
			} catch (...) {}
		}
		#pragma endregion

		#pragma region Override
		HWND DialogLogView::BuildDialog(HWND h) {
			hinst_.reset(LangInterface::Get().GetLangHinstance());
			return IO::PluginUi::BuildDialog(hinst_, h, MAKEINTRESOURCEW(DLG_LOGVIEW_WINDOW));
		}
		LRESULT DialogLogView::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
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
						NMHDR* hdr = reinterpret_cast<LPNMHDR>(l);
						event_Scint_(hdr);
						break;
					}
					case WM_HELP: {
						if (!l) break;
						UI::UiUtils::ShowHelpPage(LangInterface::Get().GetHelpLangId(), DLG_LOGVIEW_WINDOW, reinterpret_cast<HELPINFO*>(l));
						return static_cast<INT_PTR>(1);
					}
					case WM_COMMAND: {
						if (!isload_) break;
						uint16_t c{ LOWORD(w) };
						switch (c) {
							case DLG_LOGVIEW_MENU_ZOOMIN: {
								zoomin_();
								break;
							}
							case DLG_LOGVIEW_MENU_ZOOMOUT: {
								zoomout_();
								break;
							}
							case DLG_LOGVIEW_MENU_BEGIN: {
								gostart_();
								break;
							}
							case DLG_LOGVIEW_MENU_END: {
								goend_();
								break;
							}
							case DLG_LOGVIEW_MENU_CLEAR: {
								clear_();
								break;
							}
							case DLG_LOGVIEW_MENU_CONF_ALL: {
								auto& conf = common_config::Get().GetConfig();
								event_Config_Show_(
									std::vformat(
										std::wstring_view(LangInterface::Get().GetString(STRING_LOGV_MSG2)),
										std::make_wformat_args(
											conf->config,
											conf->builder
										)
									),
									conf->dump()
								);
								break;
							}
							case DLG_LOGVIEW_MENU_CONF_MIDI: {
								auto& conf = common_config::Get().GetConfig();
								event_Config_Show_(LangInterface::Get().GetString(STRING_LOGV_MSG3), conf->midiconf.dump());
								break;
							}
							case DLG_LOGVIEW_MENU_CONF_MQTT: {
								auto& conf = common_config::Get().GetConfig();
								event_Config_Show_(LangInterface::Get().GetString(STRING_LOGV_MSG4), conf->mqttconf.dump());
								break;
							}
							case DLG_LOGVIEW_MENU_CONF_MMKEYS: {
								auto& conf = common_config::Get().GetConfig();
								event_Config_Show_(LangInterface::Get().GetString(STRING_LOGV_MSG5), conf->mmkeyconf.dump());
								break;
							}
							case DLG_LOGVIEW_MENU_CONF_LIGTS: {
								auto& conf = common_config::Get().GetConfig();
								event_Config_Show_(
									std::vformat(
										std::wstring_view(LangInterface::Get().GetString(STRING_LOGV_MSG6)),
										std::make_wformat_args(L"LIGTS")
									),
									conf->lightconf.dump()
								);
								break;
							}
							case DLG_LOGVIEW_MENU_CONF_DMX512: {
								auto& conf = common_config::Get().GetConfig();
								event_Config_Show_(
									std::vformat(
										std::wstring_view(LangInterface::Get().GetString(STRING_LOGV_MSG6)),
										std::make_wformat_args(L"LIGTS/DMX")
									),
									conf->lightconf.dmxconf.dump()
								);
								break;
							}
							case DLG_LOGVIEW_MENU_CONF_ARTNET: {
								auto& conf = common_config::Get().GetConfig();
								event_Config_Show_(
									std::vformat(
										std::wstring_view(LangInterface::Get().GetString(STRING_LOGV_MSG6)),
										std::make_wformat_args(L"LIGTS/ARTNET")
									),
									conf->lightconf.artnetconf.dump()
								);
								break;
							}
							case DLG_LOGVIEW_MENU_CONF_REMOTE: {
								auto& conf = common_config::Get().GetConfig();
								event_Config_Show_(LangInterface::Get().GetString(STRING_LOGV_MSG7), conf->remoteconf.dump());
								break;
							}
							case DLG_LOGVIEW_MENU_CONF_GAMEPAD: {
								auto& conf = common_config::Get().GetConfig();
								event_Config_Show_(LangInterface::Get().GetString(STRING_LOGV_MSG8), conf->gamepadconf.dump());
								break;
							}
							case DLG_LOGVIEW_MENU_CONF_SCRIPT: {
								auto& conf = common_config::Get().GetConfig();
								event_Config_Show_(LangInterface::Get().GetString(STRING_LOGV_MSG8), conf->vmscript.dump());
								break;
							}
							case DLG_PLUGSTAT_MENU: {
								event_Stats_();
								break;
							}
							case DLG_PLUGSTAT_MENU_0:
							case DLG_PLUGSTAT_MENU_1:
							case DLG_PLUGSTAT_MENU_2:
							case DLG_PLUGSTAT_MENU_3:
							case DLG_PLUGSTAT_MENU_4:
							case DLG_PLUGSTAT_MENU_5:
							case DLG_PLUGSTAT_MENU_6:
							case DLG_PLUGSTAT_MENU_7:
							case DLG_PLUGSTAT_MENU_8:
							case DLG_PLUGSTAT_MENU_9: {
								event_Stat_(c);
								break;
							}
							case DLG_EXIT:
							case IDCANCEL: {
								dispose_();
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

		LRESULT CALLBACK DialogLogView::event_edit_(HWND hwnd, UINT m, WPARAM w, LPARAM l, UINT_PTR sc, DWORD_PTR data) {
			switch (m) {
				case WM_COMMAND: {
					uint16_t c{ LOWORD(w) };
					switch (c) {
						case DLG_EVENT_LOG:
						case DLG_EVENT_MONITOR: {
							if (!l || !data) break;
							DialogLogView* dlgl = reinterpret_cast<DialogLogView*>(data);
							if (dlgl) {
								CbEventData* d = reinterpret_cast<CbEventData*>(l);
								if (!d) break;
								if (c == DLG_EVENT_LOG) dlgl->event_Log_(d);
								else if (c == DLG_EVENT_MONITOR) dlgl->event_Monitor_(d);
							}
							return static_cast<INT_PTR>(1);
						}
						default: break;
					}
					break;
				}
				default: break;
			}
			return ::DefSubclassProc(hwnd, m, w, l);
		}
	}
}
