/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {

	static ui_theme theme_empty__{};

	ui_theme::ui_theme() : PanelBackground(0), Text(0), PanelBorder(0) {
	}
	ui_theme::~ui_theme() {
		dispose_();
	}

	void ui_theme::dispose_() {
		try {
			brushs__[0].reset();
			brushs__[1].reset();
		} catch (...) {}
	}
	HBRUSH ui_theme::GetBackgroundBrush() {
		return brushs__[0].get();
	}
	HBRUSH ui_theme::GetBorderBrush() {
		return brushs__[1].get();
	}

	void ui_theme::Set(COLORREF bg, COLORREF txt, COLORREF border) {
		try {
			PanelBackground = bg;
			PanelBorder = border;
			Text = txt;
			brushs__[0].reset(::CreateSolidBrush(bg));
			brushs__[1].reset(::CreateSolidBrush(border));
		} catch (...) {}
	}
	void ui_theme::SetColorText(COLORREF c) {
		Text = c;
	}
	void ui_theme::SetColorBackground(COLORREF c) {
		try {
			PanelBackground = c;
			brushs__[0].reset(::CreateSolidBrush(c));
		} catch (...) {}
	}
	void ui_theme::SetColorBorder(COLORREF c) {
		try {
			PanelBorder = c;
			brushs__[1].reset(::CreateSolidBrush(c));
		} catch (...) {}
	}

	ui_themes::ui_themes() {
		common_config& cnf = common_config::Get();

		themes__[ThemeId::Light] .Set(RGB(233, 233, 233), RGB(59, 59, 59),    RGB(112, 112, 112));
		themes__[ThemeId::Metro] .Set(RGB(240, 240, 240), RGB(36, 164, 219),  RGB(116, 211, 250));
		themes__[ThemeId::Dark]  .Set(RGB(30, 30, 30),    RGB(219, 209, 209), RGB(172, 150, 150));
		themes__[ThemeId::Modern].Set(RGB(30, 30, 30),    RGB(106, 197, 39),  RGB(66, 134, 0));
		themes__[ThemeId::Retro] .Set(RGB(65, 65, 65),    RGB(178, 174, 158), RGB(108, 104, 104));

		display__ = cnf.Registry.GetDisplay();

		std::tuple<COLORREF, COLORREF, COLORREF> t = cnf.Registry.GetUiCustomThemeColors();
		themes__[ThemeId::Custom].Set(
			(std::get<0>(t) == 0) ? RGB(7, 62, 141) : std::get<0>(t),
			(std::get<1>(t) == 0) ? RGB(179, 242, 250) : std::get<1>(t),
			(std::get<2>(t) == 0) ? RGB(129, 199, 207) : std::get<2>(t)
		);
		CurrentPlace = cnf.Registry.GetUiPlace();
		CurrentTheme = cnf.Registry.GetUiTheme();
		CustomTheme  = cnf.Registry.GetUiCustomTheme();
	}
	ui_theme& ui_themes::GetTheme() {
		return GetTheme(CurrentTheme);
	}
	ui_theme& ui_themes::GetTheme(ThemeId id) {
		switch (id) {
			case ThemeId::Light:
			case ThemeId::Dark:
			case ThemeId::Metro:
			case ThemeId::Retro:
			case ThemeId::Modern:
			case ThemeId::Custom:
				return themes__[static_cast<int>(id)];
			default: return theme_empty__;
		}
	}
	ui_themes::ThemeId ui_themes::GetThemeId() {
		return CurrentTheme;
	}
	ui_themes::ThemeId ui_themes::GetCustomThemeId() {
		return CustomTheme;
	}
	ui_themes::ThemePlace ui_themes::GetPlaceId() {
		return CurrentPlace;
	}

	void ui_themes::SetPlace(ThemePlace t) {
		CurrentPlace = t;
		common_config::Get().Registry.SetUiPlace(t);
	}
	void ui_themes::SetTheme(ThemeId t) {
		CurrentTheme = t;
		common_config::Get().Registry.SetUiTheme(t);
	}

	void ui_themes::SetCustomTheme(ThemeId t, COLORREF bg, COLORREF txt, COLORREF border) {
		CustomTheme = t;
		themes__[ThemeId::Custom].Set(bg, txt, border);
		common_config& cnf = common_config::Get();
		cnf.Registry.SetUiCustomThemeColors(bg, txt, border);
		cnf.Registry.SetUiCustomTheme(t);
	}
	void ui_themes::SetCustomThemeId(ThemeId t) {
		CustomTheme = t;
		common_config::Get().Registry.SetUiCustomTheme(t);
	}
	void ui_themes::SetCustomThemeColorText(COLORREF c) {
		themes__[ThemeId::Custom].SetColorText(c);
		common_config::Get().Registry.SetUiCustomThemeColorText(c);
	}
	void ui_themes::SetCustomThemeColorBackground(COLORREF c) {
		themes__[ThemeId::Custom].SetColorBackground(c);
		common_config::Get().Registry.SetUiCustomThemeColorBackground(c);
	}
	void ui_themes::SetCustomThemeColorBorder(COLORREF c) {
		themes__[ThemeId::Custom].SetColorBorder(c);
		common_config::Get().Registry.SetUiCustomThemeColorBorder(c);
	}

	RECT& ui_themes::GetDisplay() {
		if ((display__.left == 0) && (display__.right == 0) && (display__.top == 0) && (display__.bottom == 0))
			display__ = common_config::Get().Registry.GetDisplay();
		return display__;
	}
	void ui_themes::SetDisplay(RECT r) {
		display__.left = r.left;
		display__.right = r.right;
		display__.top = r.top;
		display__.bottom = r.bottom;
		common_config::Get().Registry.SetDisplay(display__);
	}
	bool ui_themes::IsPlaceVertical() {
		switch (CurrentPlace) {
		case ui_themes::ThemePlace::HorizontalTop:
		case ui_themes::ThemePlace::HorizontalBottom: return false;
		default: return true;
		}
	}
}
