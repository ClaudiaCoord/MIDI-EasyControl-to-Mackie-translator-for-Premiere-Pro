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

		#if defined(_DEBUG)
		/* #define _DEBUG_AUDIOMIXERPANEL 1 */
		#endif

		#define ID_TIMER 100001
		static bool TIMER_lock = false,
					PEAK_clear = false;

		using namespace std::placeholders;

		/* AudioMixerPanel */

		AudioMixerPanel::AudioMixerPanel(UI::Sprites& sprites) : sprites__(sprites), Theme(common_config::Get().UiThemes.GetTheme()) {
			ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].Set(
				WS_CHILD | WS_CLIPSIBLINGS | BS_FLAT,
				0, 0, 84, 94
			);
			ctrl__[AudioMixerPanel::ITEMID::KNOB_ID].Set(
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_FLAT | WS_TABSTOP | SS_NOTIFY | SS_BITMAP,
				10, 4, 64, 64
			);
			ctrl__[AudioMixerPanel::ITEMID::TEXT_ID].Set(
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_FLAT | SS_NOPREFIX | SS_NOTIFY | SS_CENTER | SS_WORDELLIPSIS | SS_OWNERDRAW,
				1, 70, 82, 22
			);
		}
		AudioMixerPanel::~AudioMixerPanel() {
			dispose_();
		}

		void AudioMixerPanel::dispose_(int32_t closeanim) {
			try {
				disposed__ = true;
				if (showpeaklevel__) {
					HWND hwnd = ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND();
					::KillTimer(hwnd, ID_TIMER);
				}
				Item.Item.App.set_onchangecb_defsault();
				Item.Item.Volume.set_onchangecb_defsault();

				for (size_t i = 0; i < _countof(ctrl__); i++)
					ctrl__[i].Close(closeanim);
			} catch (...) {}
		}

		void AudioMixerPanel::valuecb_volume_(bool isup, bool renew, uint8_t u) {
			int32_t idx = Item.Item.Volume.sprite_index();
			if (!renew && (knob_idx__.load() == idx)) return;
			knob_idx__ = idx;

			UI::Panel& kpanel = ctrl__[AudioMixerPanel::ITEMID::KNOB_ID];
			const bool state = kpanel.GetEnabled();
			valueknob_(kpanel, !state, state);

			if (isup) {
				if (isduplicateappremoved__) {
					const std::size_t nameid = Item.GetAppId();
					if (nameid == 0) return;
					worker_background::Get().to_async(
						std::async(std::launch::async, [=](const std::size_t i) {
						ClassStorage& st = ClassStorage::Get();
						if (st.IsDialog<MIXER::AudioSessionMixer>())
							st.GetDialog<MIXER::AudioSessionMixer>()->Volume.ByAppId.SetVolume(i, u);
					}, nameid));
				} else {
					const GUID guid = Item.GetGUID();
					if (guid == GUID_NULL) return;
					worker_background::Get().to_async(
						std::async(std::launch::async, [=](const GUID g) {
						ClassStorage& st = ClassStorage::Get();
						if (st.IsDialog<MIXER::AudioSessionMixer>())
							st.GetDialog<MIXER::AudioSessionMixer>()->Volume.ByGuid.SetVolume(g, u);
					}, guid));
				}
			}
		}
		void AudioMixerPanel::valuecb_mute_(bool isup, bool renew, bool m) {
			UI::Panel& kpanel = ctrl__[AudioMixerPanel::ITEMID::KNOB_ID];
			const bool state = kpanel.GetEnabled();

			if (!renew && ((!state && m) || (state && !m))) return;
			valueknob_(kpanel, m, state);

			if (isup) {
				if (isduplicateappremoved__) {
					const std::size_t nameid = Item.GetAppId();
					if (nameid == 0) return;
					worker_background::Get().to_async(
						std::async(std::launch::async, [=](const std::size_t i) {
						ClassStorage& st = ClassStorage::Get();
						if (st.IsDialog<MIXER::AudioSessionMixer>())
							st.GetDialog<MIXER::AudioSessionMixer>()->Volume.ByAppId.SetMute(i, m);
					}, nameid));
				} else {
					const GUID guid = Item.GetGUID();
					if (guid == GUID_NULL) return;
					worker_background::Get().to_async(
						std::async(std::launch::async, [=](const GUID g) {
						ClassStorage& st = ClassStorage::Get();
						if (st.IsDialog<MIXER::AudioSessionMixer>())
							st.GetDialog<MIXER::AudioSessionMixer>()->Volume.ByGuid.SetMute(g, m);
					}, guid));
				}
			}
		}
		void AudioMixerPanel::valuecb_(bool isup, MIXER::OnChangeType t, uint8_t u, float f, bool m) {
			try {

				#if defined(_DEBUG_AUDIOMIXERPANEL)
				log_string ls{};
				ls  << L"* UpdateItem + value CB = " << Item.to_string().str()
					<< L"\t\t{ callcb=" << std::boolalpha << isup
					<< L", type=" << MIXER::AudioSessionHelper::OnChangeTypeHelper(t)
					<< L", volume(u)=" << (uint32_t)u << L", volume(f)=" << f << L", mute=" << m << L" }\n\n";

				::OutputDebugStringW(ls.str().c_str());
				#endif

				switch (t) {
					case MIXER::OnChangeType::OnChangeUpdateAllValues: {
						valuecb_mute_(isup ? isup : Item.IsCallAudioCb(), true, m);
						valuecb_volume_(isup ? isup : Item.IsCallAudioCb(), true, u);
						break;
					}
					case MIXER::OnChangeType::OnChangeUpdateMute: {
						valuecb_mute_(isup, false, m);
						break;
					}
					case MIXER::OnChangeType::OnChangeUpdateVolume: {
						valuecb_volume_(isup, false, u);
						break;
					}
					default: break;
				}
			} catch(...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void AudioMixerPanel::valueknob_(UI::Panel& panel, bool mute, bool state) {
			try {

				if (!::IsGUIThread(true)) {
					to_log::Get() << log_string().to_log_string(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_PANEL_NOT_UI_THREAD)
					);
					#if defined(_DEBUG_AUDIOMIXERPANEL)
					::OutputDebugStringW(strPanelNoUiThread.data());
					#endif
					return;
				}

				if (!state) {
					panel.SetEnabled(true);
					if (!panel.GetEnabled()) {
						to_log::Get() << log_string().to_log_string(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_PANEL_ENABLE)
						);
						#if defined(_DEBUG_AUDIOMIXERPANEL)
						::OutputDebugStringW(panel.to_string().c_str());
						#endif
						return;
					}
				}

				int32_t idx = knob_idx__.load();
				HBITMAP hbmp = mute ?
					sprites__.sprites_disabled.get(idx) :
					sprites__.sprites_enabled.get(idx);
				panel.SetData(hbmp);
				if (mute) panel.SetEnabled(false);
				else	  panel.SetEnabled(true);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void AudioMixerPanel::datacb_(std::wstring& name, std::wstring&, std::wstring&, std::wstring&, GUID&, uint32_t) {
			try {
				if (!name.empty())
					ctrl__[AudioMixerPanel::ITEMID::TEXT_ID].SetData(name);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		HWND AudioMixerPanel::GetHWND(ITEMID idx) {
			return ctrl__[static_cast<int>(idx)].GetHWND();
		}
		RECT AudioMixerPanel::GetSize(ITEMID idx) {
			return ctrl__[static_cast<int>(idx)].GetSize();
		}
		void AudioMixerPanel::SetSize(RECT& r, ITEMID idx) {
			if (!disposed__)
				ctrl__[static_cast<int>(idx)].SetSize(r);
		}
		const bool AudioMixerPanel::SetPositionUp(bool isv, bool anime) {
			if (!disposed__)
				return ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].SetPosition(isv, anime);
			return false;
		}
		HBITMAP AudioMixerPanel::GetCurrentSprite() {
			try {
				return ctrl__[AudioMixerPanel::ITEMID::KNOB_ID].GetEnabled() ?
					sprites__.sprites_enabled.get(knob_idx__) :
					sprites__.sprites_disabled.get(knob_idx__);
			} catch (...) {}
			return sprites__.sprites_disabled.get(0);
		}
		MIXER::AudioSessionItemChange& AudioMixerPanel::GetAudioItem() {
			return std::ref(Item);
		}
		UI::ToolTipData AudioMixerPanel::GetBalloonData() {
			try {
				do {
					HWND hwnd = ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND(),
						 hwnt = ctrl__[AudioMixerPanel::ITEMID::TEXT_ID].GetHWND(),
						 hwnp;

					if ((hwnd == nullptr) || (hwnt == nullptr)) break;
					if ((hwnp = ::GetAncestor(hwnd, GA_PARENT)) == nullptr) break;

					RECT r{};
					if (::GetClientRect(hwnt, &r))
						return UI::ToolTipData(hwnt, r, Item.Item.GetPath());
				} while (0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return UI::ToolTipData();
		}

		void AudioMixerPanel::SetCapture(ITEMID idx, bool iscap) {
			if (!disposed__)
				ctrl__[static_cast<int32_t>(idx)].SetCapture(iscap);
		}
		void AudioMixerPanel::SetKnobValue(int32_t val) {
			if (disposed__) return;
			try {
				UI::Panel& kpanel = ctrl__[AudioMixerPanel::ITEMID::KNOB_ID];
				if (!kpanel.GetEnabled()) return;
				Item.SetVolume(static_cast<uint8_t>(val));
			} catch (...) {}
		}
		void AudioMixerPanel::SetBtnMute() {
			if (disposed__) return;
			try {
				UI::Panel& kpanel = ctrl__[AudioMixerPanel::ITEMID::KNOB_ID];
				Item.SetMute(kpanel.GetEnabled());
			} catch (...) {}
		}
		void AudioMixerPanel::SetVisableStatus(bool state) {
			statevisable__ = state;
		}

		void AudioMixerPanel::ShowAnimation(bool b) {
			if (disposed__) return;
			ctrl__[0].SetAnimation(b);
		}
		void AudioMixerPanel::ShowMidiKeyBind(bool b) {
			if (disposed__) return;
			try {
				showmidikeybind__ = b;
				HWND hwnd = ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND();
				if (hwnd == nullptr) return;
				::PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(DLG_AMIX_EVENT_CLEAR, 0), (LPARAM)0);
			} catch (...) {}
		}
		void AudioMixerPanel::ShowPeakMeter(bool b) {
			if (disposed__) return;
			try {
				HWND hwnd = ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND();
				if (hwnd == nullptr) return;
				
				if (b) {
					::SetTimer(hwnd, ID_TIMER, 200, 0);
				} else {
					::KillTimer(hwnd, ID_TIMER);
					::PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(DLG_AMIX_EVENT_CLEAR, 0), (LPARAM)0);
				}
			} catch (...) {}
		}

		const std::size_t AudioMixerPanel::GetAppId() {
			return Item.GetAppId();
		}
		const bool AudioMixerPanel::IsAppValid() {
			return (ctrl__[0].GetHWND() != nullptr) && Item.IsValid();
		}
		const bool AudioMixerPanel::IsMaster() {
			return (Item.Item.App.Pid == 0) && (Item.Item.App.get<std::size_t>() == MIXER::AudioSessionMixer::MasterControl.GetId());
		}

		bool AudioMixerPanel::InitItem(MIXER::AudioSessionItemChange item) {
			try {
				if (!item.IsValid()) return false;
				Item.Copy(item);
				Item.Item.App.set_onchangecb(
					std::bind(
						static_cast<void(AudioMixerPanel::*)(std::wstring&, std::wstring&, std::wstring&, std::wstring&, GUID&, uint32_t)>(&AudioMixerPanel::datacb_),
						this, _1, _2, _3, _4, _5, _6
					)
				);
				Item.Item.Volume.set_onchangecb(
					std::bind(
						static_cast<void(AudioMixerPanel::*)(bool, MIXER::OnChangeType, uint8_t, float, bool)>(&AudioMixerPanel::valuecb_),
						this, _1, _2, _3, _4, _5
					),
					true
				);
				#if defined(_DEBUG_AUDIOMIXERPANEL)
				log_string ls{};
				ls << L"* InitItem = " << Item.to_string().str() << L"\n";
				::OutputDebugStringW(ls.str().c_str());
				#endif
				return true;
			} catch (...) {}
			return false;
		}
		void AudioMixerPanel::UpdateItem(MIXER::AudioSessionItemChange& item) {
			if (disposed__) return;
			try {
				if (!item.IsUpdateValid()) return;
				Item.Update(item);

				#if defined(_DEBUG_AUDIOMIXERPANEL)
				log_string ls{};
				ls << L"* UpdateItem = " << Item.to_string().str() << L"\n";
				::OutputDebugStringW(ls.str().c_str());
				#endif

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void AudioMixerPanel::Close() {
			dispose_(0);
		}
		AudioMixerPanel* AudioMixerPanel::Open(
			HINSTANCE hi, HWND h, MIXER::AudioSessionItemChange item, ui_theme& theme,
			std::atomic<int32_t>& startid, std::atomic<int32_t>& position, bool isvertical, bool animele, bool isdupremoved) {

			try {
				do {
					if ((!item.IsValid()) || (!::IsGUIThread(true))) return nullptr;

					Theme = theme;
					RECT r = GetSize();
					int32_t panelsize = UI::PLACESIZE(isvertical, r);

					for (size_t i = 0; i < _countof(ctrl__); i++) {
						HWND hwnd;
						int32_t pos;
						SUBCLASSPROC proc;

						startid += static_cast<int32_t>(i);
						UI::Panel& p = ctrl__[i];

						switch (i) {
							case 0: {
								pos = position.load();
								hwnd = h;
								proc = (SUBCLASSPROC)&AudioMixerPanel::EventPANEL_ID;
								break;
							}
							case 1: {
								pos = 0;
								hwnd = ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND();
								proc = (SUBCLASSPROC)&AudioMixerPanel::EventKNOB_ID;
								break;
							}
							case 2: {
								pos = 0;
								hwnd = ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND();
								proc = (SUBCLASSPROC)&AudioMixerPanel::EventTEXT_ID;
								break;
							}
							default: {
								throw make_common_error(common_error_id::err_PANEL_INDEX);
							}
						}
						if ((hwnd == nullptr) || !p.Init(hi, hwnd, startid, pos, proc, animele, this))
							throw make_common_error(common_error_id::err_PANEL_INIT);
					}
					startid += 1;
					position += panelsize;
					isduplicateappremoved__ = isdupremoved;

					const bool anime = item.IsEvent();
					if (!InitItem(std::move(item))) {
						Close();
						to_log::Get() << log_string().to_log_string(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_PANEL_NOT_ITEM_VALID)
						);
						#if defined(_DEBUG_AUDIOMIXERPANEL)
						::OutputDebugStringW(strPanelNoItemValid.data());
						::OutputDebugStringW(item.to_string().str().c_str());
						#endif
						return nullptr;
					}
					ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].Show(anime);
					return this;
				} while (0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return nullptr;
		}

		/* STATIC */

		static AudioMixerPanel* getProcClass(DWORD_PTR data) {
			try {
				return (AudioMixerPanel*)reinterpret_cast<AudioMixerPanel*>(data);
			} catch (...) { return nullptr; }
		}

		LRESULT CALLBACK AudioMixerPanel::EventPANEL_ID(HWND hwnd, UINT m, WPARAM w, LPARAM l, UINT_PTR sc, DWORD_PTR data) {
			AudioMixerPanel* ap;
			switch (m) {
				case WM_PAINT: {
					if ((ap = getProcClass(data)) == nullptr) return ::DefSubclassProc(hwnd, m, w, l);

					PAINTSTRUCT ps{};
					HDC hdc = BeginPaint(hwnd, &ps);
					::SetBkMode(hdc, TRANSPARENT);
					::SetBkColor(hdc, ap->Theme.PanelBackground);
					::SetTextColor(hdc, ap->Theme.Text);

					if (ap->showmidikeybind__.load()) {
						COLORREF clr = ap->Item.Item.IsEmptyId() ? RGB(250, 0, 0) : RGB(39, 141, 7);
						HPEN pen1 = ::CreatePen(PS_SOLID, 3, clr),
							pen2 = (HPEN)::SelectObject(hdc, pen1);
						::Ellipse(hdc, 74, 4, 78, 8);
						::DeleteObject(SelectObject(hdc, pen2));
					}

					::EndPaint(hwnd, &ps);
					break;
				}
				case WM_PRINTCLIENT:
				case WM_ERASEBKGND: {
					if ((ap = getProcClass(data)) == nullptr) return ::DefSubclassProc(hwnd, m, w, l);

					HDC hdc = (HDC)(w);
					RECT rc; GetClientRect(hwnd, &rc);
					FillRect(hdc, &rc, ap->Theme.GetBackgroundBrush());
					break;
				}
				case WM_CTLCOLORSTATIC: {
					if ((ap = getProcClass(data)) == nullptr) return ::DefSubclassProc(hwnd, m, w, l);
					return reinterpret_cast<LRESULT>(ap->Theme.GetBackgroundBrush());
				}
				case WM_DRAWITEM: {
					(void)AudioMixerPanel::EventTEXT_ID(hwnd, m, w, l, sc, data);
					return ::DefSubclassProc(hwnd, m, w, l);
				}
				case WM_TIMER: {
					if ((int)w == ID_TIMER) {

						if (TIMER_lock) break;
						TIMER_lock = true;
						do {
							try {
								if ((ap = getProcClass(data)) == nullptr) break;
								float f{};
								ClassStorage& st = ClassStorage::Get();
								if (!st.IsDialog<MIXER::AudioSessionMixer>()) break;
								if (!st.GetDialog<MIXER::AudioSessionMixer>()->GetMasterVolumePeak(&f)) break;

								#if defined (_DEBUG_DESIGN)
								Gdiplus::Graphics g(GetDC(hwnd));
								g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

								Gdiplus::Rect r{};
								r.X = 18;
								r.Y = 12;
								r.Width = 47;
								r.Height = 45;
								Pen pen1(Color(255, 90, 226, 62), 3);
								//Pen pen1(Color(255, 91, 199, 72), 3);
								Pen pen2(Color(255, 55, 164, 35), 4);
								//Pen pen2(Color(255, 6, 83, 6), 4);

								float start = 146,
									  end   = 180;
								g.DrawArc(&pen2, r, start, 242);
								g.DrawArc(&pen1, r, start, end);
								#else

								const bool isnew = (f > 0.0f);
								if (ap->statevisable__ && (isnew || !PEAK_clear)) {
									
									Gdiplus::Graphics g(GetDC(hwnd));
									g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

									Gdiplus::Rect r = ap->Theme.PeakMeter.GetRect();
									Gdiplus::Pen penbg(ap->Theme.PeakMeter.GetBackgroundColor(), 4);

									if (isnew) {
										Gdiplus::Pen penfg(ap->Theme.PeakMeter.GetForegroundColor(), 3);
										g.DrawArc(&penbg, r, ap->Theme.PeakMeter.GetStartAngle(), ap->Theme.PeakMeter.GetEndAngle());
										PEAK_clear = static_cast<bool>(g.DrawArc(&penfg, r, 146, static_cast<Gdiplus::REAL>(ap->Theme.PeakMeter.GetVolumeAngle(f))));
									}
									else if (!PEAK_clear)
										PEAK_clear = (g.DrawArc(&penbg, r, ap->Theme.PeakMeter.GetStartAngle(), ap->Theme.PeakMeter.GetEndAngle()) == 0);
								}
								#endif

							} catch (...) {}
						} while (0);
						TIMER_lock = false;
					}
					break;
				}
				case WM_SETFOCUS: {
					try {
						if ((ap = getProcClass(data)) == nullptr) break;
						ap->statevisable__ = true;
					} catch (...) {}
					break;
				}
				case WM_COMMAND: {
					switch (LOWORD(w)) {
						case DLG_AMIX_EVENT_CLEAR: {
							try {
								if ((ap = getProcClass(data)) == nullptr) break;
								if (!ap->statevisable__) break;
								Gdiplus::Graphics g(GetDC(hwnd));
								g.Clear(ap->Theme.GetBackgroundGdi());
								::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
							} catch (...) {}
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
		LRESULT CALLBACK AudioMixerPanel::EventKNOB_ID(HWND hwnd,  UINT m, WPARAM w, LPARAM l, UINT_PTR sc, DWORD_PTR data) {
			AudioMixerPanel* ap;
			switch (m) {
				case WM_RBUTTONUP:
				case WM_MBUTTONUP:
				case WM_LBUTTONUP:
				case WM_XBUTTONUP:
				case WM_LBUTTONDOWN: {
					if ((ap = getProcClass(data)) == nullptr) return ::DefSubclassProc(hwnd, m, w, l);
					switch (m) {
						case WM_RBUTTONUP:   ap->SetBtnMute(); break;
						case WM_MBUTTONUP:   ap->SetKnobValue(65); break;
						case WM_XBUTTONUP:	 (GET_XBUTTON_WPARAM(w) == XBUTTON1) ? ap->SetKnobValue(25) : ap->SetKnobValue(75); break;
						case WM_LBUTTONUP:   ap->SetCapture(AudioMixerPanel::ITEMID::KNOB_ID, false); break;
						case WM_LBUTTONDOWN: ap->SetCapture(AudioMixerPanel::ITEMID::KNOB_ID, true); break;
						default: return ::DefSubclassProc(hwnd, m, w, l);
					}
					break;
				}
				case WM_MOUSEMOVE: {
					switch (w) {
						case MK_LBUTTON: {
							if ((ap = getProcClass(data)) == nullptr) return ::DefSubclassProc(hwnd, m, w, l);
							POINTS p = MAKEPOINTS(l);
							int32_t val = static_cast<int32_t>(std::abs(p.x * 2.007));
							val = (val < 0) ? 0 : ((val > 127) ? 127 : val);
							ap->SetKnobValue(val);
							break;
						}
						default: return ::DefSubclassProc(hwnd, m, w, l);
					}
					break;
				}
				case WM_MOUSEWHEEL: {
					if ((ap = getProcClass(data)) == nullptr) return ::DefSubclassProc(hwnd, m, w, l);

					bool dir = (GET_WHEEL_DELTA_WPARAM(w) > 0);
					int32_t val = static_cast<int32_t>(std::abs(ap->GetAudioItem().GetVolume() * 127));
					val = dir ? (val + 10) : (val - 10);
					val = (val < 0) ? 0 : ((val > 127) ? 127 : val);
					ap->SetKnobValue(val);
					break;
				}
				default: return ::DefSubclassProc(hwnd, m, w, l);
			}
			return TRUE;
		}
		LRESULT CALLBACK AudioMixerPanel::EventTEXT_ID(HWND hwnd,  UINT m, WPARAM w, LPARAM l, UINT_PTR sc, DWORD_PTR data) {
			AudioMixerPanel* ap;
			switch (m) {
				case WM_DRAWITEM: {
					try {
						if ((ap = getProcClass(data)) == nullptr) return ::DefSubclassProc(hwnd, m, w, l);

						LPDRAWITEMSTRUCT pd = (LPDRAWITEMSTRUCT)l;
						HFONT hNF = ::CreateFontW(
							15, 0, 0, 0,
							FW_NORMAL, 0, 0, 0,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
							L"Segoe UI");
						HFONT hOF = (HFONT)SelectObject(pd->hDC, hNF);

						(void) ::SetTextColor(pd->hDC, ap->Theme.Text);
						(void) ::SetBkColor(pd->hDC, ap->Theme.PanelBackground);
						wchar_t txt[100]{};
						long len = static_cast<long>(::SendMessageW(pd->hwndItem, WM_GETTEXT, ARRAYSIZE(txt), (LPARAM)txt));
						::DrawTextW(pd->hDC, txt, len, &pd->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						::FrameRect(pd->hDC, &pd->rcItem, ap->Theme.GetBorderBrush());

						::SelectObject(pd->hDC, hOF);
						::DeleteObject(hNF);
						break;
					} catch (...) {}
					return FALSE;
				}
				default: return ::DefSubclassProc(hwnd, m, w, l);
			}
			return TRUE;
		}
	}
}
