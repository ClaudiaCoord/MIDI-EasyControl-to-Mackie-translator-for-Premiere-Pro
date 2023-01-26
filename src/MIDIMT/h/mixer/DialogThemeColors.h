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

		class DialogThemeColors
		{
		private:
			handle_ptr<HWND>   hwnd__;
			handle_ptr<HWND>   hwndparent__;
			handle_ptr<HWND>   ctrls__[3];
			handle_ptr<HBRUSH, default_hgdiobj_deleter<HBRUSH>> brushs__[3]{};
			std::atomic<bool>  ischanged__ {false};
			static COLORREF customcolors__[16];

			void dispose_();
			void changebrush_(uint16_t, COLORREF);
			void changetheme_(ui_theme&);

		public:

			DialogThemeColors() = default;
			~DialogThemeColors();

			const bool IsRun() const;
			const bool IsChanged();
			void SetFocus();
			void SetHWNDParent(HWND);

			void InitDialog(HWND);
			void EndDialog();

			void ThemeSelector(uint16_t);
			void ColorSelector(uint16_t);
			LRESULT ColorsDraw(LPARAM);

		};
	}
}
