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
		using namespace std::string_view_literals;

		constexpr std::wstring_view strPanelIndex = L": Audio mixer panel - abort, index elements wrong"sv;
		constexpr std::wstring_view strPanelInit  = L": Audio mixer panel - abort, init failed"sv;
		constexpr std::wstring_view strPanelEnable = L": Audio mixer panel window - status mismatch, next operation cannot be continued."sv;
		constexpr std::wstring_view strPanelNoUiThread = L": Audio mixer panel - no UI thread, abort visual changed"sv;
		constexpr std::wstring_view strPanelNoItemValid = L": Audio mixer panel - bad Item, abort create application control box"sv;

		/* ToolTipData */

		ToolTipData::ToolTipData() : hwnd(nullptr) {}
		ToolTipData::ToolTipData(HWND h, RECT& r, const std::wstring& s) : hwnd(h) {
			rect.left = r.left;
			rect.right = r.right;
			rect.top = r.top;
			rect.bottom = r.bottom;
			title = std::wstring(s.begin(), s.end());
		}
		const bool ToolTipData::IsValid() {
			return (!title.empty()) && (hwnd != nullptr);
		}

		/* PanelData */

		PanelData::PanelData() : style(0U), x(0), y(0), w(0), h(0), mcapture__(false), enable__(true), anime__(false) {
		}
		PanelData::~PanelData() {
			dispose_();
		}
		void PanelData::dispose_() {
			try {
				HWND hwnd = hwnd__.release();
				if (hwnd != nullptr) {
					SUBCLASSPROC proc = proc__.release();
					if (proc != nullptr)
						::RemoveWindowSubclass(hwnd, proc, id__);
					::DestroyWindow(hwnd);
				}
			} catch (...) {}
		}
		void PanelData::refresh_() {
			if (hwnd__ && ::IsGUIThread(true))
				::SetWindowPos(hwnd__.get(), HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
		}

		void PanelData::Close(int32_t attr) {
			if ((attr != -1) && anime__ && hwnd__)
				::AnimateWindow(
					hwnd__.get(),
					(attr == AW_BLEND) ? 300 : 200,
					(attr) ? (AW_HIDE | attr) : (AW_HIDE | AW_CENTER)
				);
			dispose_();
		}
		void PanelData::Show(const bool anime) {
			Show(anime ? 0 : 1);
		}
		void PanelData::Show(int32_t attr) {
			if (anime__ && hwnd__) {
				if (attr == -1) return;
				if (attr == 1) ::ShowWindow(hwnd__.get(), SW_SHOW);
				else ::AnimateWindow(
						hwnd__.get(),
						(attr == AW_BLEND) ? 300 : 150,
						(attr) ? (AW_ACTIVATE | attr) : (AW_ACTIVATE | AW_HOR_POSITIVE)
					 );
			}
		}
		void PanelData::Set(uint32_t s_, int32_t x_, int32_t y_, int32_t w_, int32_t h_) {
			style = s_, x = x_; y = y_; w = w_; h = h_;
		}
		const bool PanelData::Init(HINSTANCE hi, int32_t id) {
			try {
				HWND wnd;
				if ((wnd = ::CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, TOOLTIPS_CLASSW, L" ", style, x, y, w, h, 0, 0, hi, 0)) == nullptr)
					return false;

				id__ = id;
				hwnd__.reset(wnd);
				return true;
			} catch (...) {}
			return false;
		}
		const bool PanelData::Init(HINSTANCE hi, HWND hwnd, int32_t id, int32_t position, SUBCLASSPROC proc, bool animele, void* clz) {
			try {
				int32_t xx = x, yy = y;
				if (common_config::Get().UiThemes.IsPlaceVertical())
					yy += position;
				else
					xx += position;

				if ((style & WS_VISIBLE) == 0) {
					if (!animele) style = (style | WS_VISIBLE);
					else anime__ = true;
				}

				HWND wnd;
				if ((wnd = ::CreateWindowExW(WS_EX_TRANSPARENT | WS_EX_CONTROLPARENT, L"STATIC", L" ", style, xx, yy, w, h, hwnd, 0, hi, 0)) == nullptr)
					return false;

				id__ = id;
				hwnd__.reset(wnd);

				if ((proc != nullptr) && (clz != nullptr)) {
					proc__.reset(proc);
					::SetWindowSubclass(hwnd__.get(), proc__.get(), id, (DWORD_PTR)clz);
				}
				return true;
			} catch (...) {}
			return false;
		}
		HWND PanelData::GetHWND() {
			return hwnd__.get();
		}
		RECT PanelData::GetSize() {
			RECT r{};
			r.left = x;
			r.right = y;
			r.top = w;
			r.bottom = h;
			return r;
		}
		void PanelData::SetSize(RECT& r) {
			x = r.left;
			y = r.right;
			w = r.top;
			h = r.bottom;
			refresh_();
		}
		bool PanelData::SetPosition(bool isv, bool anime) {
			try {
				RECT r{};
				HWND hwnd = hwnd__.get();
				if (!hwnd__ || !::GetWindowRect(hwnd, &r)) return false;
				POINT p{};
				p.x = isv ? 0 : (r.left - (r.right - r.left));
				p.y = isv ? (r.top - (r.bottom - r.top)) : 0;

				uint32_t flags = (anime && anime__) ? (SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW) : (SWP_NOSIZE | SWP_NOZORDER);
				::SetWindowPos(hwnd, HWND_TOP, p.x, p.y, 0, 0, flags);
				if (anime && anime__)
					::AnimateWindow(hwnd, 150, AW_ACTIVATE | AW_VER_POSITIVE);
				return true;
			} catch (...) {}
			return false;
		}
		void PanelData::SetAnimation(bool b) {
			anime__ = b;
		}
		void PanelData::SetCapture(bool iscap) {
			if (hwnd__) {
				if (iscap) {
					mcapture__ = true;
					(void) ::SetCapture(hwnd__.get());
				}
				else if (mcapture__) {
					mcapture__ = !::ReleaseCapture();
				}
			}
		}
		void PanelData::SetEnabled(bool state) {
			if (!hwnd__) {
				enable__ = false;
				return;
			}
			enable__ = state;
			refresh_();
		}
		const bool PanelData::GetEnabled() {
			return enable__.load();
		}

		std::wstring PanelData::to_string() {
			try {
				std::wstringstream ws;
				ws << L"ID:" << id__ << std::boolalpha << L", HWND:" << (bool)(hwnd__) << L", Enable:" << enable__.load()
					<< L", Animation:" << anime__.load() << L", Mouse cap:" << mcapture__.load() << L", SubClass:" << (bool)(proc__)
					<< L", (x:" << x << L",y:" << y << L",w:" << w << L",h:" << h << L"), style:" << style << L";\n";
				return ws.str();
			} catch (...) {}
			return L"";
		}

		/* AudioMixerPanel */

		AudioMixerPanel::AudioMixerPanel(HINSTANCE h, Sprites& sprites) : sprites__(sprites), Theme(common_config::Get().UiThemes.GetTheme()) {
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

			PanelData& kpanel = ctrl__[AudioMixerPanel::ITEMID::KNOB_ID];
			const bool state = kpanel.GetEnabled();
			valueknob_(kpanel, !state, state);

			if (isup) {
				if (isduplicateappremoved__) {
					const std::size_t nameid = Item.GetAppId();
					if (nameid == 0) return;
					worker_background::Get().to_async(
						std::async(std::launch::async, [=](const std::size_t i) {
						MIXER::AudioSessionMixer::Get().Volume.ByAppId.SetVolume(i, u);
					}, nameid));
				} else {
					const GUID guid = Item.GetGUID();
					if (guid == GUID_NULL) return;
					worker_background::Get().to_async(
						std::async(std::launch::async, [=](const GUID g) {
						MIXER::AudioSessionMixer::Get().Volume.ByGuid.SetVolume(g, u);
					}, guid));
				}
			}
		}
		void AudioMixerPanel::valuecb_mute_(bool isup, bool renew, bool m) {
			PanelData& kpanel = ctrl__[AudioMixerPanel::ITEMID::KNOB_ID];
			const bool state = kpanel.GetEnabled();

			if (!renew && ((!state && m) || (state && !m))) return;
			valueknob_(kpanel, m, state);

			if (isup) {
				if (isduplicateappremoved__) {
					const std::size_t nameid = Item.GetAppId();
					if (nameid == 0) return;
					worker_background::Get().to_async(
						std::async(std::launch::async, [=](const std::size_t i) {
						MIXER::AudioSessionMixer::Get().Volume.ByAppId.SetMute(i, m);
					}, nameid));
				} else {
					const GUID guid = Item.GetGUID();
					if (guid == GUID_NULL) return;
					worker_background::Get().to_async(
						std::async(std::launch::async, [=](const GUID g) {
						MIXER::AudioSessionMixer::Get().Volume.ByGuid.SetMute(g, m);
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
		void AudioMixerPanel::valueknob_(PanelData& panel, bool mute, bool state) {
			try {

				if (!::IsGUIThread(true)) {
					to_log::Get() << (log_string() << __FUNCTIONW__ << strPanelNoUiThread);
					#if defined(_DEBUG_AUDIOMIXERPANEL)
					::OutputDebugStringW(strPanelNoUiThread.data());
					#endif
					return;
				}

				if (!state) {
					panel.SetEnabled(true);
					if (!panel.GetEnabled()) {
						to_log::Get() << (log_string() << __FUNCTIONW__ << strPanelEnable);
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
		ToolTipData AudioMixerPanel::GetBalloonData() {
			try {
				do {
					HWND hwnd = ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].GetHWND(),
						 hwnt = ctrl__[AudioMixerPanel::ITEMID::TEXT_ID].GetHWND(),
						 hwnp;

					if ((hwnd == nullptr) || (hwnt == nullptr)) break;
					if ((hwnp = ::GetAncestor(hwnd, GA_PARENT)) == nullptr) break;

					RECT r{};
					if (::GetClientRect(hwnt, &r))
						return ToolTipData(hwnt, r, Item.Item.GetPath());
				} while (0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return ToolTipData();
		}

		void AudioMixerPanel::SetCapture(ITEMID idx, bool iscap) {
			if (!disposed__)
				ctrl__[static_cast<int32_t>(idx)].SetCapture(iscap);
		}
		void AudioMixerPanel::SetKnobValue(int32_t val) {
			if (disposed__) return;
			try {
				PanelData& kpanel = ctrl__[AudioMixerPanel::ITEMID::KNOB_ID];
				if (!kpanel.GetEnabled()) return;
				Item.SetVolume(static_cast<uint8_t>(val));
			} catch (...) {}
		}
		void AudioMixerPanel::SetBtnMute() {
			if (disposed__) return;
			try {
				PanelData& kpanel = ctrl__[AudioMixerPanel::ITEMID::KNOB_ID];
				Item.SetMute(kpanel.GetEnabled());
			} catch (...) {}
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
				::PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_IEVENT_CLEAR, 0), (LPARAM)0);
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
					::PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_IEVENT_CLEAR, 0), (LPARAM)0);
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
					int32_t panelsize = PLACESIZE(isvertical, r);

					for (size_t i = 0; i < _countof(ctrl__); i++) {
						HWND hwnd;
						int32_t pos;
						SUBCLASSPROC proc;

						startid += static_cast<int32_t>(i);
						PanelData& p = ctrl__[i];

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
								throw runtime_werror(Utils::to_string(strPanelIndex));
							}
						}
						if ((hwnd == nullptr) || !p.Init(hi, hwnd, startid, pos, proc, animele, this))
							throw runtime_werror(Utils::to_string(strPanelInit));
					}
					startid += 1;
					position += panelsize;
					isduplicateappremoved__ = isdupremoved;

					const bool anime = item.IsEvent();
					if (!InitItem(std::move(item))) {
						Close();
						to_log::Get() << (log_string() << __FUNCTIONW__ << strPanelNoItemValid);
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
								if (!MIXER::AudioSessionMixer::Get().GetMasterVolumePeak(&f)) break;

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
								if (isnew || !PEAK_clear) {
									
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
				case WM_COMMAND: {
					switch (LOWORD(w)) {
						case IDC_IEVENT_CLEAR: {
							try {
								if ((ap = getProcClass(data)) == nullptr) break;
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
