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

		CbEventDataDeleter::CbEventDataDeleter(CbEventData* d) {
			cbd__ = d;
		}
		CbEventDataDeleter::~CbEventDataDeleter() {
			try {
				CbEventData* d = cbd__;
				cbd__ = nullptr;
				if (d != nullptr) delete d;
			} catch (...) {}
		}
		CbEventData* CbEventDataDeleter::GetData() {
			return cbd__;
		}
		CbHWNDType CbEventDataDeleter::GetType() {
			if (cbd__ == nullptr) return CbHWNDType::TYPE_CB_NONE;
			return cbd__->GetType();
		}

		template <typename T>
		T CbEventDataDeleter::Get() {
			if constexpr (std::is_same_v<std::wstring, T>) {
				if (cbd__ == nullptr) return L"";
				return cbd__->Get<T>();
			}
			if constexpr (std::is_same_v<std::pair<DWORD, Common::MIDI::Mackie::MIDIDATA>, T>) {
				if (cbd__ == nullptr) std::pair<DWORD, Common::MIDI::Mackie::MIDIDATA>();
				return cbd__->Get<T>();
			}
		}
		template std::wstring CbEventDataDeleter::Get<std::wstring>();
		template std::pair<DWORD, MIDI::Mackie::MIDIDATA> CbEventDataDeleter::Get<std::pair<DWORD, MIDI::Mackie::MIDIDATA>>();

		CbEventData::CbEventData(std::wstring s) {
			type__ = CbHWNDType::TYPE_CB_LOG;
			ws__ = std::wstring(s.begin(), s.end());
		}
		CbEventData::CbEventData(MIDI::Mackie::MIDIDATA& m, DWORD& t) {
			type__ = CbHWNDType::TYPE_CB_MON;
			data__ = std::pair<DWORD, MIDI::Mackie::MIDIDATA>(t, m);
		}
		CbHWNDType CbEventData::GetType() {
			return type__;
		}
		CbEventDataDeleter CbEventData::GetDeleter() {
			return CbEventDataDeleter(this);
		}

		template <typename T>
		T CbEventData::Get() {
			if constexpr (std::is_same_v<std::wstring, T>)
				return ws__;
			if constexpr (std::is_same_v<std::pair<DWORD, Common::MIDI::Mackie::MIDIDATA>, T>)
				return data__;
		}
		template std::wstring CbEventData::Get<std::wstring>();
		template std::pair<DWORD, MIDI::Mackie::MIDIDATA> CbEventData::Get<std::pair<DWORD, MIDI::Mackie::MIDIDATA>>();


		static inline void send_(HWND hwnd, std::wstring ws, bool singleline) {
			if (singleline)
				::SendMessageW(hwnd, WM_SETTEXT, (WPARAM)0, reinterpret_cast<LPARAM>(ws.c_str()));
			else {
				::SendMessageW(hwnd, EM_SETSEL, 0, -1);
				::SendMessageW(hwnd, EM_SETSEL, -1, -1);
				::SendMessageW(hwnd, EM_REPLACESEL, (WPARAM)0, reinterpret_cast<LPARAM>(ws.c_str()));
			}
		}

		CbEvent::CbEvent()
			: HwndCb([]() { return nullptr; }) {
			Set(
				MIDI::ClassTypes::ClassMonitor,
				std::bind(static_cast<const bool(CbEvent::*)(Common::MIDI::Mackie::MIDIDATA&, DWORD&)>(&CbEvent::MonitorCb), this, _1, _2),
				[=](MIDI::MidiUnit&, DWORD&) -> bool { return false; },
				std::bind(static_cast<void(CbEvent::*)(const std::wstring&)>(&CbEvent::LogCb), this, _1)
			);
		}
		void CbEvent::Init(int l, int m) {
			ILOG = l; IMON = m;
		}
		void CbEvent::Clear() {
			HwndCb = []() { return nullptr; };
		}
		void CbEvent::AddToLog(std::wstring s) {
			LogCb(s);
		}

		void CbEvent::LogCb(const std::wstring& s) {
			try {
				do {
					if ((ILOG < 0) || s.empty()) break;
					HWND hwnd = HwndCb();
					if (hwnd == nullptr) break;
					::PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(ILOG, 0), (LPARAM)reinterpret_cast<LPARAM>(new CbEventData(s)));
				} while (0);
			} catch (...) {}
		}
		const bool CbEvent::MonitorCb(MIDI::Mackie::MIDIDATA& m, DWORD& t) {
			try {
				do {
					if (IMON < 0) break;
					HWND hwnd = HwndCb();
					if (hwnd == nullptr) break;
					::PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IMON, 0), (LPARAM)reinterpret_cast<LPARAM>(new CbEventData(m, t)));
					return true;
				} while (0);
			} catch (...) {}
			return false;
		}

		void CbEvent::ToLog(HWND hwnd, CbEventData* data, bool singleline) {
			try {
				if ((hwnd == nullptr) || (data == nullptr) || data->GetType() != CbHWNDType::TYPE_CB_LOG) return;
				std::wstring ws = (singleline) ? data->Get<std::wstring>() :
					(log_string() << data->Get<std::wstring>().c_str() << L"\r\n").str();

				send_(hwnd, ws, singleline);
			} catch (...) {}
		}
		void CbEvent::ToMonitor(HWND hwnd, CbEventData* data, bool singleline) {
			try {
				if ((hwnd == nullptr) || (data == nullptr) || data->GetType() != CbHWNDType::TYPE_CB_MON) return;
				std::pair<DWORD, MIDI::Mackie::MIDIDATA> p = data->Get<std::pair<DWORD, MIDI::Mackie::MIDIDATA>>();
				std::wstring ws = (log_string() << p.second.UiDump()
												<< L"," << Gui::GetBlank(p.second.value())
												<< L"\tTime offset: " << Utils::MILLISECONDS_to_string(p.first)
												<< (singleline ? L"" : L"\r\n")
					);
				send_(hwnd, ws, singleline);
			} catch (...) {}
		}
	}
}