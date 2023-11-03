/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"

namespace Common {
	namespace MIDIMT {

		DialogLogView::DialogLogView() {
			mcb_.Init(IDC_IEVENT_LOG, IDC_IEVENT_MONITOR);
			mcb_.HwndCb = [=]() { return hwnd_.get(); };
		}
		DialogLogView::~DialogLogView() {
			mcb_.Clear();
		}

		const bool DialogLogView::IsRunOnce() {
			return !hwnd_;
		}
		void DialogLogView::SetFocus() {
			if (hwnd_) (void) ::SetFocus(hwnd_.get());
		}
		void DialogLogView::EventLog(CbEventData* data) {
			try {
				if (data == nullptr)  return;
				CbEventDataDeleter d = data->GetDeleter();
				if (!hwnd_ || !editor_) return;
				if (data->GetType() != CbHWNDType::TYPE_CB_LOG) return;
				std::wstring ws = (log_string() << data->Get<std::wstring>().c_str() << L"\n").str();
				editor_->set(ws);
			} catch (...) {}
		}

		void DialogLogView::InitDialog(HWND h) {
			try {
				editor_ = std::make_unique<UI::ScintillaBox>();

				RECT r{};
				::GetClientRect(h, &r);

				HINSTANCE hi = LangInterface::Get().GetMainHinstance();
				HWND h_ = CreateWindowExW(0,
					L"Scintilla", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN,
					r.left, r.top, r.right - r.left, r.bottom - r.top,
					h, 0, hi, 0);

				::SetWindowSubclass(h_, &DialogLogView::event_edit_, 0, reinterpret_cast<DWORD_PTR>(this));
				
				hwnd_.reset(h_);
				std::wstring path{};
				{
					wchar_t cpath[MAX_PATH + 1]{};
					if (::GetModuleFileNameW(hi, cpath, MAX_PATH) > 0) {
						std::filesystem::path p = std::filesystem::path(cpath).parent_path();
						p.append(to_log::Get().logname());
						if (std::filesystem::exists(p)) path = p.wstring();
					}
				}
				editor_->init(hwnd_, path);
				to_log::Get().registred(mcb_.GetCbLog());
				if (!common_config::Get().Local.IsMidiBridgeRun())
					mcb_.AddToLog(LangInterface::Get().GetString(IDS_DLG_MSG12));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void DialogLogView::EndDialog() {
			try {
				to_log::Get().unregistred(mcb_.GetCbLog());
				::RemoveWindowSubclass(hwnd_, &DialogLogView::event_edit_, 0);
				hwnd_.reset();
				editor_.reset();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void DialogLogView::zoomin() {
			if (editor_) editor_->zoomin();
		}
		void DialogLogView::zoomout() {
			if (editor_) editor_->zoomout();
		}
		void DialogLogView::gostart() {
			if (editor_) editor_->gostart();
		}
		void DialogLogView::goend() {
			if (editor_) editor_->goend();
		}
		void DialogLogView::clear() {
			if (editor_) editor_->clear();
		}

		void DialogLogView::sc_event(LPNMHDR hdr) {
			if (!hwnd_ || !hdr || !editor_) return;
			try {
				if (hdr->hwndFrom != hwnd_) return;
				editor_->sc_event(hdr);
			} catch (...) {}
		}

		LRESULT CALLBACK DialogLogView::event_edit_(HWND hwnd, UINT m, WPARAM w, LPARAM l, UINT_PTR sc, DWORD_PTR data) {
			switch (m) {
				case WM_COMMAND: {
					switch (LOWORD(w)) {
						case IDC_IEVENT_LOG: {
							DialogLogView* dlgl = reinterpret_cast<DialogLogView*>(data);
							if (dlgl)
								dlgl->EventLog(reinterpret_cast<CbEventData*>(l));
							return true;
						}
						default: break;
					}
					break;
				}
				default: break;
			}
			return ::DefSubclassProc(hwnd, m, w, l);
		}
	}
}
