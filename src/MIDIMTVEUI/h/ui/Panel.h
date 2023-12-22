/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ UI/Viewer/Editor interface.
	(c) CC 2023, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace UI {
		
		/*
			local panel and other elements size, RECTANGLE:
			{ left = 0 (pad x), right = 0 (pad y), top = 84 (width), bottom = 94 (height) }
		*/
		static inline int32_t PLACESIZE(const bool isvertical, RECT& r) {
			return (isvertical ? (r.bottom + r.right) : (r.top + r.left));
		}

		class FLAG_EXPORT Panel {
		private:
			std::atomic<bool> anime_;
			std::atomic<bool> enable_;
			std::atomic<bool> mcapture_;
			hwnd_ptr<hwnd_deleter> hwnd_;

			void dispose_();
			void refresh_();

		public:
			uint32_t style;
			int32_t x, y, w, h;

			Panel();
			~Panel();
			void Close(int32_t = -1);
			void Show(const bool);
			void Show(int32_t = 0);
			void Set(uint32_t, int32_t, int32_t, int32_t, int32_t);
			const bool Init(HINSTANCE, int32_t); /* ToolTip init */
			const bool Init(HINSTANCE, HWND, int32_t, int32_t, SUBCLASSPROC, bool, void*);
			HWND GetHWND();

			const bool GetEnabled();
			void SetEnabled(bool);
			void SetCapture(bool = true);
			void SetAnimation(bool);
			RECT GetSize();
			void SetSize(RECT& r);
			bool SetPosition(bool, bool);

			std::wstring to_string();

			template <typename T1>
			void SetData(T1 val) {
				if (enable_ && (hwnd_ != nullptr)) {
					if constexpr (std::is_same_v<HBITMAP, T1>)
						(void) ::SendMessageW(hwnd_, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)val);
					else if constexpr (std::is_same_v<HICON, T1>)
						(void) ::SendMessageW(hwnd_, STM_SETICON, (WPARAM)val, (LPARAM)0);
					else if constexpr (std::is_same_v<std::wstring, T1>)
						(void) ::SendMessageW(hwnd_, WM_SETTEXT, (WPARAM)0, (LPARAM)val.c_str());
					else if constexpr (std::is_same_v<wchar_t[], T1>)
						(void) ::SendMessageW(hwnd_, WM_SETTEXT, (WPARAM)0, (LPARAM)val);
				}
			}
		};
	}
}
