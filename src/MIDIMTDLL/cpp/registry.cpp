/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include "h\winreg.h"

/* #define DEBUG_PRINT 1 */

namespace Common
{
	class RegNames {
	public:
		static constexpr wchar_t Root[] = L"SOFTWARE\\CC\\MIDIMT";
		static constexpr wchar_t Run[] = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
		static constexpr wchar_t Logger[] = L"Logger";
		static constexpr wchar_t Autorun[] = L"Autorun";
		static constexpr wchar_t LangId[] = L"LangId";
		static constexpr wchar_t ConfigName[] = L"ConfigName";

		static constexpr wchar_t MixerEnable[] = L"MixerEnable";
		static constexpr wchar_t MixerRightClick[] = L"MixerRightClick";
		static constexpr wchar_t MixerDupAppRemove[] = L"MixerDupAppRemove";
		static constexpr wchar_t MixerFastValue[] = L"MixerFastValue";
		static constexpr wchar_t MixerSetAudioLevelOldvalue[] = L"MixerSetAudioLevelOldvalue";

		static constexpr wchar_t UiCustomTheme[] = L"UiCustomTheme";
		static constexpr wchar_t UiCustomThemeColorBg[] = L"UiCustomThemeColorBg";
		static constexpr wchar_t UiCustomThemeColorBr[] = L"UiCustomThemeColorBr";
		static constexpr wchar_t UiCustomThemeColorTxt[] = L"UiCustomThemeColorTxt";
		static constexpr wchar_t UiTheme[] = L"UiTheme";
		static constexpr wchar_t UiPlace[] = L"UiPlace";
		static constexpr wchar_t UiAnimation[] = L"UiAnimation";
		static constexpr wchar_t UiShowAppPath[] = L"UiShowAppPath";
		static constexpr wchar_t UiShowMidiKeyBind[] = L"UiShowMidiKeyBind";
		static constexpr wchar_t UiShowAudioPeakMeter[] = L"UiShowAudioPeakMeter";

		static constexpr wchar_t DisplayX[] = L"DisplayX";
		static constexpr wchar_t DisplayY[] = L"DisplayY";
		static constexpr wchar_t DisplayWidth[] = L"DisplayWidth";
		static constexpr wchar_t DisplayHeight[] = L"DisplayHeight";

		static constexpr wchar_t RunId[] = L"MIDIMT";
	};

	template <typename T1>
	T1 getnumber_(const std::wstring field, T1 defvalue) {
		try {
			winreg::RegKey key{ HKEY_CURRENT_USER, RegNames::Root };
			return static_cast<T1>(key.GetDwordValue(field));
		}
		catch (...) {
			#if defined(DEBUG_PRINT)
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			#endif
			return defvalue;
		}
	}
	template <typename T1>
	void setnumber_(T1 val, const std::wstring field) {
		try {
			winreg::RegKey key{ HKEY_CURRENT_USER, RegNames::Root };
			key.SetDwordValue(field, static_cast<DWORD>(val));
		}
		catch (...) {
			#if defined(DEBUG_PRINT)
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			#endif
		}
	}

	static std::wstring getstring_(const HKEY k, const std::wstring root, const std::wstring field) {
		try {
			winreg::RegKey key{ k, root };
			return key.GetStringValue(field);
		}
		catch (...) {
			#if defined(DEBUG_PRINT)
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			#endif
			return L"";
		}
	}
	static void			setstring_(const HKEY k, std::wstring& val, const std::wstring root, const std::wstring field) {
		try {
			winreg::RegKey key{ k, root };
			key.SetStringValue(field, val);
		}
		catch (...) {
			#if defined(DEBUG_PRINT)
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			#endif
		}
	}

	static void			keydelete_(const HKEY k, const std::wstring root, const std::wstring field) {
		try {
			winreg::RegKey key{ k, root };
			key.DeleteValue(field);
		}
		catch (...) {
			#if defined(DEBUG_PRINT)
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			#endif
		}
	}
	static void			keydelete_(const std::wstring root, const std::wstring field) {
		try {
			winreg::RegKey key{ HKEY_CURRENT_USER, root };
			key.DeleteValue(field);
		}
		catch (...) {
			#if defined(DEBUG_PRINT)
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			#endif
		}
	}

	static void			setautostart_(std::wstring& val) {
		try {
			winreg::RegKey key{ HKEY_LOCAL_MACHINE, RegNames::Run };
			key.SetStringValue(RegNames::RunId, val);
		} catch (...) {
			#if defined(DEBUG_PRINT)
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			#endif
			try {
				winreg::RegKey key{ HKEY_CURRENT_USER, RegNames::Run };
				key.SetStringValue(RegNames::RunId, val);
			} catch (...) {
				#if defined(DEBUG_PRINT)
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				#endif
			}
		}
	}
	static std::wstring getautostart_() {
		try {
			winreg::RegKey key{ HKEY_LOCAL_MACHINE, RegNames::Run };
			return key.GetStringValue(RegNames::RunId);
		} catch (...) {
			#if defined(DEBUG_PRINT)
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			#endif
			try {
				winreg::RegKey key{ HKEY_CURRENT_USER, RegNames::Run };
				return key.GetStringValue(RegNames::RunId);
			} catch (...) {
				#if defined(DEBUG_PRINT)
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				#endif
			}
			return L"";
		}
	}
	static void			deleteautostart_() {
		try {
			winreg::RegKey key{ HKEY_LOCAL_MACHINE, RegNames::Run };
			key.DeleteValue(RegNames::RunId);
		} catch (...) {
			#if defined(DEBUG_PRINT)
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			#endif
			try {
				winreg::RegKey key{ HKEY_CURRENT_USER, RegNames::Run };
				key.DeleteValue(RegNames::RunId);
			} catch (...) {
				#if defined(DEBUG_PRINT)
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				#endif
			}
		}
	}

	registry::registry() {

	}

	bool		 registry::GetLogWrite() {
		return getnumber_<bool>(RegNames::Logger, false);
	}
	bool		 registry::GetAutoRun() {
		return getnumber_<bool>(RegNames::Autorun, false);
	}
	bool		 registry::GetSysAutoBoot() {
		std::wstring s = getautostart_();
		return !s.empty();
	}
	bool		 registry::GetMixerEnable() {
		return getnumber_<bool>(RegNames::MixerEnable, false);
	}
	bool		 registry::GetMixerRightClick() {
		return getnumber_<bool>(RegNames::MixerRightClick, false);
	}
	bool		 registry::GetMixerFastValue() {
		return getnumber_<bool>(RegNames::MixerFastValue, false);
	}
	bool		 registry::GetMixerSetOldLevelValue() {
		return getnumber_<bool>(RegNames::MixerSetAudioLevelOldvalue, false);
	}
	bool		 registry::GetMixerDupAppRemove() {
		return getnumber_<bool>(RegNames::MixerDupAppRemove, false);
	}
	std::wstring registry::GetConfPath() {
		return getstring_(HKEY_CURRENT_USER, RegNames::Root, RegNames::ConfigName);
	}
	int 		 registry::GetLanguageId() {
		return getnumber_<int>(RegNames::LangId, 0);
	}
	bool 		 registry::GetUiShowAppPath() {
		return getnumber_<bool>(RegNames::UiShowAppPath, false);
	}
	bool		 registry::GetUiShowMidiKeyBind() {
		return getnumber_<bool>(RegNames::UiShowMidiKeyBind, false);
	}
	bool		 registry::GetUiShowAudioPeakMeter() {
		return getnumber_<bool>(RegNames::UiShowAudioPeakMeter, false);
	}
	bool 		 registry::GetUiAnimation() {
		return getnumber_<bool>(RegNames::UiAnimation, true);
	}
	ui_themes::ThemeId registry::GetUiTheme() {
		return static_cast<ui_themes::ThemeId>(getnumber_<int>(RegNames::UiTheme, 0));
	}
	ui_themes::ThemeId registry::GetUiCustomTheme() {
		return static_cast<ui_themes::ThemeId>(getnumber_<int>(RegNames::UiCustomTheme, 0));
	}
	ui_themes::ThemePlace registry::GetUiPlace() {
		return static_cast<ui_themes::ThemePlace>(getnumber_<int>(RegNames::UiPlace, 0));
	}
	std::tuple<COLORREF, COLORREF, COLORREF> registry::GetUiCustomThemeColors() {
		return std::make_tuple(
			getnumber_<COLORREF>(RegNames::UiCustomThemeColorBg, 0),
			getnumber_<COLORREF>(RegNames::UiCustomThemeColorTxt, 0),
			getnumber_<COLORREF>(RegNames::UiCustomThemeColorBr, 0)
		);
	}
	RECT 		 registry::GetDisplay() {
		RECT r{};
		r.left = getnumber_<long>(RegNames::DisplayX, 0);
		r.right = getnumber_<long>(RegNames::DisplayY, 0);
		r.top = getnumber_<long>(RegNames::DisplayWidth, 0);
		r.bottom = getnumber_<long>(RegNames::DisplayHeight, 0);
		return r;
	}

	void		 registry::SetLogWrite(bool b) {
		setnumber_(b, RegNames::Logger);
	}
	void		 registry::SetAutoRun(bool b) {
		setnumber_(b, RegNames::Autorun);
	}
	void		 registry::SetSysAutoBoot(bool b) {
		if (b) {
			try {
				wchar_t path[MAX_PATH]{};
				std::wstring s;

				if (::GetModuleFileNameW(nullptr, path, MAX_PATH) == 0) return;
				s = std::wstring(path);
				setautostart_(s);
			} catch (...) {}
		}
		else deleteautostart_();
	}
	void		 registry::SetMixerEnable(bool b) {
		setnumber_(b, RegNames::MixerEnable);
	}
	void		 registry::SetMixerRightClick(bool b) {
		setnumber_(b, RegNames::MixerRightClick);
	}
	void		 registry::SetMixerFastValue(bool b) {
		setnumber_(b, RegNames::MixerFastValue);
	}
	void		 registry::SetMixerSetOldLevelValue(bool b) {
		setnumber_(b, RegNames::MixerSetAudioLevelOldvalue);
	}
	void		 registry::SetMixerDupAppRemove(bool b) {
		setnumber_(b, RegNames::MixerDupAppRemove);
	}
	void		 registry::SetConfPath(std::wstring& s) {
		setstring_(HKEY_CURRENT_USER, s, RegNames::Root, RegNames::ConfigName);
	}
	void		 registry::SetLanguageId(int id) {
		setnumber_(id, RegNames::LangId);
	}
	void		 registry::SetUiShowAppPath(bool b) {
		setnumber_(b, RegNames::UiShowAppPath);
	}
	void		 registry::SetUiShowMidiKeyBind(bool b) {
		setnumber_(b, RegNames::UiShowMidiKeyBind);
	}
	void		 registry::SetUiShowAudioPeakMeter(bool b) {
		setnumber_(b, RegNames::UiShowAudioPeakMeter);
	}
	void 		 registry::SetUiAnimation(bool b) {
		setnumber_(b, RegNames::UiAnimation);
	}
	void		 registry::SetUiTheme(ui_themes::ThemeId t) {
		setnumber_(static_cast<int>(t), RegNames::UiTheme);
	}
	void		 registry::SetUiCustomTheme(ui_themes::ThemeId t) {
		setnumber_(static_cast<int>(t), RegNames::UiCustomTheme);
	}
	void		 registry::SetUiPlace(ui_themes::ThemePlace t) {
		setnumber_(static_cast<int>(t), RegNames::UiPlace);
	}
	void		 registry::SetUiCustomThemeColors(COLORREF bg, COLORREF txt, COLORREF border) {
		setnumber_(static_cast<DWORD>(bg), RegNames::UiCustomThemeColorBg);
		setnumber_(static_cast<DWORD>(txt), RegNames::UiCustomThemeColorTxt);
		setnumber_(static_cast<DWORD>(border), RegNames::UiCustomThemeColorBr);
	}
	void		 registry::SetUiCustomThemeColorText(COLORREF c) {
		setnumber_(static_cast<DWORD>(c), RegNames::UiCustomThemeColorTxt);
	}
	void		 registry::SetUiCustomThemeColorBackground(COLORREF c) {
		setnumber_(static_cast<DWORD>(c), RegNames::UiCustomThemeColorBg);
	}
	void		 registry::SetUiCustomThemeColorBorder(COLORREF c) {
		setnumber_(static_cast<DWORD>(c), RegNames::UiCustomThemeColorBr);
	}

	void 		 registry::SetDisplay(RECT& r) {
		setnumber_(r.left, RegNames::DisplayX);
		setnumber_(r.right, RegNames::DisplayY);
		setnumber_(r.top, RegNames::DisplayWidth);
		setnumber_(r.bottom, RegNames::DisplayHeight);
	}
}