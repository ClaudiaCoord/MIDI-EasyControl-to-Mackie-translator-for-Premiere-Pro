/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"

#include "..\..\Common\h\languages\lang_base.h"

namespace Common {
	namespace MIDIMT {

		using namespace std::placeholders;
		using namespace std::string_view_literals;

		class LangNames {
		public:
			static constexpr std::wstring_view MSG1 = L"language set to AUTO mode."sv;
			static constexpr std::wstring_view MSG2 = L"language not change."sv;
			static constexpr std::wstring_view MSG3 = L"language set to "sv;
			static constexpr std::wstring_view MSG4 = L"language not set to system thread."sv;
		};

		static bool first_run = true;

		#pragma region Static
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
		static const LANG::LANGIDBASE& find_lang_base_(int id) {
			for (size_t i = 0; i < std::size(LANG::lang_id_base); i++)
				if (LANG::lang_id_base[i].id == id) return std::ref(LANG::lang_id_base[i]);
			return std::ref(LANG::lang_id_base[0]);
		}
		static const int32_t find_lang_index_(int id) {
			for (int32_t i = 0; i < static_cast<int64_t>(std::size(LANG::lang_id_base)); i++)
				if (LANG::lang_id_base[i].id == id) return i;
			return -1;
		}
		static const LANG::LANGIDBASE& find_lang_base_(std::wstring s) {
			for (size_t i = 0; i < std::size(LANG::lang_id_base); i++)
				if (s._Equal(LANG::lang_id_base[i].name)) return std::ref(LANG::lang_id_base[i]);
			return std::ref(LANG::lang_id_base[0]);
		}
		static std::tuple<int32_t, std::wstring> find_lang_tuple_(int id) {
			for (int32_t i = 0; i < static_cast<int32_t>(std::size(LANG::lang_id_base)); i++)
				if (LANG::lang_id_base[i].id == id) return std::make_tuple(i, LANG::lang_id_base[i].name);
			return std::make_tuple(0, LANG::lang_id_base[0].name);
		}
		static const LANG::LANGIDBASE& get_lang() {
			_set_se_translator(seh_exception_catch);
			try {
				LANGID lid = common_config::Get().Registry.GetLanguageId();
				const LANG::LANGIDBASE& lb = find_lang_base_(lid);
				if (lb.id != 0) return lb;

				lid = get_uilid();
				return find_lang_base_(lid & 0xff);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return std::ref(LANG::lang_id_base[0]);
		}
		#pragma endregion

		LangInterface LangInterface::langinterface_;

		LangInterface::LangInterface() : main_hinst_(nullptr), lang_hinst_(nullptr), dll_(nullptr), hwndmain_(nullptr), id_(0) {
		}
		LangInterface::~LangInterface() {
			common_error_code::Get().set_default_error_cb();
			Dispose();
		}
		LangInterface& LangInterface::Get() {
			return std::ref(langinterface_);
		}

		HINSTANCE	   LangInterface::GetMainHinstance() {
			return main_hinst_;
		}
		HINSTANCE	   LangInterface::GetLangHinstance() {
			return lang_hinst_;
		}
		void		   LangInterface::SetMainHinstance(HINSTANCE h) {
			main_hinst_ = lang_hinst_ = h;
			Init();
		}

		void		   LangInterface::SetMainHwnd(HWND h) {
			hwndmain_ = (hwndmain_ == nullptr) ? h : hwndmain_;
		}
		HWND		   LangInterface::GetMainHwnd() {
			return hwndmain_;
		}

		void		   LangInterface::Init() {
			_set_se_translator(seh_exception_catch);
			try {
				do {
					try {
						id_ = 0;
						const LANG::LANGIDBASE& lb = get_lang();
						if (lb.id == 0) {
							if (!first_run)
								to_log::Get() << (log_string().to_log_method(__FUNCTIONW__) << LangNames::MSG1);
							break;
						}

						std::filesystem::path p(Utils::app_dir(main_hinst_));
						if (p.empty()) break;
						p.append(L"lang");

						if (std::filesystem::exists(p)) {
							for (const auto& f : std::filesystem::directory_iterator(p)) {
								if (f.path().stem().wstring().ends_with(lb.dllid)) {
									HMODULE dll = get_dll(f.path().wstring().c_str());
									if (dll_ == dll) {
										if (!first_run)
											to_log::Get() << LangNames::MSG2;
										break;
									}
									Dispose();
									lang_hinst_ = dll_ = dll;
									if (!first_run)
										to_log::Get() << (log_string().to_log_method(__FUNCTIONW__) << LangNames::MSG3 << lb.name);
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
											to_log::Get() << (log_string().to_log_method(__FUNCTIONW__) << LangNames::MSG4 << ln.c_str() << L"/" << lb.name);
										else
											isfound = true;
										break;
									}
								}
							}
							if (!isfound && !set_ui_thread(lb.locale[0]) && !first_run)
								to_log::Get() << (log_string().to_log_method(__FUNCTIONW__) << LangNames::MSG4 << lb.locale[0] << L"/" << lb.name);

							id_ = lb.id;

						} catch (...) {
							if (!first_run)
								Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						}
					} catch (...) {
						if (!first_run)
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
				} while (0);

				str_class_ = GetString(STRING_APP_CLASS);
				str_title_ = GetString(STRING_APP_TITLE);

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
				HMODULE dll = dll_;
				dll_ = nullptr;
				if (dll != nullptr) {
					lang_hinst_ = main_hinst_;
					::FreeLibrary(dll);
				}
			} catch (...) {}
		}

		bool		   LangInterface::SelectLanguage(const std::wstring& s) {
			try {
				const LANG::LANGIDBASE& lb = find_lang_base_(s);
				common_config::Get().Registry.SetLanguageId(lb.id);
				Init();
				return true;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		bool		   LangInterface::SelectLanguage(const uint32_t idx) {
			try {
				if (idx >= std::size(LANG::lang_id_base)) return false;
				const LANG::LANGIDBASE& lb = LANG::lang_id_base[idx];
				common_config::Get().Registry.SetLanguageId(lb.id);
				Init();
				return true;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		int32_t		   LangInterface::SelectedLanguageIndex() {
			try {
				int lid = common_config::Get().Registry.GetLanguageId();
				return find_lang_index_(lid);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return -1;
		}
		std::wstring   LangInterface::SelectedLanguageName() {
			try {
				int lid = common_config::Get().Registry.GetLanguageId();
				const LANG::LANGIDBASE& lb = find_lang_base_(lid);
				return lb.name;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return LANG::lang_id_base[0].name;
		}
		std::tuple<int32_t, std::wstring> LangInterface::SelectedLanguage() {
			try {
				int lid = common_config::Get().Registry.GetLanguageId();
				return find_lang_tuple_(lid);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return std::make_tuple(0, LANG::lang_id_base[0].name);
		}
		std::forward_list<std::wstring> LangInterface::GetLanguages() {
			try {
				std::forward_list<std::wstring> list;
				long sz = static_cast<long>(std::size(LANG::lang_id_base));
				for (long i = sz - 1; 0 <= i; i--)
					list.push_front(LANG::lang_id_base[i].name);
				return list;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return std::forward_list<std::wstring>();
		}

		#pragma region Static
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
		#pragma endregion

		void		   LangInterface::GetDialog(HWND hwnd, DLGPROC p, LPWSTR s, LPARAM l) {   /* using MAKEINTRESOURCEW(id) */
			_set_se_translator(seh_exception_catch);
			try {
				get_dialog(lang_hinst_, hwnd, p, s, l);
			} catch (...) {}
		}
		HMENU		   LangInterface::GetMenu(LPWSTR s) {  /* using MAKEINTRESOURCEW(id) */
			_set_se_translator(seh_exception_catch);
			try {
				return get_menu(lang_hinst_, s);
			} catch (...) {}
			return nullptr;
		}
		HICON		   LangInterface::GetIcon(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_icon(main_hinst_, s);
			} catch (...) {}
			return nullptr;
		}
		HICON		   LangInterface::GetIcon16x16(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_icon_by_sise(main_hinst_, s, 16);
			} catch (...) {}
			return nullptr;
		}
		HICON		   LangInterface::GetIcon24x24(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_icon_by_sise(main_hinst_, s, 24);
			} catch (...) {}
			return nullptr;
		}
		HICON		   LangInterface::GetIcon32x32(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_icon_by_sise(main_hinst_, s, 32);
			} catch (...) {}
			return nullptr;
		}
		HICON		   LangInterface::GetIcon48x48(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_icon_by_sise(main_hinst_, s, 48);
			} catch (...) {}
			return nullptr;
		}
		HICON		   LangInterface::GetIcon256x256(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_icon_by_sise(main_hinst_, s, 256);
			} catch (...) {}
			return nullptr;
		}
		HACCEL 		   LangInterface::GetAccelerators(LPWSTR s) {  /* using MAKEINTRESOURCEW(id) */
			_set_se_translator(seh_exception_catch);
			try {
				return get_accel(lang_hinst_, s);
			} catch (...) {}
			return nullptr;
		}
		HBITMAP 	   LangInterface::GetImageIcon(LPWSTR s) {
			_set_se_translator(seh_exception_catch);
			try {
				return get_image(main_hinst_, s);
			} catch (...) {}
			return nullptr;
		}
		std::wstring   LangInterface::GetString(uint32_t id) {
			_set_se_translator(seh_exception_catch);
			try {
				wchar_t buf[MAX_PATH]{};
				if (get_str(lang_hinst_, id, buf, static_cast<long>(std::size(buf))))
					return Common::Utils::to_string(buf);
			} catch (...) {}
			return L"";
		}

		std::wstring	LangInterface::GetHelpLangId() {
			try {
				switch (id_) {
					case LANG_CZECH:
					case LANG_SLOVAK:
					case LANG_RUSSIAN:
					case LANG_SERBIAN:
					case LANG_BULGARIAN:
					case LANG_SLOVENIAN:
					case LANG_BELARUSIAN: return LANG::lang_id_base[1].dllid;
					default: return LANG::lang_id_base[14].dllid;
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return LANG::lang_id_base[14].dllid;
		}
		std::wstring&   LangInterface::GetMainClass() {
			return std::ref(str_class_);
		}
		std::wstring&   LangInterface::GetMainTitle() {
			return std::ref(str_title_);
		}
	}
}