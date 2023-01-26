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
			const wchar_t locale[7];
			const wchar_t name[12];
		};
		static const LANGIDBASE langidbase[] = {
			{		0, L"AUTO\0", L"no-NE\0", L"AUTO\0" },
			{    LANG_RUSSIAN, L"RU\0",   L"ru-RU\0", L"Russian\0"},
			{ LANG_BELARUSIAN, L"RU\0",   L"be-BY\0", L"Belarusian\0"},
			{  LANG_BULGARIAN, L"RU\0",   L"bg-BG\0", L"Bulgarian\0"},
			{  LANG_UKRAINIAN, L"RU\0",   L"uk-UA\0", L"Ukrainian\0"},
			{    LANG_ENGLISH, L"EN\0",   L"en-US\0", L"English\0"}
		};

		static LANGID get_uilid() {
			__try {
				return ::GetUserDefaultUILanguage();
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
			for (size_t i = 0; i < std::size(langidbase); i++)
				if (langidbase[i].id == id) return std::ref(langidbase[i]);
			return std::ref(langidbase[0]);
		}
		static const LANGIDBASE& find_lang(std::wstring s) {
			for (size_t i = 0; i < std::size(langidbase); i++)
				if (s._Equal(langidbase[i].name)) return std::ref(langidbase[i]);
			return std::ref(langidbase[0]);
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
			return std::ref(langidbase[0]);
		}

		LangInterface LangInterface::langinterface__;

		LangInterface::LangInterface() : main_hinst__(nullptr), lang_hinst__(nullptr), dll__(nullptr), hwndmain__(nullptr) {
		}
		LangInterface::~LangInterface() {
			Dispose();
		}
		LangInterface& LangInterface::Get() {
			return std::ref(langinterface__);
		}

		HINSTANCE	   LangInterface::GetMainHinstance() {
			return main_hinst__;
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
						const LANGIDBASE& lb = get_lang();
						if (lb.id == 0) {
							if (!first_run)
								Common::to_log::Get() << default_lang_msg1__;
							break;
						}

						auto p = std::filesystem::current_path();
						p.append(L"lang");

						if (std::filesystem::exists(p)) {
							for (const auto& f : std::filesystem::directory_iterator(p)) {
								std::wstring name = f.path().stem().wstring();
								if (Utils::EndsWith(name, lb.dllid)) {
									HMODULE dll = get_dll(f.path().wstring().c_str());
									if (dll__ == dll) {
										if (!first_run)
											Common::to_log::Get() << default_lang_msg2__;
										break;
									}
									Dispose();
									lang_hinst__ = dll__ = dll;
									if (!first_run)
										Common::to_log::Get() << (log_string() << default_lang_msg3__ << lb.name);
									break;
								}
							}
						}
						if (!set_ui_thread(lb.locale) && !first_run)
							Common::to_log::Get() << (log_string() << default_lang_msg4__ << lb.locale << L"/" << lb.name);
					} catch (...) {
						if (!first_run)
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
				} while (0);

				str_class__ = GetString(IDC_MIDIMT);
				str_title__ = GetString(IDS_APP_TITLE);
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
				Common::common_config::Get().Registry.SetLanguageId(lb.id);
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
			return langidbase[0].name;
		}
		std::forward_list<std::wstring> LangInterface::GetLanguages() {
			try {
				std::forward_list<std::wstring> list;
				long sz = static_cast<long>(std::size(langidbase));
				for (long i = sz - 1; 0 <= i; i--)
					list.push_front(langidbase[i].name);
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

		std::wstring&   LangInterface::GetMainClass() {
			return std::ref(str_class__);
		}
		std::wstring&   LangInterface::GetMainTitle() {
			return std::ref(str_title__);
		}
	}
}