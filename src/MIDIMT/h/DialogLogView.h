
#pragma once

namespace Common {
	namespace MIDIMT {


		struct hwnd_ptr_deleter {
			void operator()(HWND h) {
				try {
					if (h)
						::DestroyWindow(h);
				} catch (...) {}
			}
		};

		class DialogLogView {
		private:
			handle_ptr<HWND, hwnd_ptr_deleter> hwnd_{};
			std::unique_ptr<UI::ScintillaBox> editor_{};
			CbEvent mcb_{};

			static LRESULT CALLBACK event_edit_(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

		public:

			DialogLogView();
			~DialogLogView();

			const bool IsRunOnce();
			void SetFocus();
			void EventLog(CbEventData*);

			void InitDialog(HWND);
			void EndDialog();

			void zoomin();
			void zoomout();
			void gostart();
			void goend();
			void clear();

			void sc_event(LPNMHDR);
		};
	}
}