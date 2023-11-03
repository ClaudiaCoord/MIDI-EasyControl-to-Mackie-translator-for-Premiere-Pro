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
#include <cwctype>
#include <regex>

#if defined(_DEBUG)
#include <stacktrace>
#endif

namespace Common {

	#include "..\..\Common\rc\resource_version.h"

	using namespace std::placeholders;
	using namespace std::string_view_literals;
	constexpr wchar_t DEFAULT_LOG_NAME[] = L"MIDIMT.log";

	std::wstring operator"" _logw(const char8_t* s, std::size_t z) {
		return Utils::to_string(s, z);
	}
	std::string operator"" _logs(const char8_t* s, std::size_t) {
		return reinterpret_cast<const char*>(s);
	}

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
	std::wstring log_string::str() const {
		return ss__.str();
	}
	const wchar_t* log_string::c_str() {
		buffer__ = ss__.str();
		return buffer__.c_str();
	}

	log_string& log_string::to_log_method(const wchar_t* s, int i) {
		const std::wstring w = std::regex_replace(s, std::wregex(L"::"), L".");
		ss__ << L"[" << (w.starts_with(L"Common.") ? w.substr(7).c_str() : w.c_str());
		if (i > 0) ss__ << L"|" << i;
		ss__ << L"]: ";
		return *this;
	}
	log_string& log_string::to_log_method(const wchar_t* s1, const wchar_t* s2) {
		to_log_method(s1, -1);
		ss__ << L"->";
		to_log_method(s2, -1);
		return *this;
	}
	log_string& log_string::to_log_method(const wchar_t* s) {
		return to_log_method(s, -1);
	}
	log_string& log_string::to_log_method(const std::wstring s) {
		return to_log_method(s.c_str(), -1);
	}

	bool log_string::is_string_empty(const std::wstring& s) {
		if (s.empty()) return true;
		return std::all_of(s.begin(), s.end(), [](wchar_t ch) {
			return std::iswspace(ch);
		});
	}
	bool log_string::is_string_empty(const std::wstring_view& ss) {
		const std::wstring s = ss.data();
		return log_string::is_string_empty(s);
	}
	bool log_string::is_string_empty(const std::wstringstream& ss) {
		const std::wstring s = ss.str();
		return log_string::is_string_empty(s);
	}
	bool log_string::is_string_empty(log_string& ls) {
		const std::wstring s = ls.str();
		return log_string::is_string_empty(s);
	}

	template<>
	log_string& log_string::operator<< (std::nullptr_t) {
		return *this;
	}
	template<>
	log_string& log_string::operator<< (common_error err) {
		if ((err.code() == 0) && (err.message().empty())) return *this;
		ss__ << err.what();
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::wstringstream wss) {
		if (wss.tellp() != std::streampos(0)) ss__ << wss.str();
		return *this;
	}
	template<>
	log_string& log_string::operator<< (Common::MIDI::ClassTypes t) {
		ss__ << Utils::to_string(t);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (Common::MIDI::MidiUnitType t) {
		ss__ << Utils::to_string(t);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (Common::MIDI::MidiUnitScene t) {
		ss__ << Utils::to_string(t);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::exception err) {
		ss__ << Utils::to_string(err);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::runtime_error err) {
		ss__ << Utils::to_string(err);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::error_code err) {
		ss__ << Utils::to_string(err);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::future_error err) {
		ss__ << Utils::to_string(err);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::filesystem::filesystem_error err) {
		ss__ << Utils::to_string(err);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::string s) {
		if (!s.empty()) ss__ << Utils::to_string(s);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::wstring s) {
		if (!s.empty()) ss__ << s;
		return *this;
	}
	template<>
	log_string& log_string::operator<< (log_string ls) {
		ss__ << ls.str();
		return *this;
	}
	template<>
	log_string& log_string::operator<< (GUID g) {
		ss__ << Utils::to_string(g);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (uint8_t u) {
		ss__ << (int)static_cast<int>(u);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (uint16_t u) {
		ss__ << (long)static_cast<long>(u);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (uint32_t u) {
		ss__ << (unsigned long)static_cast<unsigned long>(u);
		return *this;
	}

	template log_string& log_string::operator<< (std::nullptr_t);
	template log_string& log_string::operator<< (common_error);
	template log_string& log_string::operator<< (std::wstringstream);
	template log_string& log_string::operator<< (Common::MIDI::ClassTypes);
	template log_string& log_string::operator<< (Common::MIDI::MidiUnitType);
	template log_string& log_string::operator<< (Common::MIDI::MidiUnitScene);

	template log_string& log_string::operator<< (std::exception);
	template log_string& log_string::operator<< (std::runtime_error);
	template log_string& log_string::operator<< (std::error_code);
	template log_string& log_string::operator<< (std::future_error);
	template log_string& log_string::operator<< (std::filesystem::filesystem_error);

	template log_string& log_string::operator<< (std::string);
	template log_string& log_string::operator<< (std::wstring);

	template log_string& log_string::operator<< (GUID);
	template log_string& log_string::operator<< (uint8_t);
	template log_string& log_string::operator<< (uint16_t);
	template log_string& log_string::operator<< (uint32_t);

	to_log::to_log() {
		filelog_fun__ = std::bind(static_cast<void(to_log::*)(const std::wstring&)>(&to_log::logtofile), this, _1);
	}
	to_log::~to_log() {
		closelog();
	}

	std::wstring to_log::logname() {
		return DEFAULT_LOG_NAME;
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
				logtofile((log_string() << VER_GUI_EN << L" | " << VER_COPYRIGHT));
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
				pushlog_(L"log close.");
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
			#if defined(_DEBUG)
			std::wstringstream ws;
			ws << L"* [FILE] FLOG=" << std::boolalpha << filelog__.is_open() << L", MSG = [" << s << L"]\n";
			OutputDebugStringW(ws.str().c_str());
			#endif

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
		#if defined(_DEBUG)
		if (log_string::is_string_empty(s)) {
			std::stringstream s1;
			s1 << "\n* stacktrace (empty log string)\n";
			for (auto& a : std::stacktrace::current()) {
				s1 << "\t- [" << a.description() << "], line=" << a.source_line() << ", file=" << a.source_file() << "\n";
			}
			OutputDebugStringA(s1.str().c_str());
			return;
		}
		std::wstringstream w2;
		w2 << L"* [PUSH] FLOG=" << std::boolalpha << filelog__.is_open() << L", MSG = [" << s << L"]\n";
		OutputDebugStringW(w2.str().c_str());
		#endif
		event__.send(s);
	}

	to_log& to_log::operator<< (std::wstringstream& wss) {
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

	template<>
	to_log& to_log::operator<< (const log_string& ls) {
		pushlog_(ls.str());
		return *this;
	}
	template<>
	to_log& to_log::operator<< (const common_error& err) {
		if ((err.code() == 0) && (err.message().empty())) return *this;
		pushlog_(err.what());
		return *this;
	}
	template<>
	to_log& to_log::operator<< (const std::exception& err) {
		pushlog_(Utils::to_string(err));
		return *this;
	}
	template<>
	to_log& to_log::operator<< (const std::runtime_error& err) {
		pushlog_(Utils::to_string(err));
		return *this;
	}
	template<>
	to_log& to_log::operator<< (const std::future_error& err) {
		pushlog_(Utils::to_string(err));
		return *this;
	}
	template<>
	to_log& to_log::operator<< (const std::error_code& err) {
		pushlog_(Utils::to_string(err));
		return *this;
	}
	template<>
	to_log& to_log::operator<< (const std::filesystem::filesystem_error& err) {
		pushlog_(Utils::to_string(err));
		return *this;
	}
	template<>
	to_log& to_log::operator<< (const std::string& s) {
		if (!s.empty()) pushlog_(std::wstring(s.begin(), s.end()));
		return *this;
	}
	template<>
	to_log& to_log::operator<< (const std::wstring& s) {
		if (!s.empty()) pushlog_(s);
		return *this;
	}
	template<>
	to_log& to_log::operator<< (const std::wstring_view& s) {
		if (!s.empty()) pushlog_(std::wstring(s));
		return *this;
	}
	template<>
	to_log& to_log::operator<< (const GUID& g) {
		pushlog_(Utils::to_string(g));
		return *this;
	}

	template to_log& to_log::operator<< (const std::exception&);
	template to_log& to_log::operator<< (const std::runtime_error&);
	template to_log& to_log::operator<< (const std::future_error&);
	template to_log& to_log::operator<< (const std::error_code&);
	template to_log& to_log::operator<< (const std::filesystem::filesystem_error&);
	template to_log& to_log::operator<< (const std::string&);
	template to_log& to_log::operator<< (const std::wstring&);
	template to_log& to_log::operator<< (const std::wstring_view&);
	template to_log& to_log::operator<< (const GUID&);

	log_auto::log_auto(logFnType fn) : fun__(fn) {
		to_log::Get().registred(fun__);
	}
	log_auto::~log_auto() {
		to_log::Get().unregistred(fun__);
	}
}