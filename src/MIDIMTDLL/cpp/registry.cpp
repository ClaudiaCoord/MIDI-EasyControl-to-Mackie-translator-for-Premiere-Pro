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
#include "h\winreg.h"

/* #define DEBUG_PRINT 1 */

namespace Common
{
	static const wchar_t regRoot[] = L"SOFTWARE\\CC\\MIDIMT";
	static const wchar_t regRun[] = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	static const wchar_t regLogger[] = L"Logger";
	static const wchar_t regAutorun[] = L"Autorun";
	static const wchar_t regLangId[] = L"LangId";
	static const wchar_t regMixerFastValue[] = L"MixerFastValue";
	static const wchar_t regSetAudioLevelOldvalue[] = L"SetAudioLevelOldvalue";
	static const wchar_t regConfigName[] = L"ConfigName";
	static const wchar_t regMixerEnable[] = L"MixerEnable";
	static const wchar_t regMMKeyEnable[] = L"MMKeyEnable";
	static const wchar_t regSmartHomeEnable[] = L"SmartHomeEnable";
	static const wchar_t regMixerRightClick[] = L"MixerRightClick";
	
	static const wchar_t regUiCustomTheme[] = L"UiCustomTheme";
	static const wchar_t regUiCustomThemeColorBg[] = L"UiCustomThemeColorBg";
	static const wchar_t regUiCustomThemeColorBr[] = L"UiCustomThemeColorBr";
	static const wchar_t regUiCustomThemeColorTxt[] = L"UiCustomThemeColorTxt";
	static const wchar_t regUiTheme[] = L"UiTheme";
	static const wchar_t regUiPlace[] = L"UiPlace";
	static const wchar_t regDisplayX[] = L"DisplayX";
	static const wchar_t regDisplayY[] = L"DisplayY";
	static const wchar_t regDisplayWidth[] = L"DisplayWidth";
	static const wchar_t regDisplayHeight[] = L"DisplayHeight";

	static const wchar_t regRunId[] = L"MIDIMT";

	template <typename T1>
	T1 getnumber_(const std::wstring field, T1 defvalue) {
		try {
			winreg::RegKey key{ HKEY_CURRENT_USER, regRoot };
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
			winreg::RegKey key{ HKEY_CURRENT_USER, regRoot };
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
			winreg::RegKey key{ HKEY_LOCAL_MACHINE, regRun };
			key.SetStringValue(regRunId, val);
		} catch (...) {
			#if defined(DEBUG_PRINT)
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			#endif
			try {
				winreg::RegKey key{ HKEY_CURRENT_USER, regRun };
				key.SetStringValue(regRunId, val);
			} catch (...) {
				#if defined(DEBUG_PRINT)
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				#endif
			}
		}
	}
	static std::wstring getautostart_() {
		try {
			winreg::RegKey key{ HKEY_LOCAL_MACHINE, regRun };
			return key.GetStringValue(regRunId);
		} catch (...) {
			#if defined(DEBUG_PRINT)
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			#endif
			try {
				winreg::RegKey key{ HKEY_CURRENT_USER, regRun };
				return key.GetStringValue(regRunId);
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
			winreg::RegKey key{ HKEY_LOCAL_MACHINE, regRun };
			key.DeleteValue(regRunId);
		} catch (...) {
			#if defined(DEBUG_PRINT)
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			#endif
			try {
				winreg::RegKey key{ HKEY_CURRENT_USER, regRun };
				key.DeleteValue(regRunId);
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
		return getnumber_<bool>(regLogger, false);
	}
	bool		 registry::GetAutoRun() {
		return getnumber_<bool>(regAutorun, false);
	}
	bool		 registry::GetSysAutoStart() {
		std::wstring s = getautostart_();
		return !s.empty();
	}
	bool		 registry::GetMixerEnable() {
		return getnumber_<bool>(regMixerEnable, false);
	}
	bool		 registry::GetMMKeyEnable() {
		return getnumber_<bool>(regMMKeyEnable, false);
	}
	bool		 registry::GetSmartHomeEnable() {
		return getnumber_<bool>(regSmartHomeEnable, false);
	}
	//bool GetSmartHomeEnable()
	bool		 registry::GetMixerRightClick() {
		return getnumber_<bool>(regMixerRightClick, false);
	}
	bool		 registry::GetMixerFastValue() {
		return getnumber_<bool>(regMixerFastValue, false);
	}
	bool		 registry::GetMixerSetOldLevelValue() {
		return getnumber_<bool>(regSetAudioLevelOldvalue, true);
	}
	std::wstring registry::GetConfPath() {
		return getstring_(HKEY_CURRENT_USER, regRoot, regConfigName);
	}
	int 		 registry::GetLanguageId() {
		return getnumber_<int>(regLangId, 0);
	}
	ui_themes::ThemeId registry::GetUiTheme() {
		return static_cast<ui_themes::ThemeId>(getnumber_<int>(regUiTheme, 0));
	}
	ui_themes::ThemeId registry::GetUiCustomTheme() {
		return static_cast<ui_themes::ThemeId>(getnumber_<int>(regUiCustomTheme, 0));
	}
	ui_themes::ThemePlace registry::GetUiPlace() {
		return static_cast<ui_themes::ThemePlace>(getnumber_<int>(regUiPlace, 0));
	}
	std::tuple<COLORREF, COLORREF, COLORREF> registry::GetUiCustomThemeColors() {
		return std::make_tuple(
			getnumber_<COLORREF>(regUiCustomThemeColorBg, 0),
			getnumber_<COLORREF>(regUiCustomThemeColorTxt, 0),
			getnumber_<COLORREF>(regUiCustomThemeColorBr, 0)
		);
	}
	RECT 		 registry::GetDisplay() {
		RECT r{};
		r.left = getnumber_<long>(regDisplayX, 0);
		r.right = getnumber_<long>(regDisplayY, 0);
		r.top = getnumber_<long>(regDisplayWidth, 0);
		r.bottom = getnumber_<long>(regDisplayHeight, 0);
		return r;
	}

	void		 registry::SetLogWrite(bool b) {
		setnumber_(b, regLogger);
	}
	void		 registry::SetAutoRun(bool b) {
		setnumber_(b, regAutorun);
	}
	void		 registry::SetMixerEnable(bool b) {
		setnumber_(b, regMixerEnable);
	}
	void		 registry::SetMMKeyEnable(bool b) {
		setnumber_(b, regMMKeyEnable);
	}
	void		 registry::SetSmartHomeEnable(bool b) {
		setnumber_(b, regSmartHomeEnable);
	}
	void		 registry::SetMixerRightClick(bool b) {
		setnumber_(b, regMixerRightClick);
	}
	void		 registry::SetMixerFastValue(bool b) {
		setnumber_(b, regMixerFastValue);
	}
	void		 registry::SetMixerSetOldLevelValue(bool b) {
		setnumber_(b, regSetAudioLevelOldvalue);
	}
	void		 registry::SetSysAutoStart(bool b) {
		if (b) {
			try {
				wchar_t path[MAX_PATH]{};
				std::wstring s;

				if (::GetModuleFileNameW(nullptr, path, MAX_PATH) == 0) return;
				s = std::wstring(path);
				setautostart_(s);
			}
			catch (...) {}
		} else deleteautostart_();
	}
	void		 registry::SetConfPath(std::wstring& s) {
		setstring_(HKEY_CURRENT_USER, s, regRoot, regConfigName);
	}
	void		 registry::SetLanguageId(int id) {
		setnumber_(id, regLangId);
	}
	void		 registry::SetUiTheme(ui_themes::ThemeId t) {
		setnumber_(static_cast<int>(t), regUiTheme);
	}
	void		 registry::SetUiCustomTheme(ui_themes::ThemeId t) {
		setnumber_(static_cast<int>(t), regUiCustomTheme);
	}
	void		 registry::SetUiPlace(ui_themes::ThemePlace t) {
		setnumber_(static_cast<int>(t), regUiPlace);
	}
	void		 registry::SetUiCustomThemeColors(COLORREF bg, COLORREF txt, COLORREF border) {
		setnumber_(static_cast<DWORD>(bg), regUiCustomThemeColorBg);
		setnumber_(static_cast<DWORD>(txt), regUiCustomThemeColorTxt);
		setnumber_(static_cast<DWORD>(border), regUiCustomThemeColorBr);
	}
	void		 registry::SetUiCustomThemeColorText(COLORREF c) {
		setnumber_(static_cast<DWORD>(c), regUiCustomThemeColorTxt);
	}
	void		 registry::SetUiCustomThemeColorBackground(COLORREF c) {
		setnumber_(static_cast<DWORD>(c), regUiCustomThemeColorBg);
	}
	void		 registry::SetUiCustomThemeColorBorder(COLORREF c) {
		setnumber_(static_cast<DWORD>(c), regUiCustomThemeColorBr);
	}

	void 		 registry::SetDisplay(RECT& r) {
		setnumber_(r.left, regDisplayX);
		setnumber_(r.right, regDisplayY);
		setnumber_(r.top, regDisplayWidth);
		setnumber_(r.bottom, regDisplayHeight);
	}
}