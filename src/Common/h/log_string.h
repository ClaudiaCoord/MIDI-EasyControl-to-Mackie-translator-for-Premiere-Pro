/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

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
       
    class FLAG_EXPORT log_string
    {
    private:
        std::wstringstream ss__{};
        std::wstring buffer__;
    public:

        void reset();
        void reset_buffer();
        std::wstring str();
        const wchar_t* c_str();

        operator std::wstring() const;

        template<typename T1>
        constexpr  log_string& operator<< (T1 arg) noexcept {
            ss__ << arg;
            return *this;
        }
        template<> log_string& operator<< (std::nullptr_t);
        template<> log_string& operator<< (runtime_werror);
        template<> log_string& operator<< (std::wstringstream);
        template<> log_string& operator<< (Common::MIDI::ClassTypes);
        template<> log_string& operator<< (Common::MIDI::MidiUnitType);
        template<> log_string& operator<< (Common::MIDI::MidiUnitScene);
        template<> log_string& operator<< (std::exception);
        template<> log_string& operator<< (std::runtime_error);
        template<> log_string& operator<< (std::error_code);
        template<> log_string& operator<< (std::future_error);
        template<> log_string& operator<< (std::filesystem::filesystem_error);
        template<> log_string& operator<< (std::string);
        template<> log_string& operator<< (std::wstring);
        template<> log_string& operator<< (GUID);
        template<> log_string& operator<< (uint8_t);
        template<> log_string& operator<< (uint16_t);
        template<> log_string& operator<< (uint32_t);
    };

    class to_log
    {
    private:
        std::atomic<bool> isdispose = false;
        common_event<logFnType> event__;
        std::wofstream filelog__;
        logFnType filelog_fun__;
        static to_log tolog__;

        FLAG_EXPORT void pushlog_(const std::wstring);
        void logtofile(const std::wstring&);
        void closelog();

    public:

        to_log();
        FLAG_EXPORT ~to_log();

        FLAG_EXPORT uint32_t registred(logFnType);
        FLAG_EXPORT void unregistred(logFnType);
        FLAG_EXPORT void unregistred(uint32_t idx);

        FLAG_EXPORT void end();
        FLAG_EXPORT void flush();
        FLAG_EXPORT bool filelog();
        FLAG_EXPORT void filelog(bool b);
        FLAG_EXPORT void filelog(bool b, const std::wstring ws);

        FLAG_EXPORT static to_log & Get();

        FLAG_EXPORT to_log& operator<< (runtime_werror&);
        FLAG_EXPORT to_log& operator<< (std::wstringstream&);
        FLAG_EXPORT to_log& operator<< (LPWSTR);
        FLAG_EXPORT to_log& operator<< (const wchar_t[]);
        FLAG_EXPORT to_log& operator<< (bool&);

        template<typename T1>
        constexpr to_log& operator<< (const T1& arg) noexcept {
            pushlog_(arg);
            return *this;
        }
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

    class log_auto {
    private:
        logFnType fun__ = [](const std::wstring&) {};
    public:
        log_auto(logFnType);
        ~log_auto();

        template<typename T1>
        log_auto& operator<< (T1 arg) {
            to_log::Get() << arg;
        }
    };
}