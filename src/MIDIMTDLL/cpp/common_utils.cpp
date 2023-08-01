/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include <mmeapi.h>
#include "h\winreg.h"

namespace Common {

    constexpr UINT CODE_PAGE_ = CP_ACP; // CP_UTF8;
    constexpr DWORD CONV_FLAGS_ = 0; // MB_ERR_INVALID_CHARS;

    using namespace std::string_view_literals;
    constexpr std::wstring_view default_seh_error__ = L"SEH runtime error: "sv;
    constexpr std::wstring_view default_found_seh_error__ = L"SEH exception execution detected";
    constexpr std::wstring_view default_runtime_error__ = L"unknown runtime error"sv;
    constexpr std::wstring_view default_code_error__ = L"error code: "sv;
    constexpr wchar_t default_no__[] = L"no";
    constexpr wchar_t default_yes__[] = L"yes";

    const std::wstring_view Utils::DefaulRuntimeError() {
        return default_runtime_error__;
    }
    const std::wstring_view Utils::DefaulSehErrorFound() {
        return default_found_seh_error__;
    }

    static void exception_(std::exception_ptr ptr, std::wstring f) {
        if (!ptr) return;

        std::wstring we{};

        try {
            std::rethrow_exception(ptr);
        }
        catch (const seh_exception& e) {
            log_string ls;
            ls << default_seh_error__ << std::to_wstring(e.error());
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
            ls << Utils::CHARS_to_string(e.what()) << L", " << default_code_error__ << c.value();
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
            ls << default_runtime_error__;
            to_log::Get() << ls.str();
        }
    }
    static void build_MMRESULT_(const MMRESULT& r, wchar_t wc[], const DWORD sz) {
        __try {
            (void) ::midiOutGetErrorTextW(r, wc, sz);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    static void build_GUID_(const GUID& g, wchar_t wc[], const DWORD sz) {
        __try {
            (void) ::StringFromGUID2(g, wc, sz);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
    static bool build_PID_(const DWORD& d, wchar_t wc[], const DWORD sz) {
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
    static bool build_PIDRUN_(const DWORD& d) {
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
    static std::wstring build_Chars_(const char* c) {
        try {
            do {
                if (c == nullptr) break;
                size_t sz = ::strlen(c),
                       wsz = ::MultiByteToWideChar(CODE_PAGE_, CONV_FLAGS_, c, (sz ? (int)sz : -1), 0, 0);
                if (!wsz) break;

                std::vector<wchar_t> v(wsz + 1);
                ::MultiByteToWideChar(CODE_PAGE_, CONV_FLAGS_, c, -1, &v[0], (int)wsz);
                return std::wstring(v.begin(), (v.begin() + wsz));

            } while (0);
        } catch (...) {}
        return L"";
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
    uint32_t Utils::random_hash(void* v) {
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

    std::wstring Utils::trim_(std::wstring_view& sv) {
        sv.remove_prefix(sv.find_first_not_of(L" \t\r\v\n"));
        sv.remove_suffix(sv.size() - sv.find_last_not_of(L" \t\r\v\n") - 1);
        return std::wstring(sv);
    }
    bool Utils::EndsWith(std::wstring_view str, std::wstring_view suffix) {
        return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
    }
    bool Utils::StartsWith(std::wstring_view str, std::wstring_view prefix) {
        return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
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

    std::wstring Utils::MMRESULT_to_string(const MMRESULT& r) {
        std::wstring ws{};
        wchar_t wc[1024]{};
        build_MMRESULT_(r, wc, (DWORD)std::size(wc));
        return Utils::to_string(wc);
    }
    std::wstring Utils::MILLISECONDS_to_string(const DWORD& millis) {
        uint32_t ms = millis;
        uint16_t sec = ms / 1000; ms %= 1000;
        uint16_t min = sec / 60; sec %= 60;
        uint16_t hour = min / 60; min %= 60;
        std::wstringstream wss;
        wss << hour << L":" << min << L":" << sec << L"." << ms;
        return wss.str();
    }
    std::wstring Utils::BOOL_to_string(const bool b) {
        return b ? default_yes__ : default_no__;
    }
    std::wstring Utils::CHARS_to_string(const char* s) {
        return build_Chars_(s);
    }
    std::wstring Utils::to_string(const std::wstring_view v) {
        return std::wstring(v);
    }
    std::wstring Utils::to_string(const std::string s) {
        try {
            do {
                if (s.empty()) break;
                size_t sz = ::MultiByteToWideChar(CODE_PAGE_, CONV_FLAGS_, s.data(), (int)s.size(), 0, 0);
                if (!sz) break;

                std::vector<wchar_t> v(sz + 1);
                ::MultiByteToWideChar(CODE_PAGE_, CONV_FLAGS_, s.data(), (int)s.size(), &v[0], (int)sz);
                return std::wstring(v.begin(), v.end());
            } while (0);
        } catch (...) {}
        return L"";
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
        return build_Chars_(err.what());
    }
    std::wstring Utils::to_string(const std::exception& err) {
        return build_Chars_(err.what());
    }
    std::wstring Utils::to_string(const std::error_code& err) {
        std::wstringstream ws{};
        ws << default_code_error__ << err.value();
        if (!err.message().empty()) ws << L", " << Utils::to_string(err.message());
        return ws.str();
    }
    std::wstring Utils::to_string(const std::future_error& err) {
        return build_Chars_(err.what());
    }
    std::wstring Utils::to_string(const std::filesystem::filesystem_error& err) {
        return build_Chars_(err.what());
    }
    std::wstring Utils::to_string(char8_t const* s, std::size_t z) {
        return std::wstring(s, s + z);
    }
    std::wstring Utils::to_string(std::nullptr_t) {
        return L"(null)";
    }
    std::wstring Utils::to_string(const char s[]) {
        return build_Chars_(s);
    }
    std::wstring Utils::to_string(unsigned int& u) {
        return std::to_wstring(static_cast<unsigned long>(u));
    }
    std::wstring Utils::to_string(Common::MIDI::ClassTypes t) {
        return Utils::to_string(Common::MIDI::MidiHelper::GetClassTypes(t));
    }
    std::wstring Utils::to_string(Common::MIDI::MidiUnitType t) {
        return Utils::to_string(Common::MIDI::MidiHelper::GetType(t));
    }
    std::wstring Utils::to_string(Common::MIDI::MidiUnitScene t) {
        return Utils::to_string(Common::MIDI::MidiHelper::GetScene(t));
    }

    std::string Utils::from_string(const std::wstring& s) {
        try {
            do {
                if (s.empty()) break;
                size_t sz = ::WideCharToMultiByte(CODE_PAGE_, CONV_FLAGS_, s.data(), (int)s.size(), 0, 0, 0, 0);
                if (!sz) break;

                std::vector<char> v(sz + 1);
                ::WideCharToMultiByte(CODE_PAGE_, CONV_FLAGS_, s.data(), (int)s.size(), &v[0], (int)sz, 0, 0);
                return std::string(v.begin(), v.end());
            } while (0);
        } catch (...) {}
        return "";
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
