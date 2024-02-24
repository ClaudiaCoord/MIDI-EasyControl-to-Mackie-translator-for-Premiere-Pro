/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {

	template<typename T1>
	class local_ptr_deleter {
	public:
		T1 data;

		local_ptr_deleter(T1 d) : data(d) {}
		~local_ptr_deleter() {
			try { if (data) ::LocalFree(data); } catch (...) {}
		}
		const bool empty() const {
			return (data == nullptr);
		}
	};

	template<typename T1>
	class class_ptr_deleter {
	public:
		T1 data;

		class_ptr_deleter(T1 d) : data(d) {}
		~class_ptr_deleter() {
			if (data != nullptr) delete data;
		}
		const bool empty() const {
			return (data == nullptr);
		}
	};

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
	struct [[deprecated("Using hwnd_ptr<DELETER>!")]] default_hwnd_deleter {
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
	struct default_handle_deleter {
		void operator()(HANDLE h) {
			if (h != nullptr) {
				::CloseHandle(h);
			}
		}
	};
	struct default_handleio_deleter {
		void operator()(HANDLE h) {
			if (h != nullptr) {
				::CancelIo(h);
				::CloseHandle(h);
			}
		}
	};

	template <typename T1, class T2 = default_empty_deleter<T1>>
	class handle_ptr {
	private:
		T1 h_;
	public:

		handle_ptr() : h_(nullptr) {
		}
		~handle_ptr() {
			reset();
		}
		operator bool() const {
			return h_ != nullptr;
		}
		operator T1() const {
			return h_;
		}
		T1 operator->() const {
			return h_;
		}
		bool equals(T1 h) {
			return h == h_;
		}
		T1 get() const {
			return h_;
		}
		void reset(T1 h = nullptr) {
			T2 d{}; d(h_);
			h_ = h;
		}
		T1 release() {
			T1 h = h_;
			h_ = nullptr;
			return h;
		}
	};
}

