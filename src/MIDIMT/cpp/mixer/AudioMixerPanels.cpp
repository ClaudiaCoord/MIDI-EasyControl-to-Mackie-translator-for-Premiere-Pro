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
		using namespace std::string_view_literals;

		static const uint16_t sprites_id[][2] = {
			{ IDB_KNOB_LIGHT_ENABLED, IDB_KNOB_LIGHT_DISABLED },
			{ IDB_KNOB_DARK_ENABLED, IDB_KNOB_DARK_DISABLED },
			{ IDB_KNOB_METRO_ENABLED, IDB_KNOB_METRO_DISABLED },
			{ IDB_KNOB_MODERN_ENABLED, IDB_KNOB_MODERN_DISABLED },
			{ IDB_KNOB_RETRO_ENABLED, IDB_KNOB_RETRO_DISABLED },
			{ IDI_ICON_MENUH_LIGHT, IDI_ICON_MENUH_DARK } /* not sprites, this icon set */
		};
		static const uint16_t panel_width = 86;
		static const uint16_t menu_height = 96;
		static const uint16_t menu_width = 126;

		TransportItem::TransportItem(MIXER::AudioSessionItemChange i) : item(i) {
		}
		TransportDeleter::TransportDeleter(TransportItem* t) : tr(t) {
		}
		TransportDeleter::~TransportDeleter() {
			if (tr != nullptr) delete tr;
		}

		POINT&	BuildPoint::Build(HWND hwnd, LPARAM lparam) {
			try {
				POINTS p = MAKEPOINTS(lparam);
				p__.x = p.x; p__.y = p.y;

				(void) ::MapWindowPoints(hwnd, 0, &p__, 1);
				bool b = common_config::Get().UiThemes.IsPlaceVertical();
				p__.x = b ? p__.x : 0;
				p__.y = b ? 0 : p__.y;

			} catch (...) {
				p__.x = 0;
				p__.y = 0;
			}
			return std::ref(p__);
		}
		RECT	BuildPoint::Build() {
			RECT r{};
			try {
				RECT rr = common_config::Get().UiThemes.GetDisplay();
				if (::GetWindowRect(::GetDesktopWindow(), &rr)) {
					r.left = p__.x;
					r.right = p__.y;
					r.top = rr.right;
					r.bottom = rr.bottom;
				}
			} catch (...) {}
			return r;
		}
		void	BuildPoint::Set(RECT r) {
			p__.x = r.left;
			p__.y = r.right;
		}
		void	BuildPoint::Save() {
			common_config::Get().UiThemes.SetDisplay(Build());
		}
		void	BuildPoint::Save(RECT r) {
			Set(r);
			common_config::Get().UiThemes.SetDisplay(r);
		}

		AudioMixerPanels::AudioMixerPanels(TrayNotify* tn) : trayn(tn) {
			inituitheme();
			inituiele();
			event_id__ = Utils::random_hash(this);
			event_cb__ = std::bind(
				static_cast<void(AudioMixerPanels::*)(MIXER::AudioSessionItemChange)>(&AudioMixerPanels::eventmixercb),
				this, _1
			);
			dlgt = std::make_unique<DialogThemeColors>();
		}
		AudioMixerPanels::~AudioMixerPanels() {
			dispose();
		}
		PanelData& AudioMixerPanels::GetPanel() {
			return std::ref(ctrl__[AudioMixerPanel::ITEMID::PANEL_ID]);
		}
		BuildPoint& AudioMixerPanels::GetBuildPoint() {
			return std::ref(bp__);
		}
		DialogThemeColors* AudioMixerPanels::GetDialogThemeColors() {
			return dlgt.get();
		}

		void	AudioMixerPanels::dispose() {
			Close();
			icon__.Release();
			sprites__.clear();
		}
		RECT	AudioMixerPanels::panelposition() {
			RECT r = bp__.Build(), rr{};
			bp__.Save(r);

			switch (common_config::Get().UiThemes.GetPlaceId()) {
				case ui_themes::ThemePlace::VerticalLeft: {
					rr.top = panel_width;
					rr.bottom = r.bottom;
					break;
				}
				case ui_themes::ThemePlace::VerticalRight: {
					rr.top = panel_width;
					rr.bottom = r.bottom;
					rr.left = (r.top - rr.top);
					break;
				}
				case ui_themes::ThemePlace::HorizontalTop: {
					rr.top = r.top;
					rr.bottom = panel_width + 10;
					rr.right = 0;
					break;
				}
				case ui_themes::ThemePlace::HorizontalBottom: {
					rr.top = r.top;
					rr.bottom = panel_width + 10;
					rr.right = (::GetSystemMetrics(SM_CYFULLSCREEN) - rr.bottom + 20);
					break;
				}
				default: break;
			}
			return rr;
		}
		POINT	AudioMixerPanels::menuposition() {
			POINT p{};
			RECT r = bp__.Build();
			switch (common_config::Get().UiThemes.GetPlaceId()) {
				case ui_themes::ThemePlace::VerticalLeft: {
					p.x = ((r.left + panel_width + menu_width) >= r.top) ?
						(r.left - panel_width - menu_width + 2) :
						(r.left + panel_width - 4);
					p.y = 4;
					break;
				}
				case ui_themes::ThemePlace::VerticalRight: {
					#pragma warning( push )
					#pragma warning( disable : 4244 )
					p.x = (r.left <= 0) ? (r.top - ((menu_width * 1.7) + panel_width)) :
						((r.left <= (panel_width + menu_width)) ?
							(r.left + panel_width - 2) :
							(r.left - panel_width - menu_width + 2));
					#pragma warning( pop )
					p.y = 4;
					break;
				}
				case ui_themes::ThemePlace::HorizontalTop: {
					p.x = 4;
					p.y = (r.right <= 0) ? menu_height :
						((r.right <= menu_height) ?
							(r.right + menu_height) :
							(r.right - menu_height - 12));
					break;
				}
				case ui_themes::ThemePlace::HorizontalBottom: {
					p.x = 4;
					p.y = (r.right == 0) ? (::GetSystemMetrics(SM_CYFULLSCREEN) - menu_height - panel_width - 4) :
						((r.right <= menu_height) ?
							(r.right + menu_height) :
							(r.right - menu_height - 12));

					break;
				}
				default: break;
			}
			return p;
		}
		void	AudioMixerPanels::inituiele() {

			RECT p = panelposition();
			ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].Set(
				WS_POPUP | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS | BS_FLAT,
				p.left, p.right, p.top, p.bottom
			);
			ctrl__[AudioMixerPanel::ITEMID::ICON_ID].Set(
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_FLAT | WS_TABSTOP | SS_NOTIFY | SS_ICON,
				4, 4, 24, 24
			);
		}
		void	AudioMixerPanels::inituitheme() {
			try {
				bool isblack = false;
				uint32_t ide, idd;
				common_config& cnf = common_config::Get();
				ui_themes::ThemeId tid = cnf.UiThemes.GetThemeId();
				Theme = cnf.UiThemes.GetTheme(tid);

				switch (tid) {
					case ui_themes::ThemeId::Light:  ide = sprites_id[0][0]; idd = sprites_id[0][1]; break;
					case ui_themes::ThemeId::Dark:   ide = sprites_id[1][0]; idd = sprites_id[1][1]; isblack = true; break;
					case ui_themes::ThemeId::Metro:  ide = sprites_id[2][0]; idd = sprites_id[2][1]; break;
					case ui_themes::ThemeId::Modern: ide = sprites_id[3][0]; idd = sprites_id[3][1]; isblack = true; break;
					case ui_themes::ThemeId::Retro:  ide = sprites_id[4][0]; idd = sprites_id[4][1]; isblack = true; break;
					case ui_themes::ThemeId::Custom: {
						switch (cnf.UiThemes.GetCustomThemeId()) {
							case ui_themes::ThemeId::Dark:   ide = sprites_id[1][0]; idd = sprites_id[1][1]; isblack = true; break;
							case ui_themes::ThemeId::Metro:  ide = sprites_id[2][0]; idd = sprites_id[2][1]; break;
							case ui_themes::ThemeId::Modern: ide = sprites_id[3][0]; idd = sprites_id[3][1]; isblack = true; break;
							case ui_themes::ThemeId::Retro:  ide = sprites_id[4][0]; idd = sprites_id[4][1]; isblack = true; break;
							default:					     ide = sprites_id[0][0]; idd = sprites_id[0][1]; break;
						}
						break;
					}
					default: return;
				}
				uint16_t iconid[]{ isblack ? sprites_id[5][1] : sprites_id[5][0] };
				icon__.Release();
				icon__.Init(iconid, std::size(iconid), true);
				sprites__.load(ide, idd);
			} catch (...) {}

		}
		void	AudioMixerPanels::buildmenu() {
			try {
				HMENU hmain = LangInterface::Get().GetMenu(MAKEINTRESOURCEW(IDR_MIXER_CTRL_MENU));
				if (hmain) {
					HMENU hm = ::GetSubMenu(hmain, 0);
					if (hm) {

						POINT p = menuposition();
						uint32_t uFlags = TPM_RIGHTBUTTON;
						if (::GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0) uFlags |= TPM_RIGHTALIGN;
						else uFlags |= TPM_LEFTALIGN;

						common_config& cnf = common_config::Get();
						uint32_t uitheme = static_cast<uint32_t>(cnf.UiThemes.GetThemeId()) + IDM_THEME_BASE;
						uint32_t uiplace = static_cast<uint32_t>(cnf.UiThemes.GetPlaceId()) + IDM_PLACE_BASE;

						::EnableMenuItem(hm, uitheme, MF_GRAYED | MF_BYCOMMAND);
						::EnableMenuItem(hm, uiplace, MF_GRAYED | MF_BYCOMMAND);
						::CheckMenuItem(hm, uitheme, MF_CHECKED | MF_BYCOMMAND);
						::CheckMenuItem(hm, uiplace, MF_CHECKED | MF_BYCOMMAND);

						::TrackPopupMenuEx(hm,
							uFlags,
							p.x,
							p.y,
							ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND(),
							0);
					}
					::DestroyMenu(hmain);
				}
			} catch (...) {}
		}

		/* DATA EVENT */

		void	AudioMixerPanels::eventmixercb(MIXER::AudioSessionItemChange item) {
			HWND hwnd = ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND();
			if (hwnd == nullptr) return;

			::PostMessageW(hwnd,
				WM_COMMAND,
				MAKEWPARAM(IDC_IEVENT_DATA, 0),
				(LPARAM) new TransportItem(std::move(item)));
		}
		void	AudioMixerPanels::eventmixercb(TransportItem* titem) {
			if (titem == nullptr) return;

			TransportDeleter t(titem);
			#if defined(_DEBUG)
			::OutputDebugStringW(t.tr->item.ToString().str().c_str());
			#endif

			try {
				if (t.tr->item.GetAction() == MIXER::OnChangeType::OnChangeNone) return;

				MIXER::OnChangeType dst = MIXER::OnChangeType::OnChangeNew;
				RECT pr{};
				size_t back;

				/* stage 1 */
				for (back = 0U; back < panels__.size(); back++) {
					auto& panel = panels__[back];
					if (panel->GetAudioItem().IsKeyFound(t.tr->item.Data)) {
						switch (t.tr->item.GetAction()) {
							case MIXER::OnChangeType::OnChangeUpdateData:
							case MIXER::OnChangeType::OnChangeNew: {
								panel->UpdateCtrlData(t.tr->item);
								return;
							}
							case MIXER::OnChangeType::OnChangeUpdatePan:
							case MIXER::OnChangeType::OnChangeUpdateMute:
							case MIXER::OnChangeType::OnChangeUpdateVolume: {
								panel->UpdateCtrlValue(t.tr->item);
								return;
							}
							case MIXER::OnChangeType::OnChangeRemove: {
								dst = MIXER::OnChangeType::OnChangeRemove;
								pr = panel->GetSize();
								position__ -= pr.bottom;
								try {
									auto dpanel = panels__[back];
									panels__.erase(panels__.begin() + back);
									dpanel->Close();
									delete dpanel;
								} catch (...) {
									Utils::get_exception(std::current_exception(), __FUNCTIONW__);
									return;
								}
								break;
							}
							default: return;
						}
						if (dst == MIXER::OnChangeType::OnChangeRemove) break;
					}
				}
				/* stage 2 */
				if (dst == MIXER::OnChangeType::OnChangeRemove) {
					for (size_t fwd = back; fwd < panels__.size(); fwd++) {
						auto& panel = panels__[fwd];
						RECT nr = panel->GetSize();
						panel->SetSize(pr);
						std::swap(nr, pr);
					}
					RECT r{};
					HWND hwnd = ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND();
					::GetClientRect(hwnd, &r);
					::InvalidateRect(hwnd, &r, true);
				}
				else if ((dst == MIXER::OnChangeType::OnChangeNew) && (t.tr->item.GetAction() != MIXER::OnChangeType::OnChangeRemove))
					add(std::move(t.tr->item));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		bool	AudioMixerPanels::add(MIXER::AudioSessionItemChange item) {
			try {
				if (!::IsGUIThread(true)) {
					std::wstring ws = LangInterface::Get().GetString(IDS_INF1O);
					throw runtime_werror(std::move(ws));
				}

				HINSTANCE hi = LangInterface::Get().GetMainHinstance();
				AudioMixerPanel* panel = new AudioMixerPanel(hi, std::ref(sprites__));
				if (panel->Open(hi, ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND(), std::move(item), Theme, startid__, position__) == nullptr) {
					delete panel;
					return false;
				}
				panels__.push_back(panel);
				return true;
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				return false;
		}

		void	AudioMixerPanels::CallMenu() {
			buildmenu();
		}
		void	AudioMixerPanels::Close() {
			try {
				MIXER::AudioSessionMixer::Get().event_remove(event_id__);
				std::vector<AudioMixerPanel*> panels = panels__;
				panels__.clear();
				for (auto& a : panels)
					if (a != nullptr) delete a;

				for (size_t i = 0; i < _countof(ctrl__); i++)
					ctrl__[i].Close();

				startid__ = 50000;
				position__ = 0;
				isopen__ = false;
			} catch (...) {}
		}
		bool	AudioMixerPanels::Open() {
			try {
				Close();
				if (!Utils::random_isvalid(event_id__)) {
					LangInterface& lang = LangInterface::Get();
					std::wstring ws = lang.GetString(IDS_INFO9);
					trayn->Warning(lang.GetString(IDS_INF12), ws);
					throw new runtime_werror(std::move(ws));
				}

				common_config& cnf = common_config::Get();
				if (!cnf.Local.IsAudioMixerRun()) {
					LangInterface& lang = LangInterface::Get();
					std::wstring ws = lang.GetString(IDS_INFO8);
					trayn->Warning(lang.GetString(IDS_INF11), ws);
					throw new runtime_werror(std::move(ws));
				}

				inituitheme();
				inituiele();

				HINSTANCE hi = LangInterface::Get().GetMainHinstance();
				PanelData& pd = ctrl__[AudioMixerPanel::ITEMID::PANEL_ID],
						 & pi = ctrl__[AudioMixerPanel::ITEMID::ICON_ID];
				pd.Init(hi, 0, startid__++, 0, (SUBCLASSPROC)AudioMixerPanels::EventPANEL_ID, this);
				pi.Init(hi, pd.GetHWND(), startid__++, 0, (SUBCLASSPROC)AudioMixerPanels::EventICON_ID, this);
				pi.SetData(icon__.GetIcon());
				RECT r = pi.GetSize();
				position__ = cnf.UiThemes.IsPlaceVertical() ? (r.bottom + r.right) : (r.bottom + r.left);
				MIXER::AudioSessionMixer::Get().event_add(event_cb__, event_id__);
				dlgt->SetHWNDParent(pd.GetHWND());
				isopen__ = true;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				isopen__ = false;
			}
			return isopen__.load();
		}
		void	AudioMixerPanels::Show() {
			try {
				if (isopen__) Close();
				else		  Open();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void	AudioMixerPanels::SwitchUiTheme(ui_themes::ThemeId id) {
			common_config::Get().UiThemes.SetTheme(id);
			Open();
		}
		void	AudioMixerPanels::SwitchUiPlace(ui_themes::ThemePlace place) {
			common_config::Get().UiThemes.SetPlace(place);
			Open();
		}
		bool	AudioMixerPanels::IsOpen() const {
			return isopen__.load();
		}

		/* STATIC */

		static AudioMixerPanels* getProcClass(DWORD_PTR data) {
			try {
				return reinterpret_cast<AudioMixerPanels*>(data);
			} catch (...) { return nullptr; }
		}
		static DialogThemeColors* getLongClass(HWND hwnd) {
			try {
				LONG_PTR lptr = ::GetWindowLongPtr(hwnd, DWLP_USER);
				if (lptr == 0L) return nullptr;
				return reinterpret_cast<DialogThemeColors*>(lptr);

			} catch (...) { return nullptr; }
		}

		INT_PTR CALLBACK AudioMixerPanels::CustomThemeDialog(HWND hwnd, UINT m, WPARAM w, LPARAM l) {
			DialogThemeColors* dlgt;
			switch (m) {
				case WM_INITDIALOG: {
					if (l == 0) break;
					dlgt = reinterpret_cast<DialogThemeColors*>(l);
					if (!dlgt) break;

					(void) ::GetLastError();
					if (::SetWindowLongPtr(hwnd, DWLP_USER, l) < 0L) {
						to_log::Get() << (log_string() << L"CustomThemeDialog : set user data error=" << ::GetLastError());
						break;
					}
					dlgt->InitDialog(hwnd);
					return (INT_PTR)true;
				}
				case WM_COMMAND: {
					uint32_t id = LOWORD(w);
					if ((!(dlgt = getLongClass(hwnd))) && (id != IDCANCEL)) return (INT_PTR)false;

					switch (id) {
						case IDC_COLOR_BOX1:
						case IDC_COLOR_BOX2:
						case IDC_COLOR_BOX3: {
							dlgt->ColorSelector(id);
							break;
						}
						case IDC_RADIO_DARK:
						case IDC_RADIO_LIGHT:
						case IDC_RADIO_METRO:
						case IDC_RADIO_RETRO:
						case IDC_RADIO_MODERN: {
							dlgt->ThemeSelector(id);
							break;
						}
						case IDCANCEL: {
							if (dlgt)
								dlgt->EndDialog();
							::EndDialog(hwnd, LOWORD(w));
							return (INT_PTR)true;
						}
					}
					break;
				}
				case WM_CTLCOLORSTATIC: {
					if ((dlgt = getLongClass(hwnd)) == nullptr) break;
					return dlgt->ColorsDraw(l);
					break;
				}
			}
			return (INT_PTR)false;
		}
		LRESULT CALLBACK AudioMixerPanels::EventPANEL_ID(HWND hwnd, UINT m, WPARAM w, LPARAM l, UINT_PTR sc, DWORD_PTR data) {
			AudioMixerPanels* aps;
			switch (m) {
				case WM_PAINT: {
					if ((aps = getProcClass(data)) == nullptr) return ::DefSubclassProc(hwnd, m, w, l);

					PAINTSTRUCT ps{};
					HDC hdc = ::BeginPaint(hwnd, &ps);
					(void) ::SetBkMode(hdc, TRANSPARENT);
					(void) ::SetBkColor(hdc, aps->Theme.PanelBackground);
					(void) ::EndPaint(hwnd, &ps);
					break;
				}
				case WM_CTLCOLORSTATIC: {
					if ((aps = getProcClass(data)) == nullptr) return ::DefSubclassProc(hwnd, m, w, l);
					return reinterpret_cast<LRESULT>(aps->Theme.GetBackgroundBrush());
				}
				case WM_ERASEBKGND: {
					if ((aps = getProcClass(data)) == nullptr) return ::DefSubclassProc(hwnd, m, w, l);

					HDC hdc = (HDC)(w);
					RECT rc; GetClientRect(hwnd, &rc);
					FillRect(hdc, &rc, aps->Theme.GetBackgroundBrush());
					break;
				}
				case WM_COMMAND: {
					int32_t cmd = LOWORD(w);

					if (!(aps = getProcClass(data)))
						return ::DefSubclassProc(hwnd, m, w, l);

					switch (cmd) {
						case IDM_HOR_TOP:
						case IDM_VERT_LEFT:
						case IDM_HOR_BOTTOM:
						case IDM_VERT_RIGHT: {
							ui_themes::ThemePlace place = static_cast<ui_themes::ThemePlace>(LOWORD(w) - IDM_PLACE_BASE);
							if (place == common_config::Get().UiThemes.GetPlaceId()) return FALSE;
							aps->SwitchUiPlace(place);
							break;
						}
						case IDM_DARK:
						case IDM_LIGHT:
						case IDM_METRO:
						case IDM_RETRO:
						case IDM_MODERN:
						case IDM_CUSTOM: {
							ui_themes::ThemeId theme = static_cast<ui_themes::ThemeId>(LOWORD(w) - IDM_THEME_BASE);
							if (theme == common_config::Get().UiThemes.GetThemeId()) return FALSE;
							aps->SwitchUiTheme(theme);
							break;
						}
						case IDM_CUSTOM_RELOAD: {
							aps->SwitchUiTheme(ui_themes::ThemeId::Custom);
							break;
						}
						case IDM_CALLFORM_COLOR: {
							DialogThemeColors* dtc = aps->GetDialogThemeColors();
							if (!dtc) break;
							if (dtc->IsRun())
								dtc->SetFocus();
							else {
								LangInterface& lang = LangInterface::Get();
								lang.GetDialog(
									lang.GetMainHwnd(),
									&AudioMixerPanels::CustomThemeDialog,
									MAKEINTRESOURCEW(IDD_FORMCOLOR),
									reinterpret_cast<LPARAM>(dtc)
								);
							}
							break;
						}
						case IDC_IEVENT_DATA: {
							aps->eventmixercb(reinterpret_cast<TransportItem*>(l));
							break;
						}
						case IDM_CLOSE: {
							aps->Close();
							break;
						}
						case IDM_SAVE_POS: {
							aps->GetBuildPoint().Save();
							break;
						}
						default: return ::DefSubclassProc(hwnd, m, w, l);
					}
					break;
				}
				default: return ::DefSubclassProc(hwnd, m, w, l);
			}
			return TRUE;
		}
		LRESULT CALLBACK AudioMixerPanels::EventICON_ID(HWND hwnd, UINT m, WPARAM w, LPARAM l, UINT_PTR sc, DWORD_PTR data) {
			AudioMixerPanels* aps;
			switch (m) {
				case WM_RBUTTONUP: {
					if ((aps = getProcClass(data)) == nullptr) return FALSE;
					aps->Close();
					break;
				}
				case WM_MBUTTONDOWN: {
					(void) ::SetCapture(hwnd);
					if (common_config::Get().UiThemes.IsPlaceVertical())
						(void) ::SetCursor(::LoadCursorW(0, IDC_SIZEWE));
					else
						(void) ::SetCursor(::LoadCursorW(0, IDC_SIZENS));
					break;
				}
				case WM_MBUTTONUP: {
					RECT r{};
					(void) ::ReleaseCapture();
					(void) ::SetCursor(::LoadCursorW(0, IDC_ARROW));
					if ((aps = getProcClass(data)) != nullptr)
						aps->GetBuildPoint().Save();
					break;
				}
				case WM_LBUTTONUP: {
					if ((aps = getProcClass(data)) == nullptr) return FALSE;
					aps->CallMenu();
					break;
				}
				case WM_MOUSEMOVE: {
					switch (w) {
						case MK_MBUTTON: {
							if ((aps = getProcClass(data)) == nullptr) return FALSE;

							PanelData& panel = aps->GetPanel();
							BuildPoint& bp = aps->GetBuildPoint();
							POINT p = bp.Build(panel.GetHWND(), l);
							::SetWindowPos(panel.GetHWND(), HWND_TOP, p.x, p.y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
							break;
						}
						default: return ::DefSubclassProc(hwnd, m, w, l);
					}
					break;
				}
				default: return ::DefSubclassProc(hwnd, m, w, l);
			}
			return TRUE;
		}
	}
}
