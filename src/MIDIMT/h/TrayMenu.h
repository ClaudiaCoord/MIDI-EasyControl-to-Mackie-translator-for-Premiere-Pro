/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2023, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIDIMT {

		class TrayMenu {

			GuiImageStateButton<HBITMAP> icons_;
			void dispose_();
			void setitem_(HMENU, uint16_t, uint16_t, bool, bool);
		public:
			TrayMenu();
			~TrayMenu();
			void EndDialog();
			void Show(HWND, const POINT);
		};
	}
}

