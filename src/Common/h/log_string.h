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

    /*
        DEFINE:               std::function<void(const std::wstring&)>
        USE:         to_log & log = Common::to_log::Get();
        REGISTRED MONITOR:    log.registred(std::function<void(const std::wstring&)>); // [=](const std::wstring&) { ... }
        UNREGISTRED MONITOR:  log.unregistred(std::function<void(const std::wstring&)>);
        // #define TO_f_Fn_log(A) reinterpret_cast<logFnType>(A)
     */

    typedef std::pair<uint32_t, std::function<void(const std::wstring&)>> logBaseType;
       
    FLAG_EXPORT std::wstring operator"" _logw(const char8_t* s, std::size_t);
    FLAG_EXPORT std::string operator"" _logs(const char8_t* s, std::size_t);

    class FLAG_EXPORT log_string {
    private:
        std::wstringstream ss_{};
        std::wstring buffer_{};

        void to_buffer_();
    public:

        void reset();
        void reset_buffer();
        std::wstring get();
        std::wstring str() const;
        const wchar_t* c_str();
        const bool empty();

        operator std::wstring() const;

        template<typename T>
        log_string& to_log_string(const wchar_t* m, const T s) {
            (void) to_log_method(m);
            ss_ << s;
            return *this;
        }

        template<typename... Args>
        log_string& to_log_format(const wchar_t* m, const std::wstring s, Args&&... args) {
            (void) to_log_method(m);
            ss_ << std::vformat(std::wstring_view(s), std::make_wformat_args(args...));
            return *this;
        }

        log_string& to_log_method(const std::wstring);
        log_string& to_log_method(const wchar_t*);
        log_string& to_log_method(const wchar_t*, int);
        log_string& to_log_method(const wchar_t*, const wchar_t*);

        template<typename T1>
        constexpr  log_string& operator<< (T1 arg) {
            ss_ << arg;
            return *this;
        }
        template<> log_string& operator<< (std::nullptr_t);
        template<> log_string& operator<< (common_error);
        template<> log_string& operator<< (std::wstringstream);
        template<> log_string& operator<< (IO::PluginClassTypes);
        template<> log_string& operator<< (MIDI::MidiUnitType);
        template<> log_string& operator<< (MIDI::MidiUnitScene);
        template<> log_string& operator<< (std::exception);
        template<> log_string& operator<< (std::runtime_error);
        template<> log_string& operator<< (std::error_code);
        template<> log_string& operator<< (std::future_error);
        template<> log_string& operator<< (std::filesystem::filesystem_error);
        template<> log_string& operator<< (std::string);
        template<> log_string& operator<< (std::wstring);
        template<> log_string& operator<< (log_string);
        template<> log_string& operator<< (GUID);
        template<> log_string& operator<< (uint8_t);
        template<> log_string& operator<< (uint16_t);
        template<> log_string& operator<< (uint32_t);

        template<typename... Args>
        static inline std::wstring format(const std::wstring s, Args&&... args) {
            return std::vformat(std::wstring_view(s), std::make_wformat_args(args...));
        }

        static bool is_string_empty(const std::wstring&);
        static bool is_string_empty(const std::wstring_view&);
        static bool is_string_empty(const std::wstringstream&);
        static bool is_string_empty(log_string&);
    };

    extern template FLAG_EXPORT log_string& log_string::operator<< (std::nullptr_t);
    extern template FLAG_EXPORT log_string& log_string::operator<< (common_error);
    extern template FLAG_EXPORT log_string& log_string::operator<< (std::wstringstream);
    extern template FLAG_EXPORT log_string& log_string::operator<< (IO::PluginClassTypes);
    extern template FLAG_EXPORT log_string& log_string::operator<< (MIDI::MidiUnitType);
    extern template FLAG_EXPORT log_string& log_string::operator<< (MIDI::MidiUnitScene);

    extern template FLAG_EXPORT log_string& log_string::operator<< (std::exception);
    extern template FLAG_EXPORT log_string& log_string::operator<< (std::runtime_error);
    extern template FLAG_EXPORT log_string& log_string::operator<< (std::error_code);
    extern template FLAG_EXPORT log_string& log_string::operator<< (std::future_error);
    extern template FLAG_EXPORT log_string& log_string::operator<< (std::filesystem::filesystem_error);

    extern template FLAG_EXPORT log_string& log_string::operator<< (std::string);
    extern template FLAG_EXPORT log_string& log_string::operator<< (std::wstring);

    extern template FLAG_EXPORT log_string& log_string::operator<< (GUID);
    extern template FLAG_EXPORT log_string& log_string::operator<< (uint8_t);
    extern template FLAG_EXPORT log_string& log_string::operator<< (uint16_t);
    extern template FLAG_EXPORT log_string& log_string::operator<< (uint32_t);

    class to_log {
    private:
        common_event<callFromlog_t> event_{};
        std::wofstream filelog_{};
        callFromlog_t filelog_fun_{};
        std::wstring logname_{};
        uint32_t id_{ 0U };
        static to_log tolog__;

        std::wstring getlogpath_(HINSTANCE h = 0);
        FLAG_EXPORT void pushlog_(const std::wstring);
        void logtofile_(const std::wstring&);
        void closelog_();

    public:

        to_log();
        FLAG_EXPORT ~to_log();

        FLAG_EXPORT uint32_t add(callFromlog_t);
        FLAG_EXPORT void add(callFromlog_t, uint32_t);
        FLAG_EXPORT void remove(callFromlog_t);
        FLAG_EXPORT void remove(uint32_t idx);

        FLAG_EXPORT void end();
        FLAG_EXPORT void flush();
        FLAG_EXPORT bool filelog();
        FLAG_EXPORT bool filelog(HINSTANCE);
        FLAG_EXPORT void filelog(bool b, HINSTANCE = 0);
        FLAG_EXPORT void filelog(bool b, const std::wstring ws);

        FLAG_EXPORT static to_log & Get();

        FLAG_EXPORT to_log& operator<< (std::wstringstream&);
        FLAG_EXPORT to_log& operator<< (LPWSTR);
        FLAG_EXPORT to_log& operator<< (const wchar_t[]);
        FLAG_EXPORT to_log& operator<< (bool&);

        FLAG_EXPORT std::wstring logname();

        template<typename T1>
        constexpr to_log& operator<< (const T1& arg) noexcept {
            pushlog_(arg);
            return *this;
        }
        template<> FLAG_EXPORT to_log& operator<< (const log_string&);
        template<> FLAG_EXPORT to_log& operator<< (const common_error&);
        template<> FLAG_EXPORT to_log& operator<< (const std::exception&);
        template<> FLAG_EXPORT to_log& operator<< (const std::runtime_error&);
        template<> FLAG_EXPORT to_log& operator<< (const std::future_error&);
        template<> FLAG_EXPORT to_log& operator<< (const std::error_code&);
        template<> FLAG_EXPORT to_log& operator<< (const std::filesystem::filesystem_error&);
        template<> FLAG_EXPORT to_log& operator<< (const std::wstring_view&);
        template<> FLAG_EXPORT to_log& operator<< (const std::string&);
        template<> FLAG_EXPORT to_log& operator<< (const std::wstring&);
        template<> FLAG_EXPORT to_log& operator<< (const GUID&);

        template<typename T1>
        constexpr void tolog(const T1& arg) noexcept {
            pushlog_(Utils::to_string(arg));
        }

        template<typename... Args>
        constexpr void operator() (Args&&... args) noexcept {
            pushlog_(log_string() << args...);
        }

        template<typename T1>
        to_log& operator+=(T1& arg) {
            pushlog_(Utils::to_string(arg));
            return *this;
        }
    };
    extern template FLAG_EXPORT to_log& to_log::operator<< (const std::exception&);
    extern template FLAG_EXPORT to_log& to_log::operator<< (const std::runtime_error&);
    extern template FLAG_EXPORT to_log& to_log::operator<< (const std::future_error&);
    extern template FLAG_EXPORT to_log& to_log::operator<< (const std::error_code&);
    extern template FLAG_EXPORT to_log& to_log::operator<< (const std::filesystem::filesystem_error&);
    extern template FLAG_EXPORT to_log& to_log::operator<< (const std::string&);
    extern template FLAG_EXPORT to_log& to_log::operator<< (const std::wstring&);
    extern template FLAG_EXPORT to_log& to_log::operator<< (const std::wstring_view&);
    extern template FLAG_EXPORT to_log& to_log::operator<< (const GUID&);

    class log_auto {
    private:
        callFromlog_t fun_ = [](const std::wstring&) {};
        uint32_t id_{ 0U };
    public:
        log_auto(callFromlog_t);
        ~log_auto();

        template<typename T1>
        log_auto& operator<< (T1 arg) {
            to_log::Get() << arg;
        }
    };
}