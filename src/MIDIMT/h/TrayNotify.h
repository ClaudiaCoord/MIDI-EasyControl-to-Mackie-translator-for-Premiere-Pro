/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once
#include "MIDIMT.h"

namespace Common {
	namespace MIDIMT {

		class TrayNotify {
			GUID guid__;
			NOTIFYICONDATA data__ = { sizeof(data__) };

		public:

			TrayNotify();
			~TrayNotify();

			void Init(HWND, uint32_t, std::wstring&);
			void Install();
			void UnInstall();
			void Show();
			void Warning(std::wstring, std::wstring);
		};
	}
}
