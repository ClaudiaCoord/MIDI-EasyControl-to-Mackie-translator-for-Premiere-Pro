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

    class FLAG_EXPORT Utils {

    public:
        static const std::wstring_view DefaulPrefixError();
        static const std::wstring_view DefaulRuntimeError();
        static const std::wstring_view DefaulSehErrorFound();

        static const bool random_isvalid(uint32_t);
        static uint32_t random_hash(void*);
        static std::wstring runing_dir();
        static std::wstring runing_dir(const std::wstring& fname);
        static std::wstring runing_dir(const std::wstring_view& fname);
        static std::wstring device_out_name(const std::wstring & ss, const std::wstring & se);
        static std::wstring cofig_name(const std::wstring & ss);
        static std::wstring trim_(std::wstring_view&);
        static std::tuple<bool, std::wstring, std::wstring> pid_to_string(const DWORD& d);
		static bool is_pid_running(const uint32_t& d);
        static bool is_pid_running(const DWORD d);
        static std::wstring MILLISECONDS_to_string(const DWORD&);
        static std::wstring MMRESULT_to_string(const MMRESULT&);
        static std::wstring BOOL_to_string(const bool);
        static std::wstring CHARS_to_string(const char*);
        static std::wstring to_string(const std::wstring_view);
        static std::wstring to_string(const std::string);
        static std::wstring to_string(const LPWSTR);
        static std::wstring to_string(const char[]);
        static std::wstring to_string(const GUID&);
        static std::wstring to_string(const std::runtime_error&);
        static std::wstring to_string(const std::exception&);
        static std::wstring to_string(const std::error_code&);
        static std::wstring to_string(const std::future_error&);
        static std::wstring to_string(const std::filesystem::filesystem_error&);
        static std::wstring to_string(std::nullptr_t);
        static std::wstring to_string(unsigned int&);
        static std::wstring to_string(Common::MIDI::ClassTypes);
        static std::wstring to_string(Common::MIDI::MidiUnitType);
        static std::wstring to_string(Common::MIDI::MidiUnitScene);

        static std::string from_string(const std::wstring&);

        static void get_exception(std::exception_ptr, const char*);
        static void get_exception(std::exception_ptr, const wchar_t[]);
        static void get_exception(std::exception_ptr, std::wstring);

        static bool EndsWith(std::wstring_view, std::wstring_view);
        static bool StartsWith(std::wstring_view, std::wstring_view);

        template<typename T1>
        static std::wstring trim(T1& v) {
            std::wstring_view sv(v);
            return Utils::trim_(std::ref(sv));
        }
    };
}

