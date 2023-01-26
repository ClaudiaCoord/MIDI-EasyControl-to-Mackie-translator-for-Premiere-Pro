/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIDIMT {

		class LangInterface {
		private:
			HINSTANCE main_hinst__;
			HINSTANCE lang_hinst__;
			HMODULE dll__;
			HWND hwndmain__;
			static LangInterface langinterface__;

			std::wstring str_class__;
			std::wstring str_title__;

			void Init();
			void Dispose();

		public:

			HWND HwndMain = nullptr;

			LangInterface();
			~LangInterface();
			static LangInterface& Get();

			void SetMainHinstance(HINSTANCE);
			HINSTANCE GetMainHinstance();

			void SetMainHwnd(HWND);
			HWND GetMainHwnd();

			void SelectLanguage(std::wstring);
			std::wstring SelectedLanguage();
			std::forward_list<std::wstring> GetLanguages();

			void    GetDialog(HWND, DLGPROC, LPWSTR, LPARAM = 0L);
			HMENU   GetMenu(LPWSTR);
			HICON   GetIcon(LPWSTR);
			HICON   GetIcon16x16(LPWSTR);
			HICON   GetIcon24x24(LPWSTR);
			HICON   GetIcon32x32(LPWSTR);
			HBITMAP GetImageIcon(LPWSTR);
			HACCEL  GetAccelerators(LPWSTR);
			std::wstring GetString(uint32_t);

			std::wstring& GetMainClass();
			std::wstring& GetMainTitle();
		};
	}
}