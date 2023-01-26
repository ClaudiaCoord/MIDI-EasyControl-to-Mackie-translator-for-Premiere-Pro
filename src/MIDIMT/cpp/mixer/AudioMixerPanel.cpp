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

		using namespace std::string_view_literals;

		constexpr std::wstring_view strPanelIndex = L": Audio mixer panel - abort, index elements wrong"sv;
		constexpr std::wstring_view strPanelInit  = L": Audio mixer panel - abort, init failed"sv;

		/* PanelData */

		PanelData::PanelData() : style(0U), x(0), y(0), w(0), h(0), mcapture__(false), isenable__(true) {
		}
		PanelData::~PanelData() {
			dispose_();
		}
		void PanelData::dispose_() {
			try {
				HWND hwnd = hwnd__;
				hwnd__ = nullptr;
				if (hwnd != nullptr) {
					if (proc__ != nullptr)
						::RemoveWindowSubclass(hwnd, proc__, id__);
					::DestroyWindow(hwnd);
				}
			} catch (...) {}
		}
		void PanelData::refresh_() {
			if ((hwnd__ != nullptr) && ::IsGUIThread(true))
				::SetWindowPos(hwnd__, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
			/*::InvalidateRect(hwnd__, 0, true);*/
		}

		void PanelData::Close() {
			dispose_();
		}
		void PanelData::Set(uint32_t s_, int32_t x_, int32_t y_, int32_t w_, int32_t h_) {
			style = s_, x = x_; y = y_; w = w_; h = h_;
		}
		const bool PanelData::Init(HINSTANCE hi, HWND hwnd, int32_t id, int32_t position, SUBCLASSPROC proc, void* clz) {

			int32_t xx = x, yy = y;
			if (common_config::Get().UiThemes.IsPlaceVertical())
				yy += position;
			else
				xx += position;

			if ((hwnd__ = ::CreateWindowExW(WS_EX_TRANSPARENT | WS_EX_CONTROLPARENT, L"STATIC", L" ", style, xx, yy, w, h, hwnd, 0, hi, 0)) == nullptr)
				return false;
			id__ = id;
			if (proc != nullptr) {
				proc__ = proc;
				::SetWindowSubclass(hwnd__, proc__, id, (DWORD_PTR)clz);
			}
			return true;
		}
		HWND PanelData::GetHWND() {
			return hwnd__;
		}
		[[maybe_unused]] int32_t PanelData::GetId() {
			return id__;
		}
		RECT PanelData::GetSize() {
			RECT r;
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
		void PanelData::SetCapture(bool iscap) {
			if (hwnd__ != nullptr) {
				if (iscap) {
					mcapture__ = true;
					(void) ::SetCapture(hwnd__);
				}
				else if (mcapture__) {
					mcapture__ = !::ReleaseCapture();
				}
			}
		}
		void PanelData::SetEnabled(bool state) {
			if (hwnd__ == nullptr) {
				isenable__ = false;
				return;
			}
			isenable__ = state;
			refresh_();
		}
		const bool PanelData::GetEnabled() {
			return isenable__.load();
		}

		/* AudioMixerPanel */

		AudioMixerPanel::AudioMixerPanel(HINSTANCE h, Sprites& sprites) : sprites__(sprites), Theme(common_config::Get().UiThemes.GetTheme()) {
			ctrl__[AudioMixerPanel::ITEMID::PANEL_ID].Set(
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_FLAT,
				0, 0, 84, 94
			);
			ctrl__[AudioMixerPanel::ITEMID::KNOB_ID].Set(
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_FLAT | WS_TABSTOP | SS_NOTIFY | SS_BITMAP,
				10, 4, 64, 64
			);
			ctrl__[AudioMixerPanel::ITEMID::TEXT_ID].Set(
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_FLAT | SS_NOPREFIX | SS_CENTER | SS_WORDELLIPSIS | SS_OWNERDRAW,
				1, 70, 82, 22
			);
		}
		AudioMixerPanel::~AudioMixerPanel() {
			dispose_();
		}

		void AudioMixerPanel::dispose_() {
			try {
				isdesposed__ = true;
				for (size_t i = 0; i < _countof(ctrl__); i++)
					ctrl__[i].Close();
			} catch (...) {}
		}

		void AudioMixerPanel::setknobctrl_(PanelData& panel, bool mute, bool state) {
			if (!state) panel.SetEnabled(true);

			int32_t idx = knob_idx__.load();
			HBITMAP hbmp = mute ?
				sprites__.sprites_disabled.get(idx) :
				sprites__.sprites_enabled.get(idx);
			panel.SetData(hbmp);
			if (mute) panel.SetEnabled(false);
			else	  panel.SetEnabled(true);
		}
		void AudioMixerPanel::setctrl_(bool mute, MIXER::OnChangeType type) {
			try {
				PanelData& kpanel = ctrl__[AudioMixerPanel::ITEMID::KNOB_ID];
				bool state = kpanel.GetEnabled();

				switch (type) {
					case MIXER::OnChangeType::OnChangeUpdateAllValues: {
						setknobctrl_(kpanel, mute, state);
						break;
					}
					case MIXER::OnChangeType::OnChangeUpdateVolume: {
						setknobctrl_(kpanel, !state, state);
						break;
					}
					case MIXER::OnChangeType::OnChangeUpdateMute: {
						if ((!state && mute) || (state && !mute)) break;
						setknobctrl_(kpanel, mute, state);
						break;
					}
					default: return;
				}
			} catch (...) {}
		}
		void AudioMixerPanel::updateaudiosessionvalue_(bool isrenew) {
			try {
				if (!::IsGUIThread(true)) return;

				int32_t kidx = static_cast<int32_t>(std::abs(30 - (Item.Volume * 30)));
				kidx = (kidx < 0) ? 0 : ((kidx > 30) ? 30 : kidx);
				bool mute = (Item.IsKeyFound(MIXER::AudioAction::AUDIO_MUTE) && Item.Mute);

				PanelData& kpanel = ctrl__[AudioMixerPanel::ITEMID::KNOB_ID];
				if (!isrenew && (knob_idx__.load() == kidx) && (mute == !kpanel.GetEnabled())) return;
				knob_idx__ = kidx;
				setctrl_(mute, MIXER::OnChangeType::OnChangeUpdateAllValues);

				if (isrenew)
					ctrl__[AudioMixerPanel::ITEMID::TEXT_ID].SetData(Item.Name);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		const bool AudioMixerPanel::setaudiosessionitem_(MIXER::AudioSessionItemChange& item) {
			if (!item.IsValid()) return false;
			Item.Copy(item);
			return true;
		}

		HWND AudioMixerPanel::GetHWND(ITEMID idx) {
			return ctrl__[static_cast<int>(idx)].GetHWND();
		}
		[[maybe_unused]] int32_t AudioMixerPanel::GetId(ITEMID idx) {
			return ctrl__[static_cast<int>(idx)].GetId();
		}
		RECT AudioMixerPanel::GetSize(ITEMID idx) {
			return ctrl__[static_cast<int>(idx)].GetSize();
		}
		void AudioMixerPanel::SetSize(RECT& r, ITEMID idx) {
			if (!isdesposed__)
				ctrl__[static_cast<int>(idx)].SetSize(r);
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

		void AudioMixerPanel::SetCapture(ITEMID idx, bool iscap) {
			if (!isdesposed__)
				ctrl__[static_cast<int>(idx)].SetCapture(iscap);
		}
		void AudioMixerPanel::SetKnobValue(int32_t val) {
			if (isdesposed__) return;
			try {
				PanelData& kpanel = ctrl__[AudioMixerPanel::ITEMID::KNOB_ID];
				if (!kpanel.GetEnabled()) return;

				#pragma warning( push )
				#pragma warning( disable : 4244 )
				int32_t kidx = std::abs((127 - val) / 4.1); /* realy 4.2 */
				#pragma warning( pop )
				kidx = (kidx < 0) ? 0 : ((kidx > 30) ? 30 : kidx);

				if (knob_idx__.load() == kidx) return;
				knob_idx__ = kidx;
				setctrl_(false, MIXER::OnChangeType::OnChangeUpdateVolume);

				worker_background::Get().to_async(
					std::async(std::launch::async, [=]() {
						uint8_t vol = static_cast<uint8_t>((val > 127) ? 127 : ((val < 0) ? 0 : val));
						MIXER::AudioSessionMixer::Get().SetVolume(Item.Data[0].Id, vol);
					}
				));
			} catch (...) {}
		}
		void AudioMixerPanel::SetBtnMute() {
			if (isdesposed__) return;
			try {
				PanelData& kpanel = ctrl__[AudioMixerPanel::ITEMID::KNOB_ID];
				bool mute = kpanel.GetEnabled();
				setctrl_(mute, MIXER::OnChangeType::OnChangeUpdateMute);

				if (Item.Data[2].Id > 0U)
					worker_background::Get().to_async(
						std::async(std::launch::async, [=]() {
							MIXER::AudioSessionMixer::Get().SetMute(Item.Data[2].Id, mute);
						}
					));
			} catch (...) {}
		}
		[[maybe_unused]] bool AudioMixerPanel::IsValid() {
			return ctrl__[0].GetHWND() != nullptr;
		}

		void AudioMixerPanel::UpdateCtrlValue(MIXER::AudioSessionItemChange& item) {
			if (isdesposed__) return;
			try {
				do {
					if (!item.IsValid()) break;
					bool notequals = true;
					if (Item.Volume != item.Volume) {
						Item.Volume  = item.Volume;
						notequals    = false;
					}
					if (Item.Mute != item.Mute) {
						Item.Mute  = item.Mute;
						notequals  = false;
					}
					if (notequals) break;
					updateaudiosessionvalue_();
				} while (0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void AudioMixerPanel::UpdateCtrlData(MIXER::AudioSessionItemChange& item) {
			if (isdesposed__) return;
			try {
				if (!setaudiosessionitem_(item)) return;
				updateaudiosessionvalue_(true);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void AudioMixerPanel::Close() {
			dispose_();
		}
		AudioMixerPanel* AudioMixerPanel::Open(
			HINSTANCE hi, HWND h, MIXER::AudioSessionItemChange item, ui_theme& theme,
			std::atomic<int32_t>& startid, std::atomic<int32_t>& position) {

			try {
				do {
					if ((!item.IsValid()) || (!::IsGUIThread(true))) return nullptr;

					Theme = theme;
					RECT r = GetSize();
					int32_t panelsize = (r.bottom + r.right);

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
								proc = (SUBCLASSPROC)AudioMixerPanel::EventPANEL_ID;
								break;
							}
							case 1: {
								pos = 0;
								hwnd = ctrl__[0].GetHWND();
								proc = (SUBCLASSPROC)AudioMixerPanel::EventKNOB_ID;
								break;
							}
							case 2: {
								pos = 0;
								hwnd = ctrl__[0].GetHWND();
								proc = (SUBCLASSPROC)AudioMixerPanel::EventTEXT_ID;
								break;
							}
							default: {
								throw runtime_werror(Utils::to_string(strPanelIndex));
							}
						}
						if ((hwnd == nullptr) || !p.Init(hi, hwnd, startid, pos, proc, this))
							throw runtime_werror(Utils::to_string(strPanelInit));
					}
					startid += 1;
					position += panelsize;
					UpdateCtrlData(std::ref(item));

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
					::EndPaint(hwnd, &ps);
					break;
				}
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
					int32_t val = static_cast<int32_t>(std::abs(ap->GetAudioItem().Volume * 127));
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
