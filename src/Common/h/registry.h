/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {

	class FLAG_EXPORT registry {
	public:
		registry();

		bool GetLogWrite();
		bool GetAutoRun();
		bool GetSysAutoStart();
		bool GetMixerEnable();
		bool GetMMKeyEnable();
		bool GetSmartHomeEnable();
		bool GetMixerRightClick();
		bool GetMixerFastValue();
		bool GetMixerSetOldLevelValue();
		bool GetMixerDupAppRemove();
		int  GetLanguageId();
		bool GetUiShowMidiKeyBind();
		bool GetUiShowAudioPeakMeter();
		bool GetUiShowAppPath();
		bool GetUiAnimation();
		bool GetDMXPollEnable();
		ui_themes::ThemeId GetUiTheme();
		ui_themes::ThemeId GetUiCustomTheme();
		ui_themes::ThemePlace GetUiPlace();
		std::tuple<COLORREF, COLORREF, COLORREF> GetUiCustomThemeColors();
		RECT GetDisplay();

		std::wstring GetConfPath();

		void SetLogWrite(bool);
		void SetAutoRun(bool);
		void SetSysAutoStart(bool);
		void SetMixerEnable(bool);
		void SetMMKeyEnable(bool);
		void SetSmartHomeEnable(bool);
		void SetMixerRightClick(bool);
		void SetMixerFastValue(bool);
		void SetMixerSetOldLevelValue(bool);
		void SetMixerDupAppRemove(bool);
		void SetLanguageId(int);
		void SetConfPath(std::wstring&);
		void SetUiAnimation(bool);
		void SetUiShowAppPath(bool);
		void SetUiShowMidiKeyBind(bool);
		void SetUiShowAudioPeakMeter(bool);
		void SetUiTheme(ui_themes::ThemeId);
		void SetUiCustomTheme(ui_themes::ThemeId);
		void SetUiPlace(ui_themes::ThemePlace);
		void SetUiCustomThemeColors(COLORREF, COLORREF, COLORREF);
		void SetUiCustomThemeColorText(COLORREF);
		void SetUiCustomThemeColorBackground(COLORREF);
		void SetUiCustomThemeColorBorder(COLORREF);
		void SetDisplay(RECT&);
		void SetDMXPollEnable(bool);
	};
}
