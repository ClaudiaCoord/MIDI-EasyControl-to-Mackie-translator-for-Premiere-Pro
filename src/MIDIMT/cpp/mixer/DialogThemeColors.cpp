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
#include <Commdlg.h>

namespace Common {
	namespace MIDIMT {


		COLORREF DialogThemeColors::customcolors_[16]{};

		DialogThemeColors::~DialogThemeColors() {
			dispose_();
		}

		void DialogThemeColors::dispose_() {
			try {
				isload_ = false;

				if (ischanged_.load()) {
					worker_background::Get().to_async(std::async(std::launch::async, [=]() {
						common_config& cnf = common_config::Get();
						for (uint16_t i = 0; i < _countof(brushs_); i++) {
							LOGBRUSH lbr{};
							if (::GetObjectW(brushs_[i].get(), sizeof(lbr), &lbr)) {
								switch (i) {
									case 0: cnf.UiThemes.SetCustomThemeColorText(lbr.lbColor); break;
									case 1: cnf.UiThemes.SetCustomThemeColorBackground(lbr.lbColor); break;
									case 2: cnf.UiThemes.SetCustomThemeColorBorder(lbr.lbColor); break;
									default: break;
								}
							}
							brushs_[i].reset();
							ctrls_[i].reset();
						}
						if (hwndp_)
							::PostMessageW(hwndp_, WM_COMMAND, MAKEWPARAM(DLG_COLOR_RELOAD, 0), 0);
					}));
				} else {
					for (uint16_t i = 0; i < _countof(brushs_); i++) {
						brushs_[i].reset();
						ctrls_[i].reset();
					}
				}
				hwndp_.reset();
				hwnd_.reset();
			} catch (...) {}
			isload_ = ischanged_ = false;
		}
		void DialogThemeColors::init_() {
			try {
				ctrls_[0].reset(::GetDlgItem(hwnd_, DLG_COLOR_BOX1));
				ctrls_[1].reset(::GetDlgItem(hwnd_, DLG_COLOR_BOX2));
				ctrls_[2].reset(::GetDlgItem(hwnd_, DLG_COLOR_BOX3));

				common_config& cnf = common_config::Get();
				change_theme_(cnf.UiThemes.GetTheme(ui_themes::ThemeId::Custom));

				uint16_t idx = static_cast<uint16_t>(cnf.UiThemes.GetCustomThemeId());
				switch (idx) {
					case 0:  idx = DLG_COLOR_RADIO_LIGHT;  break;
					case 1:  idx = DLG_COLOR_RADIO_DARK;   break;
					case 2:  idx = DLG_COLOR_RADIO_METRO;  break;
					case 3:  idx = DLG_COLOR_RADIO_MODERN; break;
					case 4:  idx = DLG_COLOR_RADIO_RETRO;  break;
					default: idx = DLG_COLOR_RADIO_METRO;  break;
				}

				(void) ::CheckRadioButton(hwnd_, DLG_COLOR_RADIO_LIGHT, DLG_COLOR_RADIO_RETRO, idx);

				ischanged_ = false;
				isload_ = true;

			} catch (...) {}
		}

		const bool DialogThemeColors::IsChanged() const {
			return ischanged_.load();
		}
		void DialogThemeColors::SetHWNDParent(HWND h) {
			hwndp_.reset(h);
		}

		IO::PluginUi* DialogThemeColors::GetUi() {
			return static_cast<IO::PluginUi*>(this);
		}

		const bool DialogThemeColors::IsRun() const {
			return (hwnd_) && isload_.load();
		}
		const bool DialogThemeColors::IsRunOnce() const {
			return !hwnd_ && !isload_;
		}
		void DialogThemeColors::SetFocus() {
			if (hwnd_) (void) ::SetFocus(hwnd_);
		}

		#pragma region Change On events
		void DialogThemeColors::change_brush_(uint16_t idx, COLORREF clr) {
			try {
				brushs_[idx].reset(::CreateSolidBrush(clr));
				::InvalidateRect(ctrls_[idx].get(), 0, true);
			} catch (...) {}
		}
		void DialogThemeColors::change_theme_(ui_theme& theme) {
			try {
				change_brush_(0, theme.Text);
				change_brush_(1, theme.PanelBackground);
				change_brush_(2, theme.PanelBorder);
			} catch (...) {}
		}
		void DialogThemeColors::change_color_select_(uint16_t id) {
			try {

				common_config& cnf = common_config::Get();
				ui_theme& theme = cnf.UiThemes.GetTheme(ui_themes::ThemeId::Custom);
				DWORD color;
				uint16_t idx;
				switch (id) {
					case DLG_COLOR_BOX1: {
						color = theme.Text;
						idx = 0U;
						break;
					}
					case DLG_COLOR_BOX2: {
						color = theme.PanelBackground;
						idx = 1U;
						break;
					}
					case DLG_COLOR_BOX3: {
						color = theme.PanelBorder;
						idx = 2U;
						break;
					}
					default: return;
				}

				CHOOSECOLORW cc{};
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = hwnd_;
				cc.lpCustColors = (LPDWORD)customcolors_;
				cc.rgbResult = color;
				cc.Flags = CC_ANYCOLOR | CC_RGBINIT;

				if (::ChooseColorW(&cc)) {
					change_brush_(idx, cc.rgbResult);
					switch (id) {
						case DLG_COLOR_BOX1: cnf.UiThemes.SetCustomThemeColorText(cc.rgbResult); break;
						case DLG_COLOR_BOX2: cnf.UiThemes.SetCustomThemeColorBackground(cc.rgbResult); break;
						case DLG_COLOR_BOX3: cnf.UiThemes.SetCustomThemeColorBorder(cc.rgbResult); break;
						default: return;
					}
					ischanged_ = true;
				}
			} catch (...) {}
		}
		void DialogThemeColors::change_theme_select_(uint16_t id) {
			try {
				uint16_t idx;
				switch (id) {
					case DLG_COLOR_RADIO_LIGHT:  idx = 0; break;
					case DLG_COLOR_RADIO_DARK:   idx = 1; break;
					case DLG_COLOR_RADIO_METRO:  idx = 2; break;
					case DLG_COLOR_RADIO_MODERN: idx = 3; break;
					case DLG_COLOR_RADIO_RETRO:  idx = 4; break;
					default: return;
				}
				common_config& cnf = common_config::Get();

				ui_themes::ThemeId theme = static_cast<ui_themes::ThemeId>(idx);
				change_theme_(cnf.UiThemes.GetTheme(theme));
				if (theme == cnf.UiThemes.GetCustomThemeId()) return;
				cnf.UiThemes.SetCustomThemeId(theme);
				ischanged_ = true;
			} catch (...) {}
		}
		LRESULT DialogThemeColors::event_colors_draw_(LPARAM lp) {
			try {
				HWND hwnd = reinterpret_cast<HWND>(lp);
				if (hwnd) {
					for (uint16_t i = 0; i < _countof(ctrls_); i++) {
						if (ctrls_[i].equals(hwnd)) {
							if (brushs_[i])
								return reinterpret_cast<LRESULT>(brushs_[i].get());
						}
					}
				}
			} catch (...) {}
			return 0;
		}
		#pragma endregion

		#pragma region Override
		HWND DialogThemeColors::BuildDialog(HWND h) {
			hinst_.reset(LangInterface::Get().GetLangHinstance());
			return IO::PluginUi::BuildDialog(hinst_, h, MAKEINTRESOURCEW(DLG_COLOR_WINDOW));
		}
		LRESULT DialogThemeColors::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
			try {
				switch (m) {
					case WM_INITDIALOG: {
						hwnd_.reset(h, static_cast<SUBCLASSPROC>(&PluginUi::DialogProc_), reinterpret_cast<DWORD_PTR>(this), 0);
						init_();
						::ShowWindow(h, SW_SHOW);
						::SetFocus(::GetDlgItem(h, IDCANCEL));
						return static_cast<INT_PTR>(1);
					}
					case WM_HELP: {
						if (!l) break;
						UI::UiUtils::ShowHelpPage(DLG_COLOR_WINDOW, reinterpret_cast<HELPINFO*>(l));
						return static_cast<INT_PTR>(1);
					}
					case WM_CTLCOLORSTATIC: {
						return event_colors_draw_(l);
					}
					case WM_COMMAND: {
						if (!isload_) break;
						uint16_t c{ LOWORD(w) };
						switch (c) {
							case DLG_COLOR_RADIO_DARK:
							case DLG_COLOR_RADIO_METRO:
							case DLG_COLOR_RADIO_LIGHT:
							case DLG_COLOR_RADIO_RETRO:
							case DLG_COLOR_RADIO_MODERN: {
								change_theme_select_(c);
								break;
							}
							case DLG_COLOR_BOX1:
							case DLG_COLOR_BOX2:
							case DLG_COLOR_BOX3: {
								if (!isload_ || (HIWORD(w) != EN_CHANGE)) break;
								change_color_select_(c);
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

	}
}
