/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIDIMT {

		class LangInterface {
		private:
			LANGID id_;
			HINSTANCE main_hinst_;
			HINSTANCE lang_hinst_;
			HMODULE dll_;
			HWND hwndmain_;
			static LangInterface langinterface_;

			std::wstring str_class_;
			std::wstring str_title_;

			void Init();
			void Dispose();

		public:

			HWND HwndMain = nullptr;

			LangInterface();
			~LangInterface();
			static LangInterface& Get();

			void SetMainHinstance(HINSTANCE);
			HINSTANCE GetMainHinstance();
			HINSTANCE GetLangHinstance();

			void SetMainHwnd(HWND);
			HWND GetMainHwnd();

			bool SelectLanguage(const std::wstring&);
			bool SelectLanguage(const uint32_t);
			int32_t SelectedLanguageIndex();
			std::wstring SelectedLanguageName();
			std::tuple<int32_t, std::wstring> SelectedLanguage();
			std::forward_list<std::wstring> GetLanguages();

			void    GetDialog(HWND, DLGPROC, LPWSTR, LPARAM = 0L);
			HMENU   GetMenu(LPWSTR);
			HICON   GetIcon(LPWSTR);
			HICON   GetIcon16x16(LPWSTR);
			HICON   GetIcon24x24(LPWSTR);
			HICON   GetIcon32x32(LPWSTR);
			HICON   GetIcon48x48(LPWSTR);
			HICON   GetIcon256x256(LPWSTR);
			HBITMAP GetImageIcon(LPWSTR);
			HACCEL  GetAccelerators(LPWSTR);
			std::wstring GetHelpLangId();

			std::wstring GetString(uint32_t);
			std::wstring& GetMainClass();
			std::wstring& GetMainTitle();
		};
	}
}