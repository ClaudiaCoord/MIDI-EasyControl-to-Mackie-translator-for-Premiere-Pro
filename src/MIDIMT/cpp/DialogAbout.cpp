/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2023, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"

namespace Common {
	namespace MIDIMT {

		void DialogAbout::InitDialog(HWND hwnd) {
			try {
				hwnd__.reset(hwnd);

				HWND hwcl;
				if ((hwcl = ::GetDlgItem(hwnd, IDC_OK2)) != nullptr)			::BringWindowToTop(hwcl);
				if ((hwcl = ::GetDlgItem(hwnd, IDC_OK3)) != nullptr)			::BringWindowToTop(hwcl);
				if ((hwcl = ::GetDlgItem(hwnd, IDOK)) != nullptr)				::BringWindowToTop(hwcl);
				if ((hwcl = ::GetDlgItem(hwnd, IDC_ABOUT_COPY)) != nullptr)		::BringWindowToTop(hwcl);
				if ((hwcl = ::GetDlgItem(hwnd, IDC_ABOUT_VERSION)) != nullptr)	::BringWindowToTop(hwcl);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogAbout::EndDialog() {
			hwnd__.reset();
		}

	}
}