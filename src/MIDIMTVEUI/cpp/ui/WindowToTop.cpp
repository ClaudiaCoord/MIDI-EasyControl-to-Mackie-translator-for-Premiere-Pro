/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ UI/Viewer/Editor interface.
	(c) CC 2023-2024, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#include "global.h"
#include <tlhelp32.h>

namespace Common {
	namespace UI {

		BOOL CALLBACK WindowToTop::win_cb_(HWND hwnd, LPARAM l) {
			__try {
				Data& ed = *(Data*)l;
				DWORD id = 0x0;

				if (::GetWindowThreadProcessId(hwnd, &id))
					if (ed.id == id) {
						if (!::IsWindowVisible(hwnd))
							return TRUE;

						ed.hwnd = hwnd;
						::SetLastError(ERROR_SUCCESS);
						return FALSE;
					}
				return TRUE;
			} __except (EXCEPTION_EXECUTE_HANDLER) {}
			return FALSE;
		}
		HWND WindowToTop::find_app_(const std::wstring& app) {
			PROCESSENTRY32W e{};
			e.dwSize = sizeof(PROCESSENTRY32W);

			HANDLE snap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

			if (::Process32FirstW(snap, &e)) {
				while (::Process32NextW(snap, &e)) {

					try {
						std::wstring s(e.szExeFile);
						if (s._Equal(app)) {
							Data ed = { e.th32ProcessID, nullptr };
							if (!::EnumWindows(&win_cb_, (LPARAM)&ed) && (::GetLastError() == ERROR_SUCCESS)) {
								if (IsWindow(ed.hwnd))
									return ed.hwnd;
							}
							return nullptr;
						}
					} catch (...) {}
				}
			}
			::CloseHandle(snap);
			return nullptr;
		}
		const bool WindowToTop::app_focus_(HWND hwnd) {
			__try {
				WINDOWPLACEMENT p{};
				p.length = sizeof(WINDOWPLACEMENT);
				if (!::GetWindowPlacement(hwnd, &p))
					return false;

				switch (p.showCmd) {
					case SW_SHOWMAXIMIZED: ::ShowWindow(hwnd, SW_SHOWMAXIMIZED); break;
					case SW_SHOWMINIMIZED: ::ShowWindow(hwnd, SW_RESTORE); break;
					default: ::ShowWindow(hwnd, SW_NORMAL); break;
				}

				return ::SetForegroundWindow(hwnd);
			} __except (EXCEPTION_EXECUTE_HANDLER) {}
			return false;
		}

		const bool WindowToTop::set(const std::wstring& app) {
			_set_se_translator(seh_exception_catch);
			try {
				HWND h = find_app_(app);
				if (!h) return false;
				return app_focus_(h);

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
	}
}
