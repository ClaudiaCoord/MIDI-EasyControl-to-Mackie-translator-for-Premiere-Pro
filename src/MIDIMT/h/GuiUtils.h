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

		template <typename T>
		class ICONDATA {
			T icon;
			uint16_t iconId, ctrlId;
		public:
			ICONDATA();
			ICONDATA(T);
			ICONDATA(T, uint16_t, uint16_t);
			uint16_t GetIconId();
			uint16_t GetCtrlId();
			void Reset();
			T Get();
		};

		template <typename T>
		class GuiImageStateButton {
		private:
			bool isimagebox__;
			handle_ptr<HWND> hwnd__;
			std::vector<ICONDATA<T>*> icos__{};
			void Dispose();
		public:

			GuiImageStateButton();
			~GuiImageStateButton();
			bool  IsEmpty() const;
			void  Init(HWND, uint16_t);
			void  Init(uint16_t[], size_t, bool = false);
			void  Reset();
			void  Release();
			void  SetEnable(bool = true);
			void  SetStatus(uint16_t = 0);
			T     GetIcon(uint16_t = 0);
		};

		#if defined (EXT_DRAW_BITMAP)
		class BitmapInfo {
		private:
			BITMAPINFO bmi__{};
			void set(BITMAP&);
		public:
			BitmapInfo(BITMAP&);
			BitmapInfo(HBITMAP);
			BITMAPINFO* get();
			int width();
			int height();
		};
		#endif

		class Gui {
		public:
			static void SetControlEnable(HWND, int, bool = true);
			static bool GetControlChecked(HWND, int);
			static bool CheckControlEnable(HWND, int);
			static void IcondataButtonEnabled(HWND, ICONDATA<HICON>*, bool = true);
			static void SaveConfigEnabled(HWND);
			static void ShowHelpPage(uint32_t, uint32_t);
			static void ShowHelpPage(uint32_t, HELPINFO*);

			static std::wstring GetDragAndDrop(HDROP);
			static std::wstring GetBlank(uint32_t);
			static std::wstring GetControlText(HWND, int);
		};
	}
}