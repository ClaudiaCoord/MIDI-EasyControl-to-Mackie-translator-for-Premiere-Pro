/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ UI/Viewer/Editor interface.
	(c) CC 2023-2024, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace UI {

		#define CHECKBTN(A) (A ? BST_CHECKED : BST_UNCHECKED)
		#define CHECKRADIO(A,B,C) (A ? B : C)

		class FLAG_EXPORT UiUtils {
		public:
			static void SetSliderInfo(HWND, const uint32_t, const uint32_t);
			static void SetSliderValues(HWND, const uint32_t, const uint32_t, const uint32_t, const uint32_t, const uint32_t);
			static uint32_t GetSliderValue(HWND, const uint32_t);

			static std::wstring GetControlText(HWND, const uint32_t);
			static bool GetControlChecked(HWND, const uint32_t);
			static void SetControlEnable(HWND, const uint32_t, const bool);
			static const bool CheckControlEnable(HWND, const uint32_t);
			static void SaveDialogEnabled(HWND h, bool = true);

			static std::wstring GetBlank(uint32_t);

			static void ShowHelpPage(uint32_t, HELPINFO*);
			static void ShowHelpPage(const uint16_t, const uint16_t);

			static std::wstring GetDragAndDrop(HDROP);
		};
	}
}