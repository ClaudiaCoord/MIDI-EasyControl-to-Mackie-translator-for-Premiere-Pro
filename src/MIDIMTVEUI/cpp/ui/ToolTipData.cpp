/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ UI/Viewer/Editor interface.
	(c) CC 2023-2024, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace UI {

		using namespace std::placeholders;

		/* ToolTipData */

		ToolTipData::ToolTipData() {}
		ToolTipData::ToolTipData(HWND h, RECT& r, const std::wstring& s) {
			hwnd.reset(h);
			rect.left = r.left;
			rect.right = r.right;
			rect.top = r.top;
			rect.bottom = r.bottom;
			title = std::wstring(s.begin(), s.end());
		}
		const bool ToolTipData::IsValid() {
			return (!title.empty()) && (hwnd);
		}
	}
}

