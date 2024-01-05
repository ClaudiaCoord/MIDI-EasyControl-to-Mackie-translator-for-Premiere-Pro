/*
    MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
    + Audio session volume/mute mixer.
    + MultiMedia Key translator.
    (c) CC 2023-2024, MIT

    MIDIMT RENUMBERED part

    See README.md for more details.
    NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "..\Common\rc\template\resource_version_EDIT.h"

#include <string>
#include <sstream>
#include <exception>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

template<typename T1>
std::wstringstream print_trace__(const char* c, std::wstring s, T1 t) {
    std::wstringstream ss;
    ss << L"[" << std::wstring(c, c + strlen(c)) << L"] " << s << L": ";
    if constexpr (std::is_same_v<std::string, T1>)
        ss << std::wstring(t.begin(), t.end());
    else
        ss << t;
    ss << L"\n";
    return ss;
}

template<typename T1>
void print_trace(const char* c, std::wstring s, T1 t) {
    std::wstringstream ss = print_trace__(c, s, t);
    ::OutputDebugStringW(ss.str().c_str());
}

class writer {
private:
    uint32_t idx_{ 0U },
             count_{ 0U };
    std::filesystem::path rootpath_;
    std::wofstream stream_;

    std::wstring to_wstring(const char* c) {
        if (c == nullptr) return L"";
        return std::wstring(c, c + strlen(c));
    }
    std::wstring to_wstring(std::string s) {
        if (s.empty()) return L"";
        return std::wstring(s.begin(), s.end());
    }

public:
    writer() {
        rootpath_ = std::filesystem::current_path();
    }
    ~writer() {
        close();
    }
    bool open(std::wstring s, bool isutf8 = false, uint32_t idx = 0U) {
        try {
            close();
            idx_ = idx;
            std::filesystem::path fsroot(rootpath_);
            fsroot.append(s);

            stream_.open(fsroot.wstring(), std::ios_base::binary | std::ios_base::out | std::ios::trunc);
            if (stream_.is_open() && isutf8) stream_.imbue(std::locale(".utf-8"));
            return stream_.is_open();
        } catch (const std::exception err) {
            print_trace(__func__, L"Exception", err.what());
        }
        return false;
    }
    void write(std::wstring s) {
        stream_ << s;
    }
    void write(const char* key, const char* base, bool iscount = false) {
        std::wstring w(key, key + std::strlen(key));
        std::wstring b(base, base + std::strlen(base));
        stream_ << L"D(" << w;
        if (iscount) stream_ << std::to_wstring(count_++);
        stream_ << L",\t\t\t" << b << L", " << idx_++ << L")\n";
    }
    void close() {
        if (stream_.is_open()) {
            stream_.flush();
            stream_.close();
        }
        idx_ = count_ = 0U;
    }
    void dump(const char* key, int n) {
        std::wcout << L"*[" << to_wstring(key) << L"], [" << n << L"]\n";
    }
};

int main() {
    #define D_(A,B,C) w.write(A, B, false);
    #define D(A,B,C) D_(#A,#B,C)
    try {
        writer w{};
        if (w.open(L"resource_template_out.h", true, 0U)) {
            #include "resource_template.h"
            w.close();
        }
    } catch (const std::runtime_error& err) {
        print_trace(__func__, L"Runtime error", err.what());
    } catch (const std::exception& err) {
        print_trace(__func__, L"Exception", err.what());
    } catch (...) {
        print_trace(__func__, L"Unknown exception", L"..");
    }
    return 0;
}
