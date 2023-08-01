﻿/*
    MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
    + Audio session volume/mute mixer.
    + MultiMedia Key translator.
    (c) CC 2023, MIT

    MIDIMT BUILD part

    See README.md for more details.
    NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "..\Common\rc\template\resource_version_EDIT.h"

#if defined(_MIDIMTBUILD64)
#    define CURRENT_PLATFORM L"x64"
#elif defined(_MIDIMTBUILD32)
#    define CURRENT_PLATFORM L"x32"
#else
#    define CURRENT_PLATFORM L"xUN"
#endif

#if defined(_DEBUG)
#    define CURRENT_STATUS L"Debug"
#else
#    define CURRENT_STATUS L"Release"
#endif

#pragma message("* SolutionDir == " BUILDROOT)

#include <string>
#include <sstream>
#include <exception>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

#define print_trace(A,B,C) print_trace_console(A,B,C)

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
void print_trace_vs(const char* c, std::wstring s, T1 t) {
    std::wstringstream ss = print_trace__(c, s, t);
    ::OutputDebugStringW(ss.str().c_str());
}
template<typename T1>
void print_trace_console(const char* c, std::wstring s, T1 t) {
    std::wstringstream ss = print_trace__(c, s, t);
    std::wcout << ss.str().c_str();
}

class writer {
    private:
        std::filesystem::path rootpath__;
        std::wofstream stream__;
        std::wstring ifdef__;

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
            rootpath__ = std::filesystem::current_path();
            print_trace(__func__, L"Root running path", rootpath__.wstring());
            print_trace(__func__, L"Root build path  ", BUILDROOT);
        }
        ~writer() {
            close();
        }
        bool open(std::wstring s, std::wstring orig = L"", bool isutf8 = false) {
            try {
                close();
                const bool istxt = s.ends_with(L".txt");
                std::filesystem::path fsroot(BUILDROOT);
                fsroot.append(s);

                stream__.open(fsroot.wstring(), std::ios_base::binary | std::ios_base::out | std::ios::trunc);
                if (stream__.is_open()) {
                    if (isutf8) stream__.imbue(std::locale(".utf-8"));
                    if (!istxt) {
                        ifdef__ = s;
                        ifdef__.erase(std::remove_if(ifdef__.begin(), ifdef__.end(), [](wchar_t c) {
                            return c == L'\\' || c == L' ' || c == L'.' || c == L':';
                        }), ifdef__.end());
                        stream__ << L"\n#ifndef " << ifdef__ << L"\n#define " << ifdef__ << L" 1\n\n";
                    }
                    if (!orig.empty()) {
                        stream__ << L"/*\n * Part of MIDI-MT build\n * DO NOT EDIT THIS FILE!\n * EDIT ORIGINAL: " << orig << L"\n */\n\n";
                    }
                    print_trace(__func__, L"Write file", s);
                }
                return stream__.is_open();
            } catch (const std::exception err) {
                print_trace(__func__, L"Exception", err.what());
            }
            return false;
        }
        void write(std::wstring s) {
            stream__ << s;
        }
        void write(const char* key, std::wstring s, std::wstring base, int i, int b) {
            std::wstring w(key, key + strlen(key));
            std::size_t pos = w.find(s);
            stream__ << L"\t\terr" << w.substr(pos + s.size()) << L" = ";
            if (w._Equal(base))
                stream__ << b << L",\n";
            else
                stream__ << i << L",\n";
        }
        template<typename T1>
        void write(const char* key, T1 t) {
            stream__ << L"#define " << to_wstring(key) << L"\t\t\"" << t << L"\"\n";
        }
        template<>
        void write(const char* key, int i) {
            stream__ << L"#define " << to_wstring(key) << L"\t\t" << i << L"\n";
        }
        template<>
        void write(const char* key, std::string s) {
            stream__ << L"#define " << to_wstring(key) << L"\t\t" << to_wstring(s) << L"\n";
        }
        void close() {
            if (stream__.is_open()) {
                if (!ifdef__.empty()) stream__ << L"\n#endif\n\n";
                stream__.flush();
                stream__.close();
            }
            ifdef__ = L"";
        }
        void dump(const char* key, int n) {
            std::wcout << L"*[" << to_wstring(key) << L"], [" << n << L"]\n";
        }
};

int main() {
    try {
        #define D_(A,B,C) w.write(A, B + C)
        #define D(A,B,C) D_(#A,B,C)
        #define TEXT_VERSION() VER_MAJOR << L"." << VER_MINOR << L"." << VER_SUB << L"." << VER_BUILD

        writer w;
        std::wstring current_ver{};
        std::wstring publisher_ver{};
        if (w.open(L"Common\\rc\\resource_midimt.h", L"Common\\rc\\template\\resource_midimt_template.h")) {
            #include "..\Common\rc\template\resource_midimt_template.h"
            w.close();
        }
        if (w.open(L"Common\\rc\\resource_mackie.h", L"Common\\rc\\template\\resource_mackie_template.h")) {
            #include "..\Common\rc\template\resource_mackie_template.h"
            w.close();
        }
        if (w.open(L"Common\\rc\\resource_error_id.h", L"Common\\rc\\template\\resource_error_id_template.h")) {
            #include "..\Common\rc\template\resource_error_id_template.h"
            w.close();
        }
        if (w.open(L"Common\\rc\\resource_error_enum.h", L"Common\\rc\\template\\resource_error_id_template.h")) {
            w.write(L"\n#pragma once\n\nnamespace Common {\n\n\tenum class FLAG_EXPORT common_error_id : int {\n");
            #undef D_
            #define D_(A,B,C) w.write(A, L"IDS_ERRORID", L"IDS_ERRORID_BASE", C, B)
            #include "..\Common\rc\template\resource_error_id_template.h"
            w.write(L"\n\t};\n}\n");
            w.close();
        }
        if (w.open(L"Common\\rc\\resource_version.h", L"Common\\rc\\template\\resource_version_EDIT.h", true)) {
            #include "..\Common\rc\template\resource_version_template.h"

            const auto now = std::chrono::system_clock::now();
            std::wstringstream ss;
            ss << PRE_VER_COPYRIGHT << std::format(L"{:%Y/%m}", now) << L" MIT";
            publisher_ver = ss.str();
            w.write("VER_COPYRIGHT", publisher_ver);

            w.write("VER_MAJOR", (int)VER_MAJOR);
            w.write("VER_MINOR", (int)VER_MINOR);
            w.write("VER_SUB",   (int)VER_SUB);
            w.write("VER_BUILD", (int)VER_BUILD);

            ss.str(L"");
            ss << TEXT_VERSION();
            current_ver = ss.str();
            w.write("VER_TXT", current_ver);

            ss.str(L"");
            ss << PRE_VER_GUI_RU << TEXT_VERSION() << L" (" << CURRENT_PLATFORM << L")";
            w.write("VER_GUI_RU", ss.str());

            ss.str(L"");
            ss << PRE_VER_GUI_EN << TEXT_VERSION() << L" (" << CURRENT_PLATFORM << L")";
            w.write("VER_GUI_EN", ss.str());

            ss.str(L"");
            ss << PRE_VER_LAUNCH_HEAD << L" [" << TEXT_VERSION() << L" (" << CURRENT_PLATFORM << L"/" << CURRENT_STATUS << L")]";
            w.write("VER_LAUNCH_DESC", ss.str());

            ss.str(L"");
            ss << PRE_VER_DLL_DESC << L". [" << TEXT_VERSION() << L" (" << CURRENT_PLATFORM << L"/" << CURRENT_STATUS << L")]";
            w.write("VER_EXE_DESC", ss.str());

            ss.str(L"");
            ss << PRE_VER_DLL_HEAD << L" [" << TEXT_VERSION() << L" (" << CURRENT_PLATFORM << L"/" << CURRENT_STATUS << L")]. " << PRE_VER_DLL_DESC;
            w.write("VER_DLL_DESC", ss.str());

            ss.str(L"");
            ss << PRE_VER_DLL_NAME;
            w.write("VER_DLL_NAME", ss.str());

            w.close();
        }
        if (!current_ver.empty()) {
            if (w.open(L"Common\\current_version.txt", L"")) {
                w.write(current_ver);
                w.close();
            }
            if (w.open(L"Common\\rc\\resource_version.iss", L"")) {
                w.write("VERSION", current_ver);
                if (publisher_ver.empty())
                    w.write("PUBLISHER", L"CC");
                else
                    w.write("PUBLISHER", publisher_ver);
                w.write("MidiMtAppName", L"MIDI-MT");
                w.write("MidiMtAppGroup", L"MIDIMT");
                w.write("MidiMtAppURL", L"https://claudiacoord.github.io/MIDI-MT/");
                w.write("MidiMtAppAssocExt", L".cnf");
                w.write("MidiMtAppAssocKey", L"midimt.assoc.extcnf");
                w.close();
            }
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
