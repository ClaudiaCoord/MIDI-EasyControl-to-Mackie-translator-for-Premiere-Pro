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

		class FLAG_EXPORT WindowToTop {
		private:
			struct Data {
				DWORD id{ 0 };
				HWND hwnd{ nullptr };
			};

			HWND find_app_(const std::wstring&);
			const bool app_focus_(HWND);
			static BOOL CALLBACK win_cb_(HWND, LPARAM);

		public:

			WindowToTop() = default;
			~WindowToTop() = default;

			const bool set(const std::wstring&);

		};

	}
}

