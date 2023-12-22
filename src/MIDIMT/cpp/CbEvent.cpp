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
			cbd_ = d;
		}
		CbEventDataDeleter::~CbEventDataDeleter() {
			try {
				CbEventData* d = cbd_;
				cbd_ = nullptr;
				if (d) delete d;
			} catch (...) {}
		}
		CbEventData* CbEventDataDeleter::GetData() {
			return cbd_;
		}
		CbHWNDType CbEventDataDeleter::GetType() {
			if (!cbd_) return CbHWNDType::TYPE_CB_NONE;
			return cbd_->GetType();
		}

		template <typename T>
		T CbEventDataDeleter::Get() {
			if constexpr (std::is_same_v<std::wstring, T>) {
				if (!cbd_) return L"";
				return cbd_->Get<T>();
			}
			if constexpr (std::is_same_v<std::pair<DWORD, Common::MIDI::Mackie::MIDIDATA>, T>) {
				if (!cbd_) std::pair<DWORD, Common::MIDI::Mackie::MIDIDATA>();
				return cbd_->Get<T>();
			}
		}
		template std::wstring CbEventDataDeleter::Get<std::wstring>();
		template std::pair<DWORD, MIDI::Mackie::MIDIDATA> CbEventDataDeleter::Get<std::pair<DWORD, MIDI::Mackie::MIDIDATA>>();

		CbEventData::CbEventData(std::wstring s) {
			type_ = CbHWNDType::TYPE_CB_LOG;
			ws_ = std::wstring(s.begin(), s.end());
		}
		CbEventData::CbEventData(MIDI::Mackie::MIDIDATA& m, DWORD& t) {
			type_ = CbHWNDType::TYPE_CB_MON;
			data_ = std::pair<DWORD, MIDI::Mackie::MIDIDATA>(t, m);
		}
		CbHWNDType CbEventData::GetType() {
			return type_;
		}
		CbEventDataDeleter CbEventData::GetDeleter() {
			return CbEventDataDeleter(this);
		}

		template <typename T>
		T CbEventData::Get() {
			if constexpr (std::is_same_v<std::wstring, T>)
				return ws_;
			if constexpr (std::is_same_v<std::pair<DWORD, Common::MIDI::Mackie::MIDIDATA>, T>)
				return data_;
		}
		template std::wstring CbEventData::Get<std::wstring>();
		template std::pair<DWORD, MIDI::Mackie::MIDIDATA> CbEventData::Get<std::pair<DWORD, MIDI::Mackie::MIDIDATA>>();

		static inline void print__(HWND hwnd, std::wstring ws, bool singleline) {
			if (singleline)
				::SendMessageW(hwnd, WM_SETTEXT, (WPARAM)0, reinterpret_cast<LPARAM>(ws.c_str()));
			else {
				::SendMessageW(hwnd, EM_SETSEL, 0, -1);
				::SendMessageW(hwnd, EM_SETSEL, -1, -1);
				::SendMessageW(hwnd, EM_REPLACESEL, (WPARAM)0, reinterpret_cast<LPARAM>(ws.c_str()));
			}
		}

		CbEvent::CbEvent()
			: GetHwndCb([]() { return nullptr; }),
			  PluginCb::PluginCb(
				  this,
				  IO::PluginClassTypes::ClassMonitor,
				  (IO::PluginCbType::Out1Cb | IO::PluginCbType::LogoCb | IO::PluginCbType::ConfCb)) {

			PluginCb::out2_cb_ = [=](MIDI::MidiUnit&, DWORD) {};
			PluginCb::out1_cb_ = std::bind(static_cast<void(CbEvent::*)(Common::MIDI::Mackie::MIDIDATA, DWORD)>(&CbEvent::monitor_cb_), this, _1, _2);
			PluginCb::cnf_cb_ = std::bind(static_cast<void(CbEvent::*)(std::shared_ptr<JSON::MMTConfig>&)>(&CbEvent::set_config_cb_), this, _1);
			PluginCb::logo_cb_ = std::bind(static_cast<void(CbEvent::*)(const std::wstring&)>(&CbEvent::log_cb_), this, _1);
		}
		void CbEvent::Init() {
			ILOG = DLG_EVENT_LOG;
			IMON = DLG_EVENT_MONITOR;
		}
		void CbEvent::Init(int id) {
			ILOG = IMON = id;
		}
		void CbEvent::Init(int l, int m) {
			ILOG = l; IMON = m;
		}
		void CbEvent::Clear() {
			GetHwndCb = []() { return nullptr; };
		}
		void CbEvent::AddToLog(const std::wstring s) {
			log_cb_(s);
		}

		void CbEvent::log_cb_(const std::wstring& s) {
			try {
				if ((ILOG <= 0) || s.empty()) return;
				HWND hwnd;
				if (!(hwnd = GetHwndCb())) return;
				::PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(ILOG, 0), (LPARAM)reinterpret_cast<LPARAM>(new CbEventData(s)));
			} catch (...) {}
		}
		void CbEvent::monitor_cb_(MIDI::Mackie::MIDIDATA m, DWORD t) {
			try {
				if (IMON <= 0) return;
				HWND hwnd;
				if (!(hwnd = GetHwndCb())) return;
				::PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IMON, 0), (LPARAM)reinterpret_cast<LPARAM>(new CbEventData(m, t)));
			} catch (...) {}
		}
		void CbEvent::set_config_cb_(std::shared_ptr<JSON::MMTConfig>& mmt) {
			try {
				if (ILOG <= 0) return;
				log_cb_(log_string().to_log_format(
					__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_MIDIMT_UPCONFIG),
					mmt->config, L" status: ", Utils::BOOL_to_string(!mmt->empty()))
				);
			} catch (...) {}
		}

		void CbEvent::ToLog(HWND hwnd, CbEventData* data, bool singleline) {
			try {
				if (!hwnd || !data || (data->GetType() != CbHWNDType::TYPE_CB_LOG)) return;
				std::wstring ws = (singleline) ? data->Get<std::wstring>() :
					(log_string() << data->Get<std::wstring>().c_str() << L"\r\n").str();

				print__(hwnd, ws, singleline);
			} catch (...) {}
		}
		void CbEvent::ToMonitor(HWND hwnd, CbEventData* data, bool singleline) {
			try {
				if (!hwnd || !data || (data->GetType() != CbHWNDType::TYPE_CB_MON)) return;
				std::pair<DWORD, MIDI::Mackie::MIDIDATA> p = data->Get<std::pair<DWORD, MIDI::Mackie::MIDIDATA>>();
				std::wstring ws = (log_string() << p.second.UiDump()
												<< L"," << UI::UiUtils::GetBlank(p.second.value())
												<< L"\tCount offset: " << p.first
												<< (singleline ? L"" : L"\r\n")
					);
				print__(hwnd, ws, singleline);
			} catch (...) {}
		}
	}
}