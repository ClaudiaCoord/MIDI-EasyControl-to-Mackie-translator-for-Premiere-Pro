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

		#if defined(_DEBUG)
		/* #define _DEBUG_AUDIOMIXERPANELS 1 */
		#endif

		using namespace std::placeholders;
		using namespace std::string_view_literals;

		static const uint16_t sprites_id[][2] = {
			{ SPRITE_KNOB_LIGHT_ENABLED, SPRITE_KNOB_LIGHT_DISABLED },
			{ SPRITE_KNOB_DARK_ENABLED, SPRITE_KNOB_DARK_DISABLED },
			{ SPRITE_KNOB_METRO_ENABLED, SPRITE_KNOB_METRO_DISABLED },
			{ SPRITE_KNOB_MODERN_ENABLED, SPRITE_KNOB_MODERN_DISABLED },
			{ SPRITE_KNOB_RETRO_ENABLED, SPRITE_KNOB_RETRO_DISABLED },
			{ ICON_APP_ICON_MENUH_LIGHT, ICON_APP_ICON_MENUH_DARK } /* not sprites, this icon set */
		};
		static const uint16_t panel_width = 86;
		static const uint16_t menu_height = 96;
		static const uint16_t menu_width = 126;

		TransportItem::TransportItem(MIXER::AudioSessionItemChange i) : item(i) {
		}
		TransportDeleter TransportItem::GetDeleter() {
			return TransportDeleter(this);
		}
		TransportDeleter::TransportDeleter(TransportItem* t) : ti(t) {
		}
		TransportDeleter::~TransportDeleter() {
			TransportItem* t = ti;
			ti = nullptr;
			if (t) delete t;
		}

		POINT&	BuildPoint::Build(HWND hwnd, LPARAM lparam) {
			try {
				POINTS p = MAKEPOINTS(lparam);
				p_.x = p.x; p_.y = p.y;

				(void) ::MapWindowPoints(hwnd, 0, &p_, 1);
				bool b = common_config::Get().UiThemes.IsPlaceVertical();
				p_.x = b ? p_.x : 0;
				p_.y = b ? 0 : p_.y;

			} catch (...) {
				p_.x = 0;
				p_.y = 0;
			}
			return std::ref(p_);
		}
		RECT	BuildPoint::Build() {
			RECT r{};
			try {
				RECT rr = common_config::Get().UiThemes.GetDisplay();
				if (::GetWindowRect(::GetDesktopWindow(), &rr)) {
					r.left = p_.x;
					r.right = p_.y;
					r.top = rr.right;
					r.bottom = rr.bottom;
				}
			} catch (...) {}
			return r;
		}
		void	BuildPoint::Set(RECT r) {
			p_.x = r.left;
			p_.y = r.right;
		}
		void	BuildPoint::Save() {
			common_config::Get().UiThemes.SetDisplay(Build());
		}
		void	BuildPoint::Save(RECT r) {
			Set(r);
			common_config::Get().UiThemes.SetDisplay(r);
		}

		AudioMixerPanels::AudioMixerPanels() : event_id_(0U) {
			init_ui_theme_();
			init_ui_ele_(common_config::Get().UiThemes.GetPlaceId());
			event_id_ = Utils::random_hash(this);
			event_cb_ = std::bind(
				static_cast<void(AudioMixerPanels::*)(MIXER::AudioSessionItemChange)>(&AudioMixerPanels::event_mixer_cb_),
				this, _1
			);

			ti_.cbSize = sizeof(ti_);
			ti_.uFlags = TTF_TRANSPARENT | TTF_SUBCLASS | TTF_CENTERTIP;
			ti_.uId = 0;
			ti_.hinst = 0;
			ti_.lpszText = 0;
		}
		AudioMixerPanels::~AudioMixerPanels() {
			dispose_();
		}
		UI::Panel& AudioMixerPanels::GetPanel() {
			return std::ref(ctrl_[AudioMixerPanel::ITEMID::PANEL_ID]);
		}
		BuildPoint& AudioMixerPanels::GetBuildPoint() {
			return std::ref(bp_);
		}

		void	AudioMixerPanels::close_() {
			try {
				auto panels = panels_;
				panels_.clear();
				for (int32_t i = static_cast<long>(panels.size() - 1); 0 <= i; i--) {
					auto& a = panels[i];
					if (a) a.reset();
				}

				for (size_t i = 0; i < _countof(ctrl_); i++)
					ctrl_[i].Close(AW_BLEND);

			} catch (...) {}
		}
		void	AudioMixerPanels::dispose_() {
			close_();
			icon_.Release();
			sprites_.clear();
		}
		RECT	AudioMixerPanels::panel_position_(const ui_themes::ThemePlace place) {
			RECT r = bp_.Build(), rr{};
			bp_.Save(r);

			switch (place) {
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
		int32_t AudioMixerPanels::panel_begin_size_(bool b) {
			RECT r = ctrl_[AudioMixerPanel::ITEMID::ICON_ID].GetSize();
			return UI::PLACESIZE(b, r) + 2;
		}
		int32_t AudioMixerPanels::panel_begin_size_(common_config& cnf, RECT& r) {
			return UI::PLACESIZE(cnf.UiThemes.IsPlaceVertical(), r) + 2;
		}
		POINT	AudioMixerPanels::menu_position_(const ui_themes::ThemePlace place) {
			POINT p{};
			RECT r = bp_.Build();
			switch (place) {
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
		void	AudioMixerPanels::init_ui_ele_(const ui_themes::ThemePlace place) {

			RECT p = panel_position_(place);
			ctrl_[AudioMixerPanel::ITEMID::PANEL_ID].Set(
				WS_POPUP | WS_BORDER | WS_CLIPSIBLINGS | BS_FLAT | SS_NOTIFY,
				p.left, p.right, p.top, p.bottom
			);
			ctrl_[AudioMixerPanel::ITEMID::ICON_ID].Set(
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_FLAT | WS_TABSTOP | SS_NOTIFY | SS_ICON,
				4, 4, 24, 24
			);
			ctrl_[AudioMixerPanel::ITEMID::BALLOON_ID].Set(
				WS_POPUP | TTS_NOPREFIX | TTS_BALLOON,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT
			);
		}
		void	AudioMixerPanels::init_ui_theme_() {
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
				icon_.Release();
				icon_.Init(
					iconid,
					std::size(iconid),
					[=](uint16_t n) -> HICON {
						return LangInterface::Get().GetIcon24x24(MAKEINTRESOURCEW(n));
					},
					true);
				(void) sprites_.size(31);
				sprites_.load(ide, idd, LangInterface::Get().GetMainHinstance());
			} catch (...) {}

		}
		void	AudioMixerPanels::build_menu_() {
			try {
				HMENU hmain = LangInterface::Get().GetMenu(MAKEINTRESOURCEW(DLG_MIXER_CTRL_MENU));
				if (hmain) {
					HMENU hm = ::GetSubMenu(hmain, 0);
					if (hm) {

						common_config& cnf = common_config::Get();
						const ui_themes::ThemePlace place = cnf.UiThemes.GetPlaceId();
						POINT p = menu_position_(place);
						uint32_t uFlags = TPM_RIGHTBUTTON;
						if (::GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0) uFlags |= TPM_RIGHTALIGN;
						else uFlags |= TPM_LEFTALIGN;

						uint32_t uitheme = static_cast<uint32_t>(cnf.UiThemes.GetThemeId()) + IDM_THEME_BASE;
						uint32_t uiplace = static_cast<uint32_t>(place) + IDM_PLACE_BASE;

						std::forward_list<uint32_t> list { uitheme, uiplace };
						if (cnf.Registry.GetUiShowAppPath())
							list.emplace_front(IDM_APP_PATH);
						if (cnf.Registry.GetUiShowMidiKeyBind())
							list.emplace_front(IDM_MIDI_BIND);
						if (cnf.Registry.GetUiShowAudioPeakMeter())
							list.emplace_front(IDM_PEAK_METER);

						for (auto u : list) {
							::CheckMenuItem(hm, u, MF_CHECKED | MF_BYCOMMAND);
						}
						::EnableMenuItem(hm, uitheme, MF_GRAYED | MF_BYCOMMAND);
						::EnableMenuItem(hm, uiplace, MF_GRAYED | MF_BYCOMMAND);

						if (cnf.Registry.GetUiAnimation())
							::CheckMenuItem(hm, IDM_ANIMATION, MF_CHECKED | MF_BYCOMMAND);
						if (cnf.Registry.GetUiShowAppPath())
							::CheckMenuItem(hm, IDM_APP_PATH, MF_CHECKED | MF_BYCOMMAND);

						::TrackPopupMenuEx(hm,
							uFlags,
							p.x,
							p.y,
							ctrl_[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND(),
							0);
					}
					::DestroyMenu(hmain);
				}
			} catch (...) {}
		}
		void	AudioMixerPanels::set_balloon_(HWND hwnd) {
			try {
				HWND hwnb;
				if ((hwnd == nullptr) || ((hwnb = ctrl_[AudioMixerPanel::ITEMID::BALLOON_ID].GetHWND()) == nullptr)) return;
				ti_.hwnd = hwnd;
				(void) ::SendMessageW(hwnb, TTM_DELTOOLW, (WPARAM)0, (LPARAM)&ti_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void	AudioMixerPanels::set_balloon_(UI::ToolTipData data) {
			try {
				do {
					if (!data.IsValid()) break;
					HWND hwnd = ctrl_[AudioMixerPanel::ITEMID::BALLOON_ID].GetHWND();
					if (!hwnd) break;

					ti_.hwnd = data.hwnd;
					ti_.rect = data.rect;
					ti_.lpszText = data.title.data();

					if (!::SendMessageW(hwnd, TTM_ADDTOOL, (WPARAM)0, (LPARAM)&ti_)) break;
					if (!::SendMessageW(hwnd, TTM_ADJUSTRECT, (WPARAM)TRUE, (LPARAM)&ti_.rect)) break;
					(void) ::SendMessageW(hwnd, TTM_UPDATETIPTEXTW, (WPARAM)0, (LPARAM)&ti_);
					if (::SendMessageW(hwnd, TTM_SETMAXTIPWIDTH, (WPARAM)0, (LPARAM)200) != 200) break;

				} while (0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void	AudioMixerPanels::notify_warning_(std::wstring title, std::wstring body) {
			auto& notify = ClassStorage::Get().GetDialog<TrayNotify>();
			if (notify)	notify->Warning(title, body);
			throw make_common_error(std::move(body));
		}

		/* DATA EVENT */

		void	AudioMixerPanels::event_mixer_cb_(MIXER::AudioSessionItemChange item) {
			HWND hwnd = ctrl_[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND();
			if (!hwnd) return;

			::PostMessageW(hwnd,
				WM_COMMAND,
				MAKEWPARAM(DLG_AMIX_EVENT_DATA, 0),
				(LPARAM) new TransportItem(std::move(item)));
		}
		void	AudioMixerPanels::event_mixer_cb_(TransportItem* titem) {
			if (!titem) return;

			TransportDeleter t = titem->GetDeleter();
			#if defined(_DEBUG_AUDIOMIXERPANELS)
			::OutputDebugStringW(t.ti->item.to_string().str().c_str());
			#endif

			try {
				if (t.ti->item.GetAction() == MIXER::OnChangeType::OnChangeNone) return;

				MIXER::OnChangeType dst = MIXER::OnChangeType::OnChangeNew;
				size_t back;
				bool isv = false;

				/* stage 1 */
				for (back = 0U; back < panels_.size(); back++) {
					auto& panel = panels_[back];
					bool b = (isduplicateappremoved_) ?
						(panel->GetAudioItem().Item.IsEqualsApp(t.ti->item.Item.App.get<std::size_t>())) :
						(panel->GetAudioItem().Item.IsEqualsGuid(t.ti->item.Item.App.Guid));

					if (b) {
						switch (t.ti->item.GetAction()) {
							case MIXER::OnChangeType::OnChangeNew:
							case MIXER::OnChangeType::OnChangeUpdatePan:
							case MIXER::OnChangeType::OnChangeUpdateMute:
							case MIXER::OnChangeType::OnChangeUpdateData:
							case MIXER::OnChangeType::OnChangeUpdateVolume:
							case MIXER::OnChangeType::OnChangeUpdateAllValues: {
								panel->UpdateItem(t.ti->item);
								return;
							}
							case MIXER::OnChangeType::OnChangeRemove: {
								dst = MIXER::OnChangeType::OnChangeRemove;
								isv = common_config::Get().UiThemes.IsPlaceVertical();
								RECT r = panel->GetSize();
								int32_t sz = panel_begin_size_(isv);

								position_ -= UI::PLACESIZE(isv, r);
								if (position_ < sz) position_ = sz;

								try {
									auto dpanel = panels_[back];
									panels_.erase(panels_.begin() + back);
									if (dpanel) {
										set_balloon_(dpanel->GetHWND(AudioMixerPanel::ITEMID::TEXT_ID));
										dpanel.reset();
									}
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
					for (size_t fwd = back; fwd < panels_.size(); fwd++) {
						auto& panel = panels_[fwd];
						if (!panel->SetPositionUp(isv, fwd == back)) break;
					}
				}
				else if ((dst == MIXER::OnChangeType::OnChangeNew) && (t.ti->item.GetAction() != MIXER::OnChangeType::OnChangeRemove))
					add_(std::move(t.ti->item));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		bool	AudioMixerPanels::add_(MIXER::AudioSessionItemChange item) {
			try {
				if (!::IsGUIThread(true))
					throw make_common_error(LangInterface::Get().GetString(STRING_AMIX_MSG5));

				common_config& cnf = common_config::Get();
				const bool isv = cnf.UiThemes.IsPlaceVertical();
				const bool animele = cnf.Registry.GetUiAnimation();
				HINSTANCE hi = LangInterface::Get().GetMainHinstance();
				std::shared_ptr<AudioMixerPanel> panel = std::shared_ptr<AudioMixerPanel>(
					new AudioMixerPanel(std::ref(sprites_)), audiomixerpanel_deleter()
				);
				if (panel->Open(hi,
					ctrl_[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND(), std::move(item), Theme, startid_, position_, isv, animele, isduplicateappremoved_.load()) == nullptr) {
					panel.reset();
					return false;
				}
				panels_.push_back(panel);
				if (cnf.Registry.GetUiShowAppPath())
					set_balloon_(panel->GetBalloonData());
				if (cnf.Registry.GetUiShowMidiKeyBind())
					panel->ShowMidiKeyBind(true);
				if (panel->IsMaster() && cnf.Registry.GetUiShowAudioPeakMeter())
					panel->ShowPeakMeter(true);
				return true;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}

		void	AudioMixerPanels::CallMenu() {
			build_menu_();
		}
		void	AudioMixerPanels::Close() {
			try {
				try {
					ClassStorage& st = ClassStorage::Get();
					if (st.IsDialog<MIXER::AudioSessionMixer>())
						st.GetDialog<MIXER::AudioSessionMixer>()->event_remove(event_id_);
				} catch (...) {}

				close_();

				startid_ = 50000;
				position_ = 0;
				isopen_ = false;

			} catch (...) {}
		}
		bool	AudioMixerPanels::Open() {
			try {
				Close();

				if (!Utils::random_isvalid(event_id_)) {
					LangInterface& lang = LangInterface::Get();
					notify_warning_(
						lang.GetString(STRING_AMIX_MSG4),
						lang.GetString(STRING_AMIX_MSG3)
					);
				}

				common_config& cnf = common_config::Get();
				if (!cnf.Local.IsAudioMixerRun()) {
					LangInterface& lang = LangInterface::Get();
					notify_warning_(
						lang.GetString(STRING_AMIX_MSG2),
						lang.GetString(STRING_AMIX_MSG1)
					);
				}

				init_ui_theme_();
				init_ui_ele_(cnf.UiThemes.GetPlaceId());

				HWND hwnd;
				HINSTANCE hi = LangInterface::Get().GetMainHinstance();
				UI::Panel& pd = ctrl_[AudioMixerPanel::ITEMID::PANEL_ID],
						 & pm = ctrl_[AudioMixerPanel::ITEMID::ICON_ID],
						 & pb = ctrl_[AudioMixerPanel::ITEMID::BALLOON_ID];

				pd.Init(hi, 0, startid_++, 0, (SUBCLASSPROC)AudioMixerPanels::EventPANEL_ID, cnf.Registry.GetUiAnimation(), this);
				if (!(hwnd = pd.GetHWND())) return false;

				pm.Init(hi, hwnd, startid_++, 0, (SUBCLASSPROC)AudioMixerPanels::EventICON_ID, false, this);
				pm.SetData(icon_.GetIcon());
				RECT r = pm.GetSize();

				pb.Init(hi, startid_++);

				position_ = panel_begin_size_(cnf, r);

				ClassStorage& st = ClassStorage::Get();
				if (st.IsDialog<MIXER::AudioSessionMixer>())
					st.GetDialog<MIXER::AudioSessionMixer>()->event_add(event_cb_, event_id_);

				pd.Show(AW_BLEND);

				LangInterface& lang = LangInterface::Get();
				(void) ::SendMessageW(hwnd, WM_SETICON, ICON_SMALL,
					reinterpret_cast<LPARAM>(lang.GetIcon48x48(MAKEINTRESOURCEW(ICON_APP_ICON_SOUNDON))));
				(void) ::SendMessageW(hwnd, WM_SETICON, ICON_BIG,
					reinterpret_cast<LPARAM>(lang.GetIcon256x256(MAKEINTRESOURCEW(ICON_APP_ICON_SOUNDON))));

				isduplicateappremoved_ = cnf.Registry.GetMixerDupAppRemove();
				isopen_ = true;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				isopen_ = false;
			}
			return isopen_.load();
		}
		void	AudioMixerPanels::Show() {
			try {
				if (isopen_) Close();
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
		void	AudioMixerPanels::SwitchAnimation() {
			try {
				common_config& cnf = common_config::Get();
				bool val = !cnf.Registry.GetUiAnimation();
				for (auto& a : panels_)
					if (a) a->ShowAnimation(val);

				ctrl_[AudioMixerPanel::ITEMID::PANEL_ID].SetAnimation(val);
				cnf.Registry.SetUiAnimation(val);
			} catch (...) {}
		}
		void	AudioMixerPanels::SwitchShowAppPath() {
			try {
				common_config& cnf = common_config::Get();
				const bool b = !cnf.Registry.GetUiShowAppPath();
				cnf.Registry.SetUiShowAppPath(b);
				for (auto& a : panels_) {
					if (a) {
						if (b) set_balloon_(a->GetBalloonData());
						else   set_balloon_(a->GetHWND(AudioMixerPanel::ITEMID::TEXT_ID));
					}
				}
			} catch (...) {}
		}
		void	AudioMixerPanels::SwitchPeakMeter() {
			try {
				common_config& cnf = common_config::Get();
				const bool b = !cnf.Registry.GetUiShowAudioPeakMeter();
				cnf.Registry.SetUiShowAudioPeakMeter(b);
				for (auto& a : panels_) {
					if (a) {
						if (a->IsMaster()) {
							a->ShowPeakMeter(b);
							break;
						}
					}
				}
			} catch (...) {}
		}
		void	AudioMixerPanels::SwitchVisableStatus(bool b) {
			try {
				worker_background::Get().to_async(
					std::async(std::launch::async, [=](const bool state) {
						try {
							for (auto& a : panels_) {
								if (a) {
									if (a->IsMaster()) {
										a->SetVisableStatus(state);
										break;
									}
								}
							}
						} catch (...) {
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						}
					}, b)
				);
			} catch (...) {}
		}
		void	AudioMixerPanels::SwitchMidiBind() {
			try {
				common_config& cnf = common_config::Get();
				const bool b = !cnf.Registry.GetUiShowMidiKeyBind();
				cnf.Registry.SetUiShowMidiKeyBind(b);
				for (auto& a : panels_) {
					if (a) a->ShowMidiKeyBind(b);
				}
			} catch (...) {}
		}
		bool	AudioMixerPanels::IsOpen() const {
			return isopen_.load();
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

		LRESULT CALLBACK AudioMixerPanels::EventPANEL_ID(HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR sc, DWORD_PTR data) {
			AudioMixerPanels* aps;
			switch (m) {
				case WM_PAINT: {
					if ((aps = getProcClass(data)) == nullptr) return ::DefSubclassProc(h, m, w, l);

					PAINTSTRUCT ps{};
					HDC hdc = ::BeginPaint(h, &ps);
					(void) ::SetBkMode(hdc, TRANSPARENT);
					(void) ::SetBkColor(hdc, aps->Theme.PanelBackground);
					(void) ::EndPaint(h, &ps);
					break;
				}
				case WM_CTLCOLORSTATIC: {
					if ((aps = getProcClass(data)) == nullptr) return ::DefSubclassProc(h, m, w, l);
					return reinterpret_cast<LRESULT>(aps->Theme.GetBackgroundBrush());
				}
				case WM_PRINTCLIENT:
				case WM_ERASEBKGND: {
					if ((aps = getProcClass(data)) == nullptr) return ::DefSubclassProc(h, m, w, l);

					HDC hdc = (HDC)(w);
					RECT rc;
					::GetClientRect(h, &rc);
					::FillRect(hdc, &rc, aps->Theme.GetBackgroundBrush());
					break;
				}
				case WM_RBUTTONUP: {
					if ((aps = getProcClass(data)) == nullptr) return ::DefSubclassProc(h, m, w, l);
					aps->CallMenu();
					break;
				}
				case WM_SETFOCUS:
				case WM_CHILDACTIVATE: {
					if ((aps = getProcClass(data)) != nullptr)
						aps->SwitchVisableStatus(true);
					return ::DefSubclassProc(h, m, w, l);
				}
				case WM_DISPLAYCHANGE: {
					if ((aps = getProcClass(data)) != nullptr)
						aps->SwitchVisableStatus(::IsWindowVisible(h));
					return ::DefSubclassProc(h, m, w, l);
				}
				case WM_ACTIVATE: {
					if ((aps = getProcClass(data)) == nullptr) return ::DefSubclassProc(h, m, w, l);
					switch (LOWORD(w)) {
						case WA_ACTIVE:
						case WA_CLICKACTIVE: {
							aps->SwitchVisableStatus(true);
							break;
						}
						case WA_INACTIVE: {
							aps->SwitchVisableStatus(::IsWindowVisible(h));
							break;
						}
						default: break;
					}
					return ::DefSubclassProc(h, m, w, l);
				}
				case WM_SHOWWINDOW: {
					if ((aps = getProcClass(data)) == nullptr) return ::DefSubclassProc(h, m, w, l);
					switch (l) {
						case 0:
						case SW_OTHERUNZOOM:
						case SW_PARENTOPENING: {
							aps->SwitchVisableStatus(::IsWindowVisible(h));
							break;
						}
						case SW_OTHERZOOM:
						case SW_PARENTCLOSING: {
							aps->SwitchVisableStatus(::IsWindowVisible(h));
							break;
						}
						default: break;
					}
					return ::DefSubclassProc(h, m, w, l);
				}
				case WM_COMMAND: {
					int32_t cmd = LOWORD(w);

					if (!(aps = getProcClass(data)))
						return ::DefSubclassProc(h, m, w, l);

					switch (cmd) {
						case DLG_AMIX_EVENT_DATA: {
							aps->event_mixer_cb_(reinterpret_cast<TransportItem*>(l));
							break;
						}
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
							(void) ClassStorage::Get().OpenDialog<DialogThemeColors>(h, false);
							break;
						}
						case IDM_SAVE_POS: {
							aps->GetBuildPoint().Save();
							break;
						}
						case IDM_ANIMATION: {
							aps->SwitchAnimation();
							break;
						}
						case IDM_APP_PATH: {
							aps->SwitchShowAppPath();
							break;
						}
						case IDM_MIDI_BIND: {
							aps->SwitchMidiBind();
							break;
						}
						case IDM_PEAK_METER: {
							aps->SwitchPeakMeter();
							break;
						}
						case IDM_CLOSE: {
							aps->Close();
							break;
						}
						default: return ::DefSubclassProc(h, m, w, l);
					}
					break;
				}
				default: return ::DefSubclassProc(h, m, w, l);
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

							UI::Panel& panel = aps->GetPanel();
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
