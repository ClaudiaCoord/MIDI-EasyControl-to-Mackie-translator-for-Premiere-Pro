/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include <mmeapi.h>
#include "h\winreg.h"

namespace Common {

    using namespace std::string_view_literals;

    class UtilsLocal {
    public:
        /* CODE_PAGE = CP_ACP ? CP_UTF8 */
        static constexpr auto CONV_FLAGS = 0; // MB_ERR_INVALID_CHARS;
        
        static constexpr std::wstring_view default_seh_error = L"SEH runtime error: "sv;
        static constexpr std::wstring_view default_found_seh_error = L"SEH exception execution detected"sv;
        static constexpr std::wstring_view default_runtime_error = L"unknown runtime error"sv;
        static constexpr std::wstring_view default_code_error = L"error code: "sv;
        static constexpr std::wstring_view default_no = L"no"sv;
        static constexpr std::wstring_view default_yes = L"yes"sv;
    };

    const std::wstring_view Utils::DefaulRuntimeError() {
        return UtilsLocal::default_runtime_error;
    }
    const std::wstring_view Utils::DefaulSehErrorFound() {
        return UtilsLocal::default_found_seh_error;
    }

    uint32_t operator"" _hash(const wchar_t* s, std::size_t n) {
        if (!s || !n) return 0U;
        std::wstring w(s, n);
        return Utils::to_hash(w);
    }

    template<typename T1, UINT CODE_PAGE = CP_ACP>
    static std::wstring string_convert_(const T1 s) {
        try {
            do {
                size_t sz{ 0U }, rsz{ 0U };
                const char* c;
                if constexpr (std::is_same_v<const std::string, T1> || std::is_same_v<const std::string_view, T1>) {
                    if (s.empty()) break;
                    c = s.data();
                    sz = s.size();
                }
                else if constexpr (std::is_same_v<const char*, T1>) {
                    if (!s) break;
                    c = s;
                    sz = ::strlen(c);
                }
                else if constexpr (true) break;

                int size = (sz ? (int)sz : -1);
                rsz = ::MultiByteToWideChar(CODE_PAGE, UtilsLocal::CONV_FLAGS, c, size, 0, 0);
                if (!rsz) break;

                std::vector<wchar_t> v(sz + 1);
                ::MultiByteToWideChar(CODE_PAGE, UtilsLocal::CONV_FLAGS, c, size, &v[0], (int)rsz);
                size_t off = (v[rsz] == L'\0') ? 1 : 0;
                return std::wstring(v.begin(), v.end() - off);
            } while (0);
        } catch (...) {}
        return std::wstring();
    }

    template<typename T1, UINT CODE_PAGE = CP_ACP>
    static std::string wstring_convert_(const T1 s) {
        try {
            do {
                size_t sz{ 0U }, rsz{ 0U };
                const wchar_t* c;
                if constexpr (std::is_same_v<const std::wstring, T1> || std::is_same_v<const std::wstring_view, T1>) {
                    if (s.empty()) break;
                    c = s.data();
                    sz = s.size();
                }
                else if constexpr (std::is_same_v<const wchar_t*, T1>) {
                    if (!s) break;
                    c = s;
                    sz = ::wcslen(c);
                }
                else if constexpr (true) break;

                int size = (sz ? (int)sz : -1);
                rsz = ::WideCharToMultiByte(CODE_PAGE, UtilsLocal::CONV_FLAGS, c, size, 0, 0, 0, 0);
                if (!rsz) break;

                std::vector<char> v(sz + 1);
                ::WideCharToMultiByte(CODE_PAGE, UtilsLocal::CONV_FLAGS, c, size, &v[0], (int)rsz, 0, 0);
                size_t off = (v[rsz] == L'\0') ? 1 : 0;
                return std::string(v.begin(), v.end() - off);
            } while (0);
        } catch (...) {}
        return std::string();
    }

    static void  exception_(std::exception_ptr ptr, std::wstring f) {
        if (!ptr) return;

        std::wstring we{};

        try {
            std::rethrow_exception(ptr);
        }
        catch (const seh_exception& e) {
            log_string ls;
            ls << UtilsLocal::default_seh_error << std::to_wstring(e.error());
            we = ls.str();
        }
        catch (const common_error& e) {
            if ((e.code() == 0) && (e.message().empty())) return;
            to_log::Get() << e.what();
            return;
        }
        catch (const winreg::RegException& e) {
            log_string ls;
            std::error_code c = e.code();
            ls << string_convert_<const char*>(e.what()) << L", " << UtilsLocal::default_code_error << c.value();
            we = ls.str();
        } 
        catch (const std::future_error& e) {
            we = Utils::to_string(e);
        }
        catch (const std::filesystem::filesystem_error& e) {
            we = Utils::to_string(e);
        }
        catch (const std::runtime_error& e) {
            we = Utils::to_string(e);
        }
        catch (const std::bad_exception& e) {
            we = Utils::to_string(e);
        }
        catch (const std::exception& e) {
            we = Utils::to_string(e);
        }
        catch (...) {
            we = Utils::DefaulRuntimeError();
        }
        if (we.empty()) return;
        try {
            log_string ls;
            ls.to_log_method(f.c_str());
            ls << we;
            to_log::Get() << ls.str();
        } catch (...) {
            log_string ls;
            ls.to_log_method(__FUNCTIONW__, f.c_str());
            ls << UtilsLocal::default_runtime_error;
            to_log::Get() << ls.str();
        }
    }
    static CLSID build_GUID_(const char* s) {
        CLSID clsid{};
        if (::CLSIDFromString(ATL::CComBSTR(s), &clsid) == NOERROR) return clsid;
        return GUID_NULL;
    }
    static void  build_GUID_(const GUID& g, wchar_t wc[], const DWORD sz) {
        __try {
            (void) ::StringFromGUID2(g, wc, sz);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    static bool  build_PID_(const DWORD& d, wchar_t wc[], const DWORD sz) {
        bool b = false;
        __try {
            HANDLE h = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, d);
            if (h) {
                DWORD psz = sz;
                if (::QueryFullProcessImageNameW(h, 0, wc, &psz) && (psz > 0U)) b = true;
                ::CloseHandle(h);
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
        return b;
    }
    static bool  build_PIDRUN_(const DWORD& d) {
        DWORD x = 0U;
        __try {
            HANDLE h = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, d);
            if (h) {
                DWORD x = 0U;
                (void) ::GetExitCodeProcess(h, &x);
                ::CloseHandle(h);
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
        return x == STILL_ACTIVE;
    }
    static void  build_MMRESULT_(const MMRESULT& r, wchar_t wc[], const DWORD sz) {
        __try {
            (void) ::midiOutGetErrorTextW(r, wc, sz);
        } __except (EXCEPTION_EXECUTE_HANDLER) {}
    }

    std::string  Utils::trim_(std::string_view& sv) {
        std::string s(sv);
        static const char* t = " \t\n\r\f\v";
        s.erase(0, s.find_first_not_of(t));
        s.erase(s.find_last_not_of(t) + 1U);
        return s;
    }
    std::wstring Utils::trim_(std::wstring_view& sv) {
        std::wstring s(sv);
        static const wchar_t* t = L" \t\n\r\f\v";
        s.erase(0, s.find_first_not_of(t));
        s.erase(s.find_last_not_of(t) + 1U);
        return s;
    }

    std::wstring Utils::runing_dir() {
        return std::filesystem::current_path().wstring();
    }
    std::wstring Utils::runing_dir(const std::wstring_view& fname) {
        auto p = std::wstring(fname);
        return runing_dir(p);
    }
    std::wstring Utils::runing_dir(const std::wstring& fname) {
        auto p = std::filesystem::current_path();
        p.append(fname);
        return p.wstring();
    }
    
    std::wstring Utils::app_dir(HINSTANCE h) {
        try {
            wchar_t cpath[MAX_PATH + 1]{};
            if (::GetModuleFileNameW(h, cpath, MAX_PATH) != 0)
                return std::filesystem::path(cpath).parent_path().wstring();
        } catch (...) {
            Utils::get_exception(std::current_exception(), __FUNCTIONW__);
        }
        return std::wstring();
    }
    std::wstring Utils::app_dir(const std::wstring_view& fname, HINSTANCE h) {
        return app_dir(std::wstring(fname), h);
    }
    std::wstring Utils::app_dir(const std::wstring& fname, HINSTANCE h) {
        std::filesystem::path p(app_dir(h));
        if (!p.empty()) {
            p.append(fname);
            return p.wstring();
        }
        return std::wstring();
    }

    std::wstring Utils::app_name(HINSTANCE h) {
        std::filesystem::path p(app_dir(h));
        if (!p.empty())            
            return p.stem().wstring();
        return std::wstring();
    }
    std::tuple<bool, std::wstring, std::wstring> Utils::app_path(HINSTANCE h) {
        try {
            wchar_t cpath[MAX_PATH + 1]{};
            if (::GetModuleFileNameW(h, cpath, MAX_PATH) != 0) {
                std::filesystem::path p(cpath);
                return std::make_tuple(true, p.parent_path().wstring(), p.stem().wstring());
            }
        } catch (...) {
            Utils::get_exception(std::current_exception(), __FUNCTIONW__);
        }
        return std::make_tuple(false, std::wstring(), std::wstring());
    }

    std::wstring Utils::device_out_name(const std::wstring& ss, const std::wstring& se) {
        std::filesystem::path p = std::filesystem::path(ss);
        std::wstring ws = p.stem().wstring();
        ws.append(se);
        return ws;
    }
    std::wstring Utils::cofig_name(const std::wstring& ss) {
        std::filesystem::path p = std::filesystem::path(ss);
        return p.stem().wstring();
    }

    const bool Utils::random_isvalid(uint32_t id) {
        return (id != (UINT_MAX - 1)) && (id != 0U);
    }
    uint32_t   Utils::random_hash(void* v) {
        try {
            #pragma warning( push )
            #pragma warning( disable : 4302 4311 )
            if (v != nullptr)
                return reinterpret_cast<uint32_t>(v);
            #pragma warning( pop )
        } catch (...) {
            Utils::get_exception(std::current_exception(), __FUNCTIONW__);
        }
        return (UINT_MAX - 1);
    }
    uint32_t   Utils::to_hash(const wchar_t* s) {
        if (!s) return 0U;
        std::wstring w(s);
        return to_hash(w);
    }
    uint32_t   Utils::to_hash(const std::wstring& s) {
        try {
            return static_cast<uint32_t>(std::hash<std::wstring>{}(s));
        } catch (...) {
            Utils::get_exception(std::current_exception(), __FUNCTIONW__);
        }
        return 0U;
    }

    std::tuple<bool, std::wstring, std::wstring> Utils::pid_to_string(const DWORD& d) {
        std::tuple<bool, std::wstring, std::wstring> t{};
        wchar_t wc[1024]{};
        
        if (build_PID_(d, wc, (DWORD)std::size(wc)) && (CHECK_LPWSTRING(wc))) {
            std::wstring ws = std::wstring(wc);
            t = std::make_tuple(true, std::wstring(std::filesystem::path(ws).stem()), std::move(ws));
        }
        return t;
    }
    bool Utils::is_pid_running(const uint32_t& d) {
        return Utils::is_pid_running(static_cast<const DWORD>(d));
    }
    bool Utils::is_pid_running(const DWORD d) {
        return build_PIDRUN_(d);
    }
    bool Utils::is_guid_equals(const GUID& g1, const GUID& g2) {
        return ::IsEqualGUID(g1, g2);
    }

    std::wstring Utils::MMRESULT_to_string(const MMRESULT& r) {
        std::wstring ws{};
        wchar_t wc[1024]{};
        build_MMRESULT_(r, wc, (DWORD)std::size(wc));
        return Utils::to_string(wc);
    }
    std::wstring Utils::NANOSECONDS_to_string(const uint64_t& nanos) {
        ULARGE_INTEGER ui{ .QuadPart = nanos };
        return to_string(std::ref(ui));
    }
    std::wstring Utils::MILLISECONDS_to_string(const DWORD& millis) {
        return MILLISECONDS_to_string(static_cast<uint64_t>(millis));
    }
    std::wstring Utils::MILLISECONDS_to_string(const uint64_t& millis) {
        return std::format(L"{:%T}", std::chrono::floor<std::chrono::milliseconds>(std::chrono::milliseconds(millis)));
    }
    std::wstring Utils::MILLISECONDS_to_string(const std::chrono::steady_clock::time_point& millis) {
        auto t = std::chrono::high_resolution_clock::now();
        auto p = std::chrono::duration_cast<std::chrono::milliseconds>(t - millis);
        return std::format(L"{:%T}", std::chrono::floor<std::chrono::milliseconds>(p));
    }
    std::wstring Utils::BOOL_to_string(const bool b) {
        return b ? UtilsLocal::default_yes.data() : UtilsLocal::default_no.data();
    }
    std::wstring Utils::to_string(const std::wstring_view v) {
        return std::wstring(v);
    }
    std::wstring Utils::to_string(const std::string_view v) {
        const std::string s(v);
        return Utils::to_string(s);
    }
    std::wstring Utils::to_string(const std::string s) {
        return string_convert_<const std::string>(s);
    }
    std::wstring Utils::to_string(LPWSTR s) {
        if (CHECK_LPWSTRING(s)) return std::wstring(s);
        return L"";
    }
    std::wstring Utils::to_string(const GUID& g) {
        std::wstring ws{};
        wchar_t wc[64]{};
        build_GUID_(g, wc, (DWORD)std::size(wc));
        return Utils::to_string(wc);
    }
    std::wstring Utils::to_string(const std::runtime_error& err) {
        return string_convert_<const char*>(err.what());
    }
    std::wstring Utils::to_string(const std::exception& err) {
        return string_convert_<const char*>(err.what());
    }
    std::wstring Utils::to_string(const std::error_code& err) {
        std::wstringstream ws{};
        ws << UtilsLocal::default_code_error << err.value();
        if (!err.message().empty()) ws << L", " << Utils::to_string(err.message());
        return ws.str();
    }
    std::wstring Utils::to_string(const std::future_error& err) {
        return string_convert_<const char*>(err.what());
    }
    std::wstring Utils::to_string(const std::filesystem::filesystem_error& err) {
        return string_convert_<const char*>(err.what());
    }
    std::wstring Utils::to_string(char8_t const* s, std::size_t z) {
        return std::wstring(s, s + z);
    }
    std::wstring Utils::to_string(std::nullptr_t) {
        return L"(null)";
    }
    std::wstring Utils::to_string(const char s[]) {
        return string_convert_<const char*>(s);
    }
    std::wstring Utils::to_string(unsigned int& u) {
        return std::to_wstring(static_cast<unsigned long>(u));
    }
    std::wstring Utils::to_string(const IO::PluginClassTypes& t) {
        return Utils::to_string(IO::PluginHelper::GetClassTypes(t));
    }
    std::wstring Utils::to_string(const MIDI::MidiUnitType& t) {
        return Utils::to_string(Common::MIDI::MidiHelper::GetType(t));
    }
    std::wstring Utils::to_string(const MIDI::MidiUnitScene& t) {
        return Utils::to_string(Common::MIDI::MidiHelper::GetScene(t));
    }
    std::wstring Utils::to_string(const ULARGE_INTEGER& t) {
        FILETIME ft{ .dwLowDateTime{ t.LowPart }, .dwHighDateTime{ t.HighPart } };
        return to_string(std::ref(ft));
    }
    std::wstring Utils::to_string(const FILETIME& t) {
        SYSTEMTIME st{};
        ::FileTimeToSystemTime(&t, &st);
        return to_string(std::ref(st));
    }
    std::wstring Utils::to_string(const SYSTEMTIME& t) {
        return (log_string()
            << std::setw(2) << std::setfill(L'0') << t.wDay << L'-'
            << std::setw(2) << std::setfill(L'0') << t.wMonth << L'-'
            << std::setw(2) << std::setfill(L'0') << t.wYear << L' '
            << std::setw(2) << std::setfill(L'0') << t.wHour << L':'
            << std::setw(2) << std::setfill(L'0') << t.wMinute << L':'
            << std::setw(2) << std::setfill(L'0') << t.wSecond
            );
    }

    std::string Utils::from_string(const std::wstring& s) {
        return wstring_convert_<const std::wstring>(s);
    }
    std::string Utils::from_string(const wchar_t* s) {
        return wstring_convert_<const wchar_t*>(s);
    }

    CLSID       Utils::guid_from_string(const char* s) {
        return build_GUID_(s);
    }
    CLSID       Utils::guid_from_string(const wchar_t* s) {
        return build_GUID_(wstring_convert_<const wchar_t*>(s).c_str());
    }

    void Utils::get_exception(std::exception_ptr ptr, const char* f) {
        if (!ptr) return;
        get_exception(std::move(ptr), Utils::to_string(f));
    }
    void Utils::get_exception(std::exception_ptr ptr, const wchar_t f[]) {
        if (!ptr) return;
        get_exception(std::move(ptr), std::wstring(f));
    }
    void Utils::get_exception(std::exception_ptr ptr, std::wstring f) {
        try {
            if (!ptr) return;
            exception_(std::move(ptr), f);
        }
        catch (const std::bad_exception& e) {
            log_string ls;
            ls.to_log_method(__FUNCTIONW__, f.c_str());
            ls << Utils::to_string(e.what());
            to_log::Get() << ls.str();
        }
        catch (...) {
            log_string ls;
            ls.to_log_method(__FUNCTIONW__, f.c_str());
            ls << Utils::DefaulRuntimeError();
            to_log::Get() << ls.str();
        }
    }
}
