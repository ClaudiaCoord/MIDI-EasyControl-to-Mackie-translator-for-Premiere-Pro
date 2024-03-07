/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ VM Chai script support (https://github.com/ChaiScript/ChaiScript/)
	(c) CC 2023-2024, MIT

	MMT Script Tester (VM Chai script tester)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#pragma once

#include <syncstream>

using namespace Common;
using namespace tcolor;
using namespace std::placeholders;

namespace APP {

    #pragma region Print
    class Print {
    private:
        static std::atomic<bool> lock_;
        static uint64_t log_count_;

        template<typename T1, typename T2, typename T3, typename T4>
        static void out_(const T1 fore, const T2 back, const T3 sfore, const T4 sback, const std::wstring t, const std::wstring& s, DWORD d = 0U) {
            static const tcolor::fg clr{ fg::yellow };
            Common::awaiter a(std::ref(lock_));
            if (!a.lock_if()) a.lock_wait();
            std::wcout
                << style::reset << clr << L'\n' << log_count_++ << L") "
                << style::reset << fore << back << L" " << t.c_str() << L" " << style::reset << clr << L":"
                << back << ((d) ? L" (" : L"") << fore << ((d) ? std::to_wstring(d) : L"") << clr << ((d) ? L"), " : L"")
                << style::reset << sfore << sback << L" " << s.c_str() << L" " << style::reset << L"\n";
            std::wcout.flush();
        }

        static void out_list_(const tcolor::fg fore, const tcolor::bg back, const std::wstring& s, uint8_t ident = 0) {
            static const tcolor::fg clr{ fg::magenta };
            Common::awaiter a(std::ref(lock_));
            if (!a.lock_if()) a.lock_wait();
            std::wcout
                << style::reset << clr << L"\n*"
                << ((ident == 1) ? L"\t" : ((ident == 2) ? L"\t\t" : L""))
                << style::reset << fore << back << s.c_str() << style::reset;
            std::wcout.flush();
        }
        static void out_help_(const std::wstring& c, const std::wstring& s) {
            Common::awaiter a(std::ref(lock_));
            if (!a.lock_if()) a.lock_wait();
            std::wcout
                << style::reset << L"\n\t'"
                << tcolor::style::bold << c << style::reset << L"' - " << s;
            std::wcout.flush();
        }
        static void out_single_(const tcolor::fg fore, const tcolor::bg back, const std::wstring& s, const bool is_newline) {
            Common::awaiter a(std::ref(lock_));
            if (!a.lock_if()) a.lock_wait();
            std::wcout
                << style::reset << L"\n"
                << fore << back << L" " << s.c_str() << L" " << style::reset << (is_newline ? L"\n" : L"");
            std::wcout.flush();
        }
        static void out_normal_(const tcolor::fg fore, const std::wstring& s) {
            Common::awaiter a(std::ref(lock_));
            if (!a.lock_if()) a.lock_wait();
            std::wcout
                << style::reset << L"\n"
                << fore << bg::black << s.c_str() << style::reset;
            std::wcout.flush();
        }
        static void out_source_(const std::wstring& s) {
            Common::awaiter a(std::ref(lock_));
            if (!a.lock_if()) a.lock_wait();
            std::wcout
                << style::reset << L"\n"
                << fg::green << bg::black <<  s.c_str() << style::reset << L"\n";
            std::wcout.flush();
        }
    public:
        static void out_log(const std::wstring& t, const Common::log_string& ls) {
            Print::out_(fg::blue, bg::yellow, fg::gray, bg::black, t, ls);
        }
        static void out_log(const std::wstring& t, const std::wstring& s) {
            if (s.starts_with(L"[VM/SCRIPT]: # ")) Print::out_(fg::magenta, bg::yellow, tcolor::fg::black, tcolor::bg::yellow, t, s);
            else if (s.starts_with(L"[VM/SCRIPT]: * ")) Print::out_(fg::magenta, bg::yellow, tcolor::fg::black, tcolor::bgB::gray, t, s);
            else if (s.starts_with(L"[VM/SCRIPT]: ^ ") || t.starts_with(L"WARNING")) Print::out_(fg::magenta, bg::yellow, tcolor::fg::black, tcolor::bg::red, t, s);
            else Print::out_(fg::magenta, bg::yellow, fg::gray, bg::black, t, s);
            
        }
        static void out_log_main(const std::wstring& t, const std::wstring& s) {
            Print::out_(fg::yellow, bg::red, fg::gray, bg::black, t, s);
        }
        static void out_dump(const std::wstring& t, const std::wstring& s, DWORD d = 0U) {
            Print::out_(fg::cyan, bg::blue, fg::cyan, bg::black, t, (Common::log_string() << L'\n' << s), d);
        }
        static void out_list(const std::wstring& s, uint8_t ident) {
            Print::out_list_(fg::magenta, bg::black, s, ident);
        }
        static void out_help(const std::wstring& c, const std::wstring& s) {
            Print::out_help_(c, s);
        }
        static void out_call(const std::wstring& s) {
            Print::out_single_(fg::gray, bg::green, s, true);
        }
        static void out_prompt(const std::wstring& s) {
            Print::out_single_(fg::black, bg::cyan, s, false);
        }
        static void out_normal(const std::wstring& s) {
            Print::out_normal_(fg::gray, s);
        }
        static void out_error(const std::wstring& s) {
            Print::out_single_(fg::gray, bg::red, s, true);
        }
        static void out_source(const std::wstring& t, const std::wstring& n, const std::wstring& s) {
            Print::out_(fgB::gray, bg::green, fg::gray, bg::black, t, n);
            out_source_(s);
        }
        static void new_line() {
            Common::awaiter a(std::ref(lock_));
            if (!a.lock_if()) a.lock_wait();
            std::wcout << style::reset << L'\n';
        }
        static void new_cursor() {
            new_cursor(std::wstring());
        }
        static void new_cursor(const std::wstring& t) {
            Common::awaiter a(std::ref(lock_));
            if (!a.lock_if()) a.lock_wait();
            std::wcout
                << tcolor::style::reset << tcolor::fg::yellow << tcolor::style::bold << tcolor::style::dim << L"MMT-SC" << t
                << tcolor::style::reset << tcolor::fg::red << tcolor::style::bold << tcolor::style::dim << L'>' << tcolor::style::reset << L' ';
        }
    };
    uint64_t Print::log_count_{ 0U };
    std::atomic<bool> Print::lock_{ false };
    #pragma endregion
}
