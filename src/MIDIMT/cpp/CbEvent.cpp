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

		using namespace std::placeholders;

		CbEvent::CbEvent()
			: LogNotify([]() {}), MonitorNotify([]() {}), MonitorData([](Common::MIDI::Mackie::MIDIDATA&, DWORD&) -> bool { return false; }) {
			Set(
				MIDI::ClassTypes::ClassMonitor,
				std::bind(static_cast<const bool(CbEvent::*)(Common::MIDI::Mackie::MIDIDATA&, DWORD&)>(&CbEvent::MonitorCb), this, _1, _2),
				[=](MIDI::MidiUnit&, DWORD&) -> bool { return false; },
				std::bind(static_cast<void(CbEvent::*)(const std::wstring&)>(&CbEvent::LogCb), this, _1)
			);
		}

		void CbEvent::Clear() {
			try {
				lsl__.reset();
				lsl__.reset_buffer();
				lsm__.reset();
				lsm__.reset_buffer();
			} catch (...) {}
		}
		void CbEvent::AddToLog(std::wstring s) {
			LogCb(s);
		}

		void CbEvent::LogCb(const std::wstring& s) {
			if (s.empty()) return;
			base_log__.push(s);
			LogNotify();
		}
		const bool CbEvent::MonitorCb(Common::MIDI::Mackie::MIDIDATA& m, DWORD& t) {
			base_monitor__.push(std::pair<DWORD, Common::MIDI::Mackie::MIDIDATA>(t, m));
			MonitorNotify();
			return true;
		}

		void CbEvent::LogLoop(HWND hwnd, UINT id) {
			try {
				if (hwnd == nullptr) return;

				std::wstring ws;
				if (IsLogOneLine) {
					while (!base_log__.empty()) {
						ws = base_log__.front();
						base_log__.pop();
					}
					if (ws.empty()) return;
				} else {
					while (!base_log__.empty()) {
						lsl__ << base_log__.front() << L"\r\n";
						base_log__.pop();
					}
					ws = lsl__.str();
				}
				::SetDlgItemTextW(hwnd, id, ws.c_str());
			} catch (...) {}
		}
		void CbEvent::MonitorLoop(HWND hwnd, UINT id) {
			try {
				if (hwnd == nullptr) return;

				std::wstring ws;
				if (IsMonitorOneLine) {
					std::pair<DWORD, Common::MIDI::Mackie::MIDIDATA> p;
					while (!base_monitor__.empty()) {
						p = base_monitor__.front();
						base_monitor__.pop();
						MonitorData(p.second, p.first);
					}
					if (p.first == 0U) return;
					ws = (log_string() << p.second.UiDump() << L"," << Gui::GetBlank(p.second.value()) << L"\tTime offset: " << Utils::MILLISECONDS_to_string(p.first) << L"\r\n");
				} else {
					while (!base_monitor__.empty()) {
						auto p = base_monitor__.front();
						base_monitor__.pop();
						lsm__ << p.second.UiDump() << L"," << Gui::GetBlank(p.second.value()) << L"\tTime offset: " << Utils::MILLISECONDS_to_string(p.first) << L"\r\n";
						MonitorData(p.second, p.first);
					}
					ws = lsm__.str();
				}

				::SetDlgItemTextW(hwnd, id, ws.c_str());
				lsm__.reset_buffer();
			} catch (...) {}
		}
	}
}