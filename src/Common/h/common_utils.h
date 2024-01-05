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

    FLAG_EXPORT uint32_t operator"" _hash(const wchar_t*, std::size_t);

    class FLAG_EXPORT Utils {

    public:
        static const std::wstring_view DefaulRuntimeError();
        static const std::wstring_view DefaulSehErrorFound();

        static const bool random_isvalid(uint32_t);
        static uint32_t   random_hash(void*);
        static uint32_t   to_hash(const wchar_t*);
        static uint32_t   to_hash(const std::wstring&);

        static std::wstring runing_dir();
        static std::wstring runing_dir(const std::wstring&);
        static std::wstring runing_dir(const std::wstring_view&);

        static std::wstring app_name(HINSTANCE);
        static std::tuple<bool, std::wstring, std::wstring> app_path(HINSTANCE);
        static std::wstring app_dir(HINSTANCE = nullptr);
        static std::wstring app_dir(const std::wstring&, HINSTANCE = nullptr);
        static std::wstring app_dir(const std::wstring_view&, HINSTANCE = nullptr);

        static std::string trim_(std::string_view&);
        static std::wstring trim_(std::wstring_view&);

        static std::wstring device_out_name(const std::wstring & ss, const std::wstring & se);
        static std::wstring cofig_name(const std::wstring & ss);
        static std::tuple<bool, std::wstring, std::wstring> pid_to_string(const DWORD& d);
		static bool is_pid_running(const uint32_t& d);
        static bool is_pid_running(const DWORD d);
        static bool is_guid_equals(const GUID&, const GUID&);
        static std::wstring MILLISECONDS_to_string(const DWORD&);
        static std::wstring MILLISECONDS_to_string(const std::chrono::steady_clock::time_point&);
        static std::wstring MMRESULT_to_string(const MMRESULT&);
        static std::wstring BOOL_to_string(const bool);
        static std::wstring CHARS_to_string(const char*);
        static std::wstring to_string(const std::wstring_view);
        static std::wstring to_string(const std::string_view);
        static std::wstring to_string(const std::string);
        static std::wstring to_string(const LPWSTR);
        static std::wstring to_string(const char[]);
        static std::wstring to_string(const GUID&);
        static std::wstring to_string(const std::runtime_error&);
        static std::wstring to_string(const std::exception&);
        static std::wstring to_string(const std::error_code&);
        static std::wstring to_string(const std::future_error&);
        static std::wstring to_string(const std::filesystem::filesystem_error&);
        static std::wstring to_string(const char8_t*, std::size_t);
        static std::wstring to_string(std::nullptr_t);
        static std::wstring to_string(unsigned int&);
        static std::wstring to_string(IO::PluginClassTypes);
        static std::wstring to_string(MIDI::MidiUnitType);
        static std::wstring to_string(MIDI::MidiUnitScene);

        static std::string from_string(const std::wstring&);
        static CLSID       guid_from_string(const char*);
        static CLSID       guid_from_string(const wchar_t*);

        static void get_exception(std::exception_ptr, const char*);
        static void get_exception(std::exception_ptr, const wchar_t[]);
        static void get_exception(std::exception_ptr, std::wstring);

        template<typename T1>
        static T1 trim(T1 v) {
            return trimRef<T1>(std::ref(v));
        }
        template<typename T1>
        static T1 trimRef(T1& v) {
            if constexpr (std::is_same_v<std::string, T1>) {
                std::string_view sv(v);
                return Utils::trim_(std::ref(sv));
            }
            else if constexpr (std::is_same_v<std::wstring, T1>) {
                std::wstring_view sv(v);
                return Utils::trim_(std::ref(sv));
            }
            else if constexpr (std::is_same_v<std::wstring_view, T1> || std::is_same_v<std::string_view, T1>)
                return Utils::trim_(v);
            else throw std::logic_error("trim Ref input wrong");
        }
    };
}

