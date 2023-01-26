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
#include "Commdlg.h"

namespace Common {
	namespace MIDIMT {


		COLORREF DialogThemeColors::customcolors__[16]{};

		DialogThemeColors::~DialogThemeColors() {
			dispose_();
		}
		void DialogThemeColors::dispose_() {
			try {
				for (uint16_t i = 0; i < _countof(brushs__); i++) {
					brushs__[i].reset();
					ctrls__[i].reset();
				}
				hwnd__.reset();
			} catch (...) {}
		}
		void DialogThemeColors::changebrush_(uint16_t idx, COLORREF clr) {
			try {
				brushs__[idx].reset(::CreateSolidBrush(clr));
				::InvalidateRect(ctrls__[idx].get(), 0, true);
			} catch (...) {}
		}
		void DialogThemeColors::changetheme_(ui_theme& theme) {
			try {
				changebrush_(0, theme.Text);
				changebrush_(1, theme.PanelBackground);
				changebrush_(2, theme.PanelBorder);
			} catch (...) {}
		}

		void DialogThemeColors::InitDialog(HWND hwnd) {
			try {
				hwnd__.reset(hwnd);
				ctrls__[0].reset(::GetDlgItem(hwnd__, IDC_COLOR_BOX1));
				ctrls__[1].reset(::GetDlgItem(hwnd__, IDC_COLOR_BOX2));
				ctrls__[2].reset(::GetDlgItem(hwnd__, IDC_COLOR_BOX3));

				common_config& cnf = common_config::Get();
				changetheme_(cnf.UiThemes.GetTheme(ui_themes::ThemeId::Custom));

				uint16_t idx = static_cast<uint16_t>(cnf.UiThemes.GetCustomThemeId());
				switch (idx) {
					case 0:  idx = IDC_RADIO_LIGHT;  break;
					case 1:  idx = IDC_RADIO_DARK;   break;
					case 2:  idx = IDC_RADIO_METRO;  break;
					case 3:  idx = IDC_RADIO_MODERN; break;
					case 4:  idx = IDC_RADIO_RETRO;  break;
					default: idx = IDC_RADIO_METRO;  break;
				}

				(void) ::CheckRadioButton(hwnd__, IDC_RADIO_LIGHT, IDC_RADIO_RETRO, idx);
				ischanged__ = false;

			} catch (...) {}
		}
		void DialogThemeColors::EndDialog() {
			try {
				if (ischanged__.load()) {
					worker_background::Get().to_async(std::async(std::launch::async, [=]() {
						common_config& cnf = common_config::Get();
						for (uint16_t i = 0; i < _countof(brushs__); i++) {
							LOGBRUSH lbr{};
							if (::GetObjectW(brushs__[i].get(), sizeof(lbr), &lbr)) {
								switch (i) {
									case 0: cnf.UiThemes.SetCustomThemeColorText(lbr.lbColor); break;
									case 1: cnf.UiThemes.SetCustomThemeColorBackground(lbr.lbColor); break;
									case 2: cnf.UiThemes.SetCustomThemeColorBorder(lbr.lbColor); break;
									default: break;
								}
							}
						}
						if (hwndparent__)
							::PostMessageW(hwndparent__.get(), WM_COMMAND, MAKEWPARAM(IDM_CUSTOM_RELOAD, 0), 0);
						dispose_();
					}));
				} else {
					dispose_();
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		const bool DialogThemeColors::IsRun() const {
			return (bool)hwnd__;
		}
		const bool DialogThemeColors::IsChanged() {
			return ischanged__.load();
		}
		void DialogThemeColors::SetFocus() {
			(void) ::SetFocus(hwnd__.get());
		}
		void DialogThemeColors::SetHWNDParent(HWND h) {
			hwndparent__.reset(h);
		}

		void DialogThemeColors::ThemeSelector(uint16_t id) {
			try {
				uint16_t idx;
				switch (id) {
					case IDC_RADIO_LIGHT:  idx = 0; break;
					case IDC_RADIO_DARK:   idx = 1; break;
					case IDC_RADIO_METRO:  idx = 2; break;
					case IDC_RADIO_MODERN: idx = 3; break;
					case IDC_RADIO_RETRO:  idx = 4; break;
					default: return;
				}
				common_config& cnf = common_config::Get();

				ui_themes::ThemeId theme = static_cast<ui_themes::ThemeId>(idx);
				changetheme_(cnf.UiThemes.GetTheme(theme));
				if (theme == cnf.UiThemes.GetCustomThemeId()) return;
				cnf.UiThemes.SetCustomThemeId(theme);
				ischanged__ = true;
			} catch (...) {}
		}
		void DialogThemeColors::ColorSelector(uint16_t id) {
			try {

				common_config& cnf = common_config::Get();
				ui_theme& theme = cnf.UiThemes.GetTheme(ui_themes::ThemeId::Custom);
				DWORD color;
				uint16_t idx;
				switch (id) {
					case IDC_COLOR_BOX1: {
						color = theme.Text;
						idx = 0U;
						break;
					}
					case IDC_COLOR_BOX2: {
						color = theme.PanelBackground;
						idx = 1U;
						break;
					}
					case IDC_COLOR_BOX3: {
						color = theme.PanelBorder;
						idx = 2U;
						break;
					}
					default: return;
				}

				CHOOSECOLORW cc{};
				cc.lStructSize = sizeof(cc);
				cc.hwndOwner = hwnd__;
				cc.lpCustColors = (LPDWORD)customcolors__;
				cc.rgbResult = color;
				cc.Flags = CC_ANYCOLOR | CC_RGBINIT;

				if (::ChooseColorW(&cc)) {
					changebrush_(idx, cc.rgbResult);
					switch (id) {
						case IDC_COLOR_BOX1: cnf.UiThemes.SetCustomThemeColorText(cc.rgbResult); break;
						case IDC_COLOR_BOX2: cnf.UiThemes.SetCustomThemeColorBackground(cc.rgbResult); break;
						case IDC_COLOR_BOX3: cnf.UiThemes.SetCustomThemeColorBorder(cc.rgbResult); break;
						default: return;
					}
					ischanged__ = true;
				}
			} catch (...) {}
		}
		LRESULT DialogThemeColors::ColorsDraw(LPARAM lp) {
			try {
				HWND hwnd = reinterpret_cast<HWND>(lp);
				if (hwnd != nullptr) {
					for (uint16_t i = 0; i < _countof(ctrls__); i++) {
						if (ctrls__[i].equals(hwnd)) {
							if (brushs__[i])
								return reinterpret_cast<LRESULT>(brushs__[i].get());
						}
					}
				}
			} catch (...) {}
			return 0;
		}
	}
}
