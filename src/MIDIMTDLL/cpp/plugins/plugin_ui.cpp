/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023, MIT

	MIDIMMT DLL + MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace IO {

        template <typename T1>
        static inline LRESULT send_command__(T1 t, HWND h, UINT m, WPARAM w, LPARAM l) {
            if (!t) return 0;
            try {
                PluginUi* ui = reinterpret_cast<PluginUi*>(t);
                if (ui) return ui->CommandDialog(h, m, w, l);
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return 0;
        }

        const bool PluginUi::empty() const {
            return (!hwnd_);
        }

        HWND PluginUi::BuildDialog(HWND) {
            return nullptr;
        }
        HWND PluginUi::BuildDialog(HINSTANCE h, HWND p, LPWSTR s) {
            hinst_.reset(h);
            return ::CreateDialogParamW(h, s, p, static_cast<DLGPROC>(&PluginUi::BuildDialogProc_), reinterpret_cast<LPARAM>(this));
        }
        void PluginUi::CloseDialog() {
            isload_ = false;
            hwnd_.reset();
        }
        void PluginUi::CloseAnimateDialog() {
            isload_ = false;
            (void) ::AnimateWindow(
                hwnd_,
                400,
                (AW_HIDE | AW_BLEND)
            );
            hwnd_.reset();
        }

		void PluginUi::ChangeDialogPosition(RECT& r) {
			if (hwnd_) ::SetWindowPos(hwnd_, 0, r.left, r.top, (r.right - r.left), (r.bottom - r.top), SWP_NOSIZE | SWP_NOZORDER);
		}
        LRESULT PluginUi::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
            if (m == WM_INITDIALOG) {
                hwnd_.reset(h, static_cast<SUBCLASSPROC>(&PluginUi::DialogProc_), reinterpret_cast<DWORD_PTR>(this), 0);
                ::ShowWindow(h, SW_SHOW);
                return static_cast<LRESULT>(1);
            }
            if (m == WM_MOVE) {
                /* Place in used class -> ChangeDialogPosition(RECT) */
            }
            else if (uint16_t c = LOWORD(w); (m == WM_COMMAND) && ((c == IDCANCEL) || (c == 2001))) { /* DLG_PLUG_EXIT = 2001 */
                hwnd_.reset();
                return static_cast<LRESULT>(1);
            }
            return ::DefSubclassProc(h, m, w, l);
		}

        INT_PTR CALLBACK PluginUi::BuildDialogProc_(HWND h, UINT m, WPARAM w, LPARAM l) {
            if (m == WM_INITDIALOG) return send_command__<LPARAM>(l, h, m, w, l);
            return static_cast<INT_PTR>(0);
        }
        LRESULT CALLBACK PluginUi::DialogProc_(HWND h, UINT m, WPARAM w, LPARAM l, UINT_PTR sc, DWORD_PTR data) {
            return send_command__<DWORD_PTR>(data, h, m, w, l);
        }
	}
}
