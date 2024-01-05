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

		class FLAG_EXPORT ToolTipData {
		public:
			hwnd_ptr<empty_deleter> hwnd{};
			RECT		 rect{};
			std::wstring title;

			ToolTipData();
			ToolTipData(HWND, RECT&, const std::wstring&);

			const bool IsValid();
		};
	}
}
