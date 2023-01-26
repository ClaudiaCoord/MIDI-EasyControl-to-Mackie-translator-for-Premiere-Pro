/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "global.h"
#include <chrono>
#include <locale>

namespace Common {

	using namespace std::placeholders;
	using namespace std::string_view_literals;
	constexpr wchar_t DEFAULT_LOG_NAME[] = L"MIDIMT.log";

	to_log to_log::tolog__;

	void log_string::reset() {
		ss__.clear();
		ss__.str(std::wstring());
		buffer__ = std::wstring();
	}
	void log_string::reset_buffer() {
		buffer__ = std::wstring();
	}

	log_string::operator std::wstring() const {
		return ss__.str();
	}
	std::wstring log_string::str() {
		return ss__.str();
	}
	const wchar_t* log_string::c_str() {
		buffer__ = ss__.str();
		return buffer__.c_str();
	}

	log_string& log_string::operator<< (std::exception& err) {
		ss__ << Utils::to_string(err);
		return *this;
	}
	log_string& log_string::operator<< (std::runtime_error& err) {
		ss__ << Utils::to_string(err);
		return *this;
	}
	log_string& log_string::operator<< (std::error_code& err) {
		ss__ << Utils::to_string(err);
		return *this;
	}
	log_string& log_string::operator<< (std::future_error& err) {
		ss__ << Utils::to_string(err);
		return *this;
	}
	log_string& log_string::operator<< (runtime_werror& err) {
		if (!err.error().empty()) ss__ << err.error();
		return *this;
	}
	log_string& log_string::operator<< (std::nullptr_t) {
		return *this;
	}
	log_string& log_string::operator<< (std::wstringstream& wss) {
		if (wss.tellp() != std::streampos(0)) ss__ << wss.str();
		return *this;
	}
	log_string& log_string::operator<< (const std::string& s) {
		if (!s.empty()) ss__ << Utils::to_string(s);
		return *this;
	}
	log_string& log_string::operator<< (const GUID& g) {
		ss__ << Utils::to_string(g);
		return *this;
	}
	log_string& log_string::operator<< (const Common::MIDI::ClassTypes t) {
		ss__ << Utils::to_string(t);
		return *this;
	}
	log_string& log_string::operator<< (const Common::MIDI::MidiUnitType t) {
		ss__ << Utils::to_string(t);
		return *this;
	}
	log_string& log_string::operator<< (const Common::MIDI::MidiUnitScene t) {
		ss__ << Utils::to_string(t);
		return *this;
	}

	log_string& log_string::operator<< (uint8_t& u) {
		ss__ << (int)static_cast<int>(u);
		return *this;
	}
	log_string& log_string::operator<< (uint16_t& u) {
		ss__ << (long)static_cast<long>(u);
		return *this;
	}
	log_string& log_string::operator<< (uint32_t& u) {
		ss__ << (unsigned long)static_cast<unsigned long>(u);
		return *this;
	}

	to_log::to_log() : isdispose(false) {
		filelog_fun__ = std::bind(static_cast<void(to_log::*)(const std::wstring&)>(&to_log::logtofile), this, _1);
	}
	to_log::~to_log() {
		if (isdispose) return;
		isdispose = true;
		closelog();
	}

	static uint32_t filelog_cb_id__ = 0U;
	to_log& to_log::Get() {
		return std::ref(to_log::tolog__);
	}

	uint32_t to_log::registred(logFnType fn) {
		return event__.add(fn);
	}
	void to_log::unregistred(uint32_t idx) {
		event__.remove(idx);
	}
	void to_log::unregistred(logFnType fn) {
		event__.remove(fn);
	}

	void to_log::end() {
		try {
			closelog();
			event__.clear();
		} catch (...) {}
	}

	void to_log::flush() {
		try {
			if (filelog__.is_open())
				filelog__.flush();
		} catch (...) {}
	}
	bool to_log::filelog() {
		bool b = Common::common_config::Get().Registry.GetLogWrite();
		if (b) filelog(b, DEFAULT_LOG_NAME);
		return b;
	}
	void to_log::filelog(bool b) {
		filelog(b, DEFAULT_LOG_NAME);
	}
	void to_log::filelog(bool b, const std::wstring ws) {
		try {

			if (b && !filelog__.is_open()) {
				filelog__ = std::wofstream(ws, std::ios_base::trunc);
				filelog__.imbue(std::locale(""));
				filelog_cb_id__ = registred(filelog_fun__);
			} else if (!b && filelog__.is_open()) closelog();
			Common::common_config::Get().Registry.SetLogWrite(b);

		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}
	void to_log::closelog() {
		try {
			if (filelog_cb_id__ > 0U) unregistred(filelog_cb_id__);
			else					  unregistred(filelog_fun__);
			if (filelog__.is_open()) {
				filelog__.flush();
				filelog__.close();
				filelog_cb_id__ = 0U;
			}
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}
	void to_log::logtofile(const std::wstring& s) {
		try {
			if (filelog__.is_open()) {
				auto dat = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				std::tm buf{};
				::localtime_s(&buf, &dat);

				filelog__ << std::put_time(&buf, L"[%m-%d -> %H:%M:%S] ") << s.c_str() << L"\n";
				filelog__.flush();
			}
		} catch (...) {}
	}
	void to_log::pushlog_(const std::wstring s) {
		event__.send(s);
	}

	to_log& to_log::operator<< (std::exception& err) {
		pushlog_(Utils::to_string(err));
		return *this;
	}
	to_log& to_log::operator<< (std::runtime_error& err) {
		pushlog_(Utils::to_string(err));
		return *this;
	}
	to_log& to_log::operator<< (std::future_error& err) {
		pushlog_(Utils::to_string(err));
		return *this;
	}
	to_log& to_log::operator<< (runtime_werror & err) {
		if (!err.error().empty()) pushlog_(err.error());
		return *this;
	}
	to_log& to_log::operator<< (const std::string & s) {
		if (!s.empty()) pushlog_(std::wstring(s.begin(), s.end()));
		return *this;
	}
	to_log& to_log::operator<< (const std::wstring & ws) {
		if (!ws.empty()) pushlog_(ws);
		return *this;
	}
	to_log& to_log::operator<< (const std::wstring_view & ws) {
		if (!ws.empty()) pushlog_(std::wstring(ws));
		return *this;
	}
	to_log& to_log::operator<< (std::wstringstream & wss) {
		if (wss.tellp() != std::streampos(0)) pushlog_(wss.str());
		return *this;
	}
	to_log& to_log::operator<< (const wchar_t s[]) {
		if (CHECK_LPWSTRING(s)) pushlog_(s);
		return *this;
	}
	to_log& to_log::operator<< (const LPWSTR s) {
		pushlog_(Utils::to_string(s));
		return *this;
	}
	to_log& to_log::operator<< (const GUID& g) {
		pushlog_(Utils::to_string(g));
		return *this;
	}

	log_auto::log_auto(logFnType fn) : fun__(fn) {
		to_log::Get().registred(fun__);
	}
	log_auto::~log_auto() {
		to_log::Get().unregistred(fun__);
	}

}