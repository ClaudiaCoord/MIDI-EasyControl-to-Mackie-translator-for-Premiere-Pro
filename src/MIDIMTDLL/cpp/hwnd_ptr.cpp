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

	void empty_deleter::operator()(HWND) {
	}
	void hwnd_deleter::operator()(HWND h) {
		if (h) ::DestroyWindow(h);
	}
	void dialog_deleter::operator()(HWND h) {
		if (h) ::EndDialog(h, IDCANCEL);
	}

	template <class T2>
	hwnd_ptr<T2>::operator bool() const {
		return hwnd_;
	}
	template <class T2>
	hwnd_ptr<T2>::operator HWND() const {
		return hwnd_;
	}
	template <class T2>
	HWND hwnd_ptr<T2>::operator->() const {
		return hwnd_;
	}
	template <class T2>
	HWND hwnd_ptr<T2>::get() const {
		return hwnd_;
	}
	template <class T2>
	bool hwnd_ptr<T2>::equals(HWND h) {
		return hwnd_.equals(h);
	}
	template <class T2>
	void hwnd_ptr<T2>::reset(HWND h, SUBCLASSPROC p, DWORD_PTR data, UINT_PTR i) {
		HWND h_ = hwnd_.get();
		if (h_ && proc_) ::RemoveWindowSubclass(h_, proc_, id_);
		if (h && p)	::SetWindowSubclass(h, p, i, data);
		id_ = i;
		proc_ = p;
		hwnd_.reset(h);
	}

	template class FLAG_EXPORT hwnd_ptr<empty_deleter>;
	template class FLAG_EXPORT hwnd_ptr<hwnd_deleter>;
	template class FLAG_EXPORT hwnd_ptr<dialog_deleter>;
}

