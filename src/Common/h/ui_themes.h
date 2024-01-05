/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {

	class FLAG_EXPORT ui_theme_peak_meter {
	private:
		Gdiplus::Rect r;
		Gdiplus::Color fore;
		Gdiplus::Color back;
		float sangle, vangle;
	public:
		ui_theme_peak_meter();
		ui_theme_peak_meter(int32_t, int32_t, int32_t, int32_t, float, float, Gdiplus::Color, Gdiplus::Color);

		void	Set(int32_t, int32_t, int32_t, int32_t, float, float, Gdiplus::Color, Gdiplus::Color);
		Gdiplus::Rect  GetRect() const;
		Gdiplus::Color GetForegroundColor();
		Gdiplus::Color GetBackgroundColor();
		float	GetStartAngle() const;
		float	GetEndAngle() const;
		float	GetVolumeAngle(float&);
	};

	class FLAG_EXPORT ui_theme
	{
	private:
		handle_ptr<HBRUSH, default_hgdiobj_deleter<HBRUSH>> brushs__[2];
		void dispose_();

	public:
		COLORREF PanelBackground;
		COLORREF PanelBorder;
		COLORREF Text;

		Gdiplus::Color GetBackgroundGdi();
		Gdiplus::Color GetBorderGdi();
		Gdiplus::Color GetTextGdi();
		HBRUSH GetBackgroundBrush();
		HBRUSH GetBorderBrush();

		ui_theme_peak_meter PeakMeter{};

		ui_theme();
		~ui_theme();

		void Set(COLORREF, COLORREF, COLORREF);
		void SetColorText(COLORREF);
		void SetColorBackground(COLORREF);
		void SetColorBorder(COLORREF);
	};

	class FLAG_EXPORT ui_themes
	{
	public:
		enum ThemeId : int {
			Light = 0,
			Dark,
			Metro,
			Modern,
			Retro,
			Custom,
			None
		};
		enum ThemePlace : int {
			VerticalLeft = 0,
			VerticalRight,
			HorizontalTop,
			HorizontalBottom
		};

	private:
		RECT display__{};
		ui_theme themes__[6]{};
		
		ThemeId    CustomTheme = ThemeId::Dark;
		ThemeId    CurrentTheme = ThemeId::Dark;
		ThemePlace CurrentPlace = ThemePlace::VerticalRight;

	public:

		ui_themes();

		ui_theme& GetTheme();
		ui_theme& GetTheme(ThemeId);
		ThemeId GetThemeId();
		ThemeId GetCustomThemeId();
		ThemePlace GetPlaceId();
		RECT& GetDisplay();

		void SetTheme(ThemeId);
		void SetCustomThemeId(ThemeId);
		void SetCustomThemePeakMeter(ThemeId);
		void SetCustomThemeColorText(COLORREF);
		void SetCustomThemeColorBackground(COLORREF);
		void SetCustomThemeColorBorder(COLORREF);
		void SetCustomTheme(ThemeId, COLORREF, COLORREF, COLORREF);
		void SetPlace(ThemePlace);
		void SetDisplay(RECT);
		bool IsPlaceVertical();
	};
}
