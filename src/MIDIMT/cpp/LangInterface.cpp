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

		using namespace std::placeholders;
		using namespace std::string_view_literals;
		constexpr std::wstring_view default_lang_msg1__ = L"language set to AUTO mode."sv;
		constexpr std::wstring_view default_lang_msg2__ = L"language not change."sv;
		constexpr std::wstring_view default_lang_msg3__ = L"language set to "sv;
		constexpr std::wstring_view default_lang_msg4__ = L"language not set to system thread."sv;

		bool first_run = true;
		struct LANGIDBASE {
		public:
			const LANGID id;
			const wchar_t dllid[6];
			const wchar_t name[12];
			wchar_t locale[20][12]{};
		};
		/*
		* https://docs.translatehouse.org/projects/localization-guide/en/latest/guide/win_lang_ids.html
		* https://learn.microsoft.com/en-us/openspecs/office_standards/ms-oe376/6c085406-a698-4e12-9d4d-c3b0ee3dbc4a
		* https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
		* https://learn.microsoft.com/en-us/windows/win32/wmformat/localized-system-profiles
		* https://learn.microsoft.com/en-us/windows/win32/tablet/using-the-recognizers-collection
		* 
		* https://flagpedia.net/index
		*/
		static const LANGIDBASE lang_id_base[] = {
			{				0, L"AUTO\0", L"AUTO\0",		{ L"no-NE\0", L'\0'}},
			{    LANG_RUSSIAN, L"RU\0",   L"Russian\0",		{ L"ru-RU\0", L"ru-MD\0", L'\0'}}, /* 1049 */
			{ LANG_BELARUSIAN, L"BY\0",   L"Belarusian\0",	{ L"be-BY\0", L'\0'}}, /* 1059 */
			{  LANG_BULGARIAN, L"BG\0",   L"Bulgarian\0",	{ L"bg-BG\0", L'\0'}}, /* 1026 */
			{    LANG_BOSNIAN, L"BS\0",   L"Bosnian\0",		{ L"bs-Latn-BA\0", L'\0'}}, /* 5146 */
			{   LANG_CROATIAN, L"HR\0",   L"Croatian\0",	{ L"hr-HR\0", L"hr-BA\0", L'\0'}}, /* 1050 */
			{      LANG_CZECH, L"CZ\0",   L"Czech\0",		{ L"cs-CZ\0", L'\0'}}, /* 1029 */
			{     LANG_SLOVAK, L"SK\0",   L"Slovak\0",		{ L"sk-SK\0", L'\0'}}, /* 1051 */
			{  LANG_SLOVENIAN, L"SL\0",   L"Slovenian\0",	{ L"sl-SI\0", L'\0'}}, /* 1060 */
			{    LANG_SERBIAN, L"SR\0",   L"Serbian\0",		{ L"sr-Latn-CS\0", L"sr-Cyrl-CS\0", L'\0'}}, /* 3098 */
			{  LANG_HUNGARIAN, L"HU\0",   L"Hungarian\0",	{ L"hu-HU\0", L'\0'}}, /* 1038 */
			{   LANG_ARMENIAN, L"AR\0",   L"Armenian\0",	{ L"hy-AM\0", L'\0'}}, /* 1067 */
			{   LANG_GEORGIAN, L"GR\0",   L"Georgian\0",	{ L"ka-GE\0", L'\0'}}, /* 1079 */
			{  LANG_UKRAINIAN, L"RU\0",   L"Ukrainian\0",	{ L"uk-UA\0", L'\0'}}, /* 1049 */
			{   LANG_ROMANIAN, L"RO\0",   L"Frisian\0",		{ L"ro-RO\0", L"ro-MD\0", L"rm-CH\0", L'\0'}}, /* 1048 */
			{    LANG_ENGLISH, L"EN\0",   L"English\0",		{ L"en-US\0", L"en-GB\0", L"en-AU\0", L"en-CA\0", L"en-NZ\0", L"en-IE\0", L"en-ZA\0", L"en-JM\0", L"en-BZ\0", L"en-TT\0", L"en-ZW\0", L"en-PH\0", L"en-ID\0", L"en-HK\0", L"en-IN\0", L"en-MY\0", L"en-SG\0", L"en-029\0", L'\0'}}, /* 1033 */
			{     LANG_GERMAN, L"DE\0",   L"German\0",		{ L"de-DE\0", L"de-CH\0", L"de-AT\0", L"de-LU\0", L"de-LI\0", L'\0'}}, /* 1031 */
			{     LANG_FRENCH, L"FR\0",   L"French\0",		{ L"fr-FR\0", L"fr-FR\0", L"fr-BE\0", L"fr-CA\0", L"fr-CH\0", L"fr-LU\0", L"fr-MC\0", L"fr-RE\0", L"fr-CG\0", L"fr-SN\0", L"fr-CM\0", L"fr-CI\0", L"fr-ML\0", L"fr-MA\0", L"fr-FR\0", L"fr-FR\0", L"fr-HT\0", L"fr-029\0", L"fr-015\0", L'\0'}}, /* 1036 */
			{    LANG_FRISIAN, L"WF\0",   L"Frisian\0",		{ L"fy-NL\0", L'\0'}}, /* 1122 */
			{      LANG_DUTCH, L"NL\0",   L"Belgium\0",		{ L"nl-NL\0", L"nl-BE\0", L'\0'}}, /* 1043, 2067 */
			{     LANG_DANISH, L"DA\0",   L"Dutch\0",		{ L"da-DK\0", L'\0'}}, /* 1030 */
			{    LANG_SWEDISH, L"SV\0",   L"Swedish\0",		{ L"sv-SE\0", L"sv-FI\0", L'\0'}}, /* 1053 */
			{  LANG_NORWEGIAN, L"NR\0",   L"Norwegian\0",	{ L"nn-NO\0", L"nb-NO\0", L'\0'}}, /* 2068 */
			{ LANG_PORTUGUESE, L"PG\0",   L"Portuguese\0",	{ L"pt-PT\0", L"pt-BR\0", L'\0'}}, /* 2070 */
			{   LANG_JAPANESE, L"JP\0",   L"Japanese\0",	{ L"ja-JP\0", L'\0'}}, /* 1041 */
			{    LANG_CHINESE, L"CN\0",   L"Chinese\0",		{ L"zh-CN\0", L"zh-HK\0", L"zh-SG\0", L"zh-MO\0", L"zh-TW\0", L'\0'}}, /* 2029 */
		};

		static LANGID get_uilid() {
			__try {
				LANGID lid = ::GetUserDefaultUILanguage();
				return  (lid != 0) ? lid : ::GetSystemDefaultUILanguage();
			} __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
		}
		static const bool get_locale(LPWSTR s) {
			__try {
				return ::GetUserDefaultLocaleName(s, LOCALE_NAME_MAX_LENGTH) > 0;
			} __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
		}
		static HMODULE get_dll(const wchar_t* s) {
			__try {
				return ::LoadLibraryExW(s, 0, LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
		}
		static bool set_ui_thread(const wchar_t* s) {
			__try {
				ULONG num = 0;
				return ::SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, s, &num) && (num > 0U);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
		}
		static const LANGIDBASE& find_lang(int id) {
			for (size_t i = 0; i < std::size(lang_id_base); i++)
				if (lang_id_base[i].id == id) return std::ref(lang_id_base[i]);
			return std::ref(lang_id_base[0]);
		}
		static const LANGIDBASE& find_lang(std::wstring s) {
			for (size_t i = 0; i < std::size(lang_id_base); i++)
				if (s._Equal(lang_id_base[i].name)) return std::ref(lang_id_base[i]);
			return std::ref(lang_id_base[0]);
		}
		static const LANGIDBASE& get_lang() {
			_set_se_translator(seh_exception_catch);
			try {
				LANGID lid = common_config::Get().Registry.GetLanguageId();
				const LANGIDBASE& lb = find_lang(lid);
				if (lb.id != 0) return lb;

				lid = get_uilid();
				return find_lang(lid & 0xff);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return std::ref(lang_id_base[0]);
		}

		LangInterface LangInterface::langinterface__;

		LangInterface::LangInterface() : main_hinst__(nullptr), lang_hinst__(nullptr), dll__(nullptr), hwndmain__(nullptr), id__(0) {
		}
		LangInterface::~LangInterface() {
			common_error_code::Get().set_default_error_cb();
			Dispose();
		}
		LangInterface& LangInterface::Get() {
			return std::ref(langinterface__);
		}

		HINSTANCE	   LangInterface::GetMainHinstance() {
			return main_hinst__;
		}
		HINSTANCE	   LangInterface::GetLangHinstance() {
			return lang_hinst__;
		}
		void		   LangInterface::SetMainHinstance(HINSTANCE h) {
			main_hinst__ = lang_hinst__ = h;
			Init();
		}

		void		   LangInterface::SetMainHwnd(HWND h) {
			hwndmain__ = (hwndmain__ == nullptr) ? h : hwndmain__;
		}
		HWND		   LangInterface::GetMainHwnd() {
			return hwndmain__;
		}

		void		   LangInterface::Init() {
			_set_se_translator(seh_exception_catch);
			try {
				do {
					try {
						id__ = 0;
						const LANGIDBASE& lb = get_lang();
						if (lb.id == 0) {
							if (!first_run)
								to_log::Get() << (log_string().to_log_method(__FUNCTIONW__) << default_lang_msg1__);
							break;
						}

						std::filesystem::path p;
						{
							wchar_t cpath[MAX_PATH]{};
							if (::GetModuleFileNameW(lang_hinst__, cpath, MAX_PATH - 1) == 0) break;
							p = std::filesystem::path(cpath).parent_path();
							p.append(L"lang");
						}

						if (std::filesystem::exists(p)) {
							for (const auto& f : std::filesystem::directory_iterator(p)) {
								std::wstring name = f.path().stem().wstring();
								if (Utils::EndsWith(name, lb.dllid)) {
									HMODULE dll = get_dll(f.path().wstring().c_str());
									if (dll__ == dll) {
										if (!first_run)
											to_log::Get() << default_lang_msg2__;
										break;
									}
									Dispose();
									lang_hinst__ = dll__ = dll;
									if (!first_run)
										to_log::Get() << (log_string().to_log_method(__FUNCTIONW__) << default_lang_msg3__ << lb.name);
									break;
								}
							}
						}
						try {
							bool isfound{ false };
							wchar_t locname[LOCALE_NAME_MAX_LENGTH + 1]{};
							if (get_locale(locname)) {
								const std::wstring ln(locname);
								for (auto s : lb.locale) {
									if (!s) break;
									if (ln._Equal(s)) {
										if (!set_ui_thread(ln.c_str()) && !first_run)
											to_log::Get() << (log_string().to_log_method(__FUNCTIONW__) << default_lang_msg4__ << ln.c_str() << L"/" << lb.name);
										else
											isfound = true;
										break;
									}
								}
							}
							if (!isfound && !set_ui_thread(lb.locale[0]) && !first_run)
								to_log::Get() << (log_string().to_log_method(__FUNCTIONW__) << default_lang_msg4__ << lb.locale[0] << L"/" << lb.name);

							id__ = lb.id;

						} catch (...) {
							if (!first_run)
								Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						}
					} catch (...) {
						if (!first_run)
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
				} while (0);

				str_class__ = GetString(IDS_MIDIMT);
				str_title__ = GetString(IDS_APP_TITLE);

				common_error_code::Get().set_string_error_cb(
					[=](uint32_t i) -> std::wstring {
						try {
							uint32_t id = (i + static_cast<uint32_t>(common_error_id::err_BASE));
							const std::wstring s = this->GetString(id);
							return s.empty() ? common_error_code::get_local_error(i) : s;
						} catch (...) {}
						return L"";
					}
				);

			} catch (...) {
				if (!first_run)
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			first_run = false;
		}
		void		   LangInterface::Dispose() {
			try {
				HMODULE dll = dll__;
				dll__ = nullptr;
				if (dll != nullptr) {
					lang_hinst__ = main_hinst__;
					::FreeLibrary(dll);
				}
			} catch (...) {}
		}

		void		   LangInterface::SelectLanguage(std::wstring s) {
			try {
				const LANGIDBASE& lb = find_lang(s);
				common_config::Get().Registry.SetLanguageId(lb.id);
				Init();
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}

		}
		std::wstring   LangInterface::SelectedLanguage() {
			try {
				int lid = Common::common_config::Get().Registry.GetLanguageId();
				const LANGIDBASE& lb = find_lang(lid);
				return lb.name;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return lang_id_base[0].name;
		}
		std::forward_list<std::wstring> LangInterface::GetLanguages() {
			try {
				std::forward_list<std::wstring> list;
				long sz = static_cast<long>(std::size(lang_id_base));
				for (long i = sz - 1; 0 <= i; i--)
					list.push_front(lang_id_base[i].name);
				return list;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return std::forward_list<std::wstring>();
		}

		//
		static HMENU get_menu(HINSTANCE h, LPWSTR s) {
			__try {
				return ::LoadMenuW(h, s);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
		}
		static HICON get_icon(HINSTANCE h, LPWSTR s) {
			__try {
				return ::LoadIconW(h, s);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
		}
		static HICON get_icon_by_sise(HINSTANCE h, LPWSTR s, int sz) {
			__try {
				return (HICON)::LoadImageW(h, s, IMAGE_ICON, sz, sz, 0x0);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
		}
		static HACCEL get_accel(HINSTANCE h, LPWSTR s) {
			__try {
				return ::LoadAcceleratorsW(h, s);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
		}
		static HBITMAP get_image(HINSTANCE h, LPWSTR s) {
			__try {
				return (HBITMAP)::LoadImageW(h, s, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT | LR_DEFAULTSIZE);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
		}
		static bool get_str(HINSTANCE h, uint32_t id, LPWSTR buf, long sz) {
			__try {
				return ::LoadStringW(h, id, buf, sz) > 0;
			} __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
		}
		static void get_dialog(HINSTANCE h, HWND hwnd, DLGPROC p, LPWSTR s, LPARAM l) {
			__try {
				(void) ::DialogBoxParamW(h, s, hwnd, p, l);
			} __except (EXCEPTION_EXECUTE_HANDLER) {}
		}
		//

		void		   LangInterface::GetDialog(HWND hwnd, DLGPROC p, LPWSTR s, LPARAM l) {   /* using MAKEINTRESOURCEW(id) */
			_set_se_translator(seh_exception_catch);
			try {
				get_dialog(lang_hinst__, hwnd, p, s, l);
			} catch (...) {}
		}
		HMENU		   LangInterface::GetMenu(LPWSTR s) {  /* using MAKEINTRESOURCEW(id) */
			_set_se_translator(seh_exception_catch);
			try {
				return get_menu(lang_hinst__, s);
			} catch (...) {}
			return nullptr;
		}
		HICON		   LangInterface::GetIcon(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_icon(main_hinst__, s);
			} catch (...) {}
			return nullptr;
		}
		HICON		   LangInterface::GetIcon16x16(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_icon_by_sise(main_hinst__, s, 16);
			} catch (...) {}
			return nullptr;
		}
		HICON		   LangInterface::GetIcon24x24(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_icon_by_sise(main_hinst__, s, 24);
			} catch (...) {}
			return nullptr;
		}
		HICON		   LangInterface::GetIcon32x32(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_icon_by_sise(main_hinst__, s, 32);
			} catch (...) {}
			return nullptr;
		}
		HICON		   LangInterface::GetIcon48x48(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_icon_by_sise(main_hinst__, s, 48);
			} catch (...) {}
			return nullptr;
		}
		HICON		   LangInterface::GetIcon256x256(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_icon_by_sise(main_hinst__, s, 256);
			} catch (...) {}
			return nullptr;
		}
		HACCEL 		   LangInterface::GetAccelerators(LPWSTR s) {  /* using MAKEINTRESOURCEW(id) */
			_set_se_translator(seh_exception_catch);
			try {
				return get_accel(lang_hinst__, s);
			} catch (...) {}
			return nullptr;
		}
		HBITMAP 	   LangInterface::GetImageIcon(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_image(main_hinst__, s);
			} catch (...) {}
			return nullptr;
		}
		std::wstring   LangInterface::GetString(uint32_t id) {
			_set_se_translator(seh_exception_catch);
			try {
				wchar_t buf[MAX_PATH]{};
				if (get_str(lang_hinst__, id, buf, static_cast<long>(std::size(buf))))
					return Common::Utils::to_string(buf);
			} catch (...) {}
			return L"";
		}

		std::wstring	LangInterface::GetLangId() {
			try {
				switch (id__) {
					case LANG_CZECH:
					case LANG_SLOVAK:
					case LANG_RUSSIAN:
					case LANG_SERBIAN:
					case LANG_BULGARIAN:
					case LANG_SLOVENIAN:
					case LANG_BELARUSIAN: return lang_id_base[1].dllid;
					default: return lang_id_base[14].dllid;
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return lang_id_base[14].dllid;
		}
		std::wstring&   LangInterface::GetMainClass() {
			return std::ref(str_class__);
		}
		std::wstring&   LangInterface::GetMainTitle() {
			return std::ref(str_title__);
		}
	}
}