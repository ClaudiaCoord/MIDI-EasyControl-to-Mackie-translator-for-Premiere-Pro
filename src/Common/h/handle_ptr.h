/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {

	template <typename T1>
	struct default_empty_deleter {
		void operator()(T1 h) {}
	};
	template <typename T1>
	struct default_class_deleter {
		void operator()(T1* h) {
			if (h != nullptr) delete h;
		}
	};
	template <typename T1>
	struct default_hwnd_deleter {
		void operator()(T1 h) {
			if (h != nullptr) ::DestroyWindow(h);
		}
	};
	template <typename T1>
	struct default_hgdiobj_deleter {
		void operator()(T1 h) {
			if (h != nullptr) ::DeleteObject(h);
		}
	};

	template <typename T1, class T2 = default_empty_deleter<T1>>
	class handle_ptr
	{
	private:
		T1 h__;
	public:

		handle_ptr() : h__(nullptr) {
		}
		~handle_ptr() {
			reset();
		}
		operator bool() const {
			return h__ != nullptr;
		}
		operator T1() const {
			return h__;
		}
		T1 operator->() {
			return h__;
		}
		bool equals(T1 h) {
			return h == h__;
		}
		T1 get() const {
			return h__;
		}
		void reset(T1 h = nullptr) {
			T2 d{}; d(h__);
			h__ = h;
		}
		T1 release() {
			T1 h = h__;
			h__ = nullptr;
			return h;
		}
	};
}

