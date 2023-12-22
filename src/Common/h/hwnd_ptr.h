/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ UI/Viewer/Editor interface.
	(c) CC 2023, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {

	struct FLAG_EXPORT empty_deleter {
		void operator()(HWND);
	};
	struct FLAG_EXPORT hwnd_deleter {
		void operator()(HWND h);
	};
	struct FLAG_EXPORT dialog_deleter {
		void operator()(HWND h);
	};

	template <class T2 = empty_deleter>
	class FLAG_EXPORT hwnd_ptr {
	private:
		handle_ptr<HWND,T2> hwnd_{};
		SUBCLASSPROC proc_{ nullptr };
		UINT_PTR id_{ 0U };

	public:

		hwnd_ptr() = default;
		~hwnd_ptr() = default;

		operator bool() const;
		operator HWND() const;
		HWND operator->() const;
		HWND get() const;
		bool equals(HWND);
		void reset(HWND = nullptr, SUBCLASSPROC = nullptr, DWORD_PTR = 0U, UINT_PTR = 0U);
	};

	FLAG_EXTERN template class FLAG_IMPORT hwnd_ptr<empty_deleter>;
	FLAG_EXTERN template class FLAG_IMPORT hwnd_ptr<hwnd_deleter>;
	FLAG_EXTERN template class FLAG_IMPORT hwnd_ptr<dialog_deleter>;
}
