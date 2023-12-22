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

	void log_string::to_buffer_() {
		if (buffer_.empty())
			buffer_ = ss_.str();
	}

	void log_string::reset() {
		std::wstring s{};
		ss_.clear();
		ss_.str(s);
		buffer_ = s;
	}
	void log_string::reset_buffer() {
		buffer_ = std::wstring();
	}

	log_string::operator std::wstring() const {
		return ss_.str();
	}
	std::wstring log_string::get() {
		to_buffer_();
		return buffer_;
	}
	std::wstring log_string::str() const {
		return buffer_.empty() ? ss_.str() : buffer_;
	}
	const wchar_t* log_string::c_str() {
		to_buffer_();
		return buffer_.c_str();
	}
	const bool log_string::empty() {
		to_buffer_();
		return buffer_.empty();
	}

	log_string& log_string::to_log_method(const wchar_t* s, int i) {
		const std::wstring w = std::regex_replace(std::regex_replace(s, std::wregex(L"::"), L"."), std::wregex(L"_+$"), L"");
		ss_ << L"[" << (w.starts_with(L"Common.") ? w.substr(7).c_str() : w.c_str());
		if (i > 0) ss_ << L"|" << i;
		ss_ << L"]: ";
		return *this;
	}
	log_string& log_string::to_log_method(const wchar_t* s1, const wchar_t* s2) {
		to_log_method(s1, -1);
		ss_ << L"->";
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
		ss_ << err.what();
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::wstringstream wss) {
		if (wss.tellp() != std::streampos(0)) ss_ << wss.str();
		return *this;
	}
	template<>
	log_string& log_string::operator<< (IO::PluginClassTypes t) {
		ss_ << Utils::to_string(t);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (Common::MIDI::MidiUnitType t) {
		ss_ << Utils::to_string(t);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (Common::MIDI::MidiUnitScene t) {
		ss_ << Utils::to_string(t);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::exception err) {
		ss_ << Utils::to_string(err);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::runtime_error err) {
		ss_ << Utils::to_string(err);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::error_code err) {
		ss_ << Utils::to_string(err);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::future_error err) {
		ss_ << Utils::to_string(err);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::filesystem::filesystem_error err) {
		ss_ << Utils::to_string(err);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::string s) {
		if (!s.empty()) ss_ << Utils::to_string(s);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (std::wstring s) {
		if (!s.empty()) ss_ << s;
		return *this;
	}
	template<>
	log_string& log_string::operator<< (log_string ls) {
		ss_ << ls.str();
		return *this;
	}
	template<>
	log_string& log_string::operator<< (GUID g) {
		ss_ << Utils::to_string(g);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (uint8_t u) {
		ss_ << (int)static_cast<int>(u);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (uint16_t u) {
		ss_ << (long)static_cast<long>(u);
		return *this;
	}
	template<>
	log_string& log_string::operator<< (uint32_t u) {
		ss_ << (unsigned long)static_cast<unsigned long>(u);
		return *this;
	}

	template log_string& log_string::operator<< (std::nullptr_t);
	template log_string& log_string::operator<< (common_error);
	template log_string& log_string::operator<< (std::wstringstream);
	template log_string& log_string::operator<< (IO::PluginClassTypes);
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

	to_log::to_log() : id_(Utils::random_hash(this)) {
		filelog_fun_ = std::bind(static_cast<void(to_log::*)(const std::wstring&)>(&to_log::logtofile_), this, _1);
	}
	to_log::~to_log() {
		closelog_();
	}

	std::wstring to_log::getlogpath_(HINSTANCE h) {
		try {
			if (h) {
				auto t = Utils::app_path(h);
				if (std::get<0>(t)) {
					std::filesystem::path p(std::get<1>(t));
					p.append(std::get<2>(t));
					logname_ = (log_string() << p.wstring().c_str() << L".log").str();
				}
			}
			if (logname_.empty()) {
				std::filesystem::path p(Utils::runing_dir());
				p.append(DEFAULT_LOG_NAME);
				logname_ = p.wstring();
			}
		} catch (...) {}
		return std::wstring(DEFAULT_LOG_NAME);
	}

	to_log& to_log::Get() {
		return std::ref(to_log::tolog__);
	}

	uint32_t to_log::add(callFromlog_t fn) {
		return event_.add(fn);
	}
	void to_log::add(callFromlog_t fn, uint32_t id) {
		event_.add(fn, id);
	}
	void to_log::remove(uint32_t idx) {
		event_.remove(idx);
	}
	void to_log::remove(callFromlog_t fn) {
		event_.remove(fn);
	}
	std::wstring to_log::logname() {
		if (logname_.empty()) getlogpath_();
		return logname_;
	}
	void to_log::end() {
		try {
			closelog_();
			event_.clear();
		} catch (...) {}
	}

	void to_log::flush() {
		try {
			if (filelog_.is_open())
				filelog_.flush();
		} catch (...) {}
	}
	bool to_log::filelog() {
		bool b = Common::common_config::Get().Registry.GetLogWrite();
		if (b) filelog(b, logname_);
		return b;
	}
	bool to_log::filelog(HINSTANCE h) {
		bool b = Common::common_config::Get().Registry.GetLogWrite();
		if (!b) return false;
		getlogpath_(h);
		filelog(b, logname_);
		return b;
	}
	void to_log::filelog(bool b, HINSTANCE h) {
		if (h) getlogpath_(h);
		filelog(b, logname_);
	}
	void to_log::filelog(bool b, const std::wstring ws) {
		try {
			if (b && !filelog_.is_open()) {
				if (logname_.empty()) getlogpath_();

				filelog_ = std::wofstream(ws, std::ios_base::trunc);
				filelog_.imbue(std::locale(""));
				add(filelog_fun_, id_);
				logtofile_((log_string() << VER_GUI_EN << L" | " << VER_COPYRIGHT));
				logtofile_(logname_);

			} else if (!b && filelog_.is_open()) closelog_();
			Common::common_config::Get().Registry.SetLogWrite(b);

		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}

	void to_log::closelog_() {
		try {
			if (id_) remove(id_);
			if (filelog_.is_open()) {
				pushlog_(L"log close.");
				filelog_.flush();
				filelog_.close();
			}
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}
	void to_log::logtofile_(const std::wstring& s) {
		try {
			#if defined(_DEBUG)
			std::wstringstream ws;
			ws << L"* [FILE] FLOG=" << std::boolalpha << filelog_.is_open() << L", MSG = [" << s << L"]\n";
			OutputDebugStringW(ws.str().c_str());
			#endif

			if (filelog_.is_open()) {
				auto dat = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				std::tm buf{};
				::localtime_s(&buf, &dat);

				filelog_ << std::put_time(&buf, L"[%m-%d -> %H:%M:%S] ") << s.c_str() << L"\n";
				filelog_.flush();
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
		w2 << L"* [PUSH] FLOG=" << std::boolalpha << filelog_.is_open() << L", MSG = [" << s << L"]\n";
		OutputDebugStringW(w2.str().c_str());
		#endif
		event_.send(s);
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

	log_auto::log_auto(callFromlog_t fn) : fun_(fn), id_(Utils::random_hash(this)) {
		to_log::Get().add(fun_, id_);
	}
	log_auto::~log_auto() {
		if (id_) to_log::Get().remove(id_);
	}
}