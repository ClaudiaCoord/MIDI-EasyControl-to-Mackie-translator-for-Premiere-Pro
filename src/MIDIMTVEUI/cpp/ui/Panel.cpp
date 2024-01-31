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

		/* UI::Panel */

		Panel::Panel() : style(0U), x(0), y(0), w(0), h(0), mcapture_(false), enable_(true), anime_(false) {
		}
		Panel::~Panel() {
			dispose_();
		}
		void Panel::dispose_() {
			hwnd_.reset();
		}
		void Panel::refresh_() {
			if (hwnd_ && UiUtils::IsUIThread())
				::SetWindowPos(hwnd_.get(), HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
		}

		void Panel::Close(int32_t attr) {
			if ((attr != -1) && anime_ && hwnd_)
				::AnimateWindow(
					hwnd_.get(),
					(attr == AW_BLEND) ? 300 : 200,
					(attr) ? (AW_HIDE | attr) : (AW_HIDE | AW_CENTER)
				);
			dispose_();
		}
		void Panel::Show(const bool anime) {
			Show(anime ? 0 : 1);
		}
		void Panel::Show(int32_t attr) {
			if (anime_ && hwnd_) {
				if (attr == -1) return;
				if (attr == 1) ::ShowWindow(hwnd_.get(), SW_SHOW);
				else ::AnimateWindow(
						hwnd_.get(),
						(attr == AW_BLEND) ? 300 : 150,
						(attr) ? (AW_ACTIVATE | attr) : (AW_ACTIVATE | AW_HOR_POSITIVE)
				);
			}
		}
		void Panel::Set(uint32_t s_, int32_t x_, int32_t y_, int32_t w_, int32_t h_) {
			style = s_, x = x_; y = y_; w = w_; h = h_;
		}
		const bool Panel::Init(HINSTANCE hi, int32_t id) {
			try {
				HWND wnd;
				if ((wnd = ::CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, TOOLTIPS_CLASSW, L" ", style, x, y, w, h, 0, 0, hi, 0)) == nullptr)
					return false;

				hwnd_.reset(wnd);
				return true;
			} catch (...) {}
			return false;
		}
		const bool Panel::Init(HINSTANCE hi, HWND hwnd, int32_t id, int32_t position, SUBCLASSPROC proc, bool animele, void* clz) {
			try {
				int32_t xx = x, yy = y;
				if (common_config::Get().UiThemes.IsPlaceVertical())
					yy += position;
				else
					xx += position;

				if ((style & WS_VISIBLE) == 0) {
					if (!animele) style = (style | WS_VISIBLE);
					else anime_ = true;
				}

				HWND wnd;
				if ((wnd = ::CreateWindowExW(WS_EX_TRANSPARENT | WS_EX_CONTROLPARENT, L"STATIC", L" ", style, xx, yy, w, h, hwnd, 0, hi, 0)) == nullptr)
					return false;

				hwnd_.reset(wnd, proc, (clz) ? reinterpret_cast<DWORD_PTR>(clz) : 0U, id);
				return true;
			} catch (...) {}
			return false;
		}
		HWND Panel::GetHWND() {
			return hwnd_.get();
		}
		RECT Panel::GetSize() {
			RECT r{};
			r.left = x;
			r.right = y;
			r.top = w;
			r.bottom = h;
			return r;
		}
		void Panel::SetSize(RECT& r) {
			x = r.left;
			y = r.right;
			w = r.top;
			h = r.bottom;
			refresh_();
		}
		bool Panel::SetPosition(bool isv, bool anime) {
			try {
				RECT r{};
				HWND hwnd = hwnd_.get();
				if (!hwnd_ || !::GetWindowRect(hwnd, &r)) return false;
				POINT p{};
				p.x = isv ? 0 : (r.left - (r.right - r.left));
				p.y = isv ? (r.top - (r.bottom - r.top)) : 0;

				uint32_t flags = (anime && anime_) ? (SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW) : (SWP_NOSIZE | SWP_NOZORDER);
				::SetWindowPos(hwnd, HWND_TOP, p.x, p.y, 0, 0, flags);
				if (anime && anime_)
					::AnimateWindow(hwnd, 150, AW_ACTIVATE | AW_VER_POSITIVE);
				return true;
			} catch (...) {}
			return false;
		}
		void Panel::SetAnimation(bool b) {
			anime_ = b;
		}
		void Panel::SetCapture(bool iscap) {
			if (hwnd_) {
				if (iscap) {
					mcapture_ = true;
					(void) ::SetCapture(hwnd_.get());
				}
				else if (mcapture_) {
					mcapture_ = !::ReleaseCapture();
				}
			}
		}
		void Panel::SetEnabled(bool state) {
			if (!hwnd_) {
				enable_ = false;
				return;
			}
			enable_ = state;
			refresh_();
		}
		const bool Panel::GetEnabled() {
			return enable_.load();
		}

		std::wstring Panel::to_string() {
			try {
				std::wstringstream ws;
				ws << std::boolalpha << L", HWND:" << (bool)(hwnd_) << L", Enable:" << enable_.load()
					<< L", Animation:" << anime_.load() << L", Mouse cap:" << mcapture_.load()
					<< L", (x:" << x << L",y:" << y << L",w:" << w << L",h:" << h << L"), style:" << style << L";\n";
				return ws.str();
			} catch (...) {}
			return L"";
		}
	}
}

