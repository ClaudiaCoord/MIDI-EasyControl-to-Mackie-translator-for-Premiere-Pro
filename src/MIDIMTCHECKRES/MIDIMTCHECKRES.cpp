/*
    MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
    + Audio session volume/mute mixer.
    + MultiMedia Key translator.
    (c) CC 2023-2024, MIT

    MIDIMT CHECK RESOURCE ID part

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
#include <array>
#include <tuple>
#include <memory>
#include <utility>
#include <cstdio>
#include <future>
#include <atomic>
#include <format>

typedef std::pair<std::string, std::array<uint32_t, 3>> pair_t;

#define D_(A) A ,
#define D(A,B,C) D_(#A)
#define SIZE_(A) ((sizeof(A)) / (sizeof(A[0])))

const char* includes_[] = {
    "Common", "Languages\\MIDIMTxRU","Plugins\\shared",
    "EASYCTRL9","MIDIMT","MIDIMTBR","MIDIMTLIB","MIDIMTMIX","MIDIMTVEUI",
    "Plugins\\MMTPLUGINxLIGHT","Plugins\\MMTPLUGINxMIDICTRL","Plugins\\MMTPLUGINxMMKEYS","Plugins\\MMTPLUGINxMQTT"
};
const char* excludes_[] = {
    "\\ASIO\\", "\\enumser\\","\\scintilla\\", "\\lib\\"
};
const char* tokens_[] = {
    #include "..\Common\rc\template\resource_midimt_template.h"
};

enum class FileTypes : int {
    T_NONE = 0,
    T_CPP,
    T_RC,
    T_H
};

/*
std::atomic<bool> is_write{ false };

void print__(std::stringstream ss) {
    while (is_write.load(std::memory_order_acquire))
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    is_write.store(true, std::memory_order_release);
    std::cout << ss.str();
    std::cout.flush();
    is_write.store(false, std::memory_order_release);
}
void print__(std::string s) {
    while (is_write.load(std::memory_order_acquire))
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    is_write.store(true, std::memory_order_release);
    std::cout << s;
    std::cout.flush();
    is_write.store(false, std::memory_order_release);
}

template<typename T1>
void print_trace__(const char* c, std::string s, T1 t) {
    std::stringstream ss{};
    ss << "[" << std::string(c) << "] " << s << ": ";
    if constexpr (std::is_same_v<std::wstring, T1>)
        ss << std::string(t.begin(), t.end());
    else if constexpr (std::is_same_v<std::string, T1>)
        ss << t;
    else
        ss << "bad argument";
    ss << "\n";
    print__(std::move(ss));
}
*/
    class ConsoleWriter {
    private:
        std::atomic<bool> is_write_{ false };
    public:

        void Print(std::stringstream ss) {
            while (is_write_.load(std::memory_order_acquire))
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

            is_write_.store(true, std::memory_order_release);
            std::cout << ss.str();
            std::cout.flush();
            is_write_.store(false, std::memory_order_release);
        }
        void Print(std::string s) {
            while (is_write_.load(std::memory_order_acquire))
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

            is_write_.store(true, std::memory_order_release);
            std::cout << s;
            std::cout.flush();
            is_write_.store(false, std::memory_order_release);
        }

        template<typename T1>
        void Print_trace(const char* c, std::string s, T1 t) {
            std::stringstream ss{};
            ss << "[" << std::string(c) << "] " << s << ": ";
            if constexpr (std::is_same_v<std::wstring, T1>)
                ss << std::string(t.begin(), t.end());
            else if constexpr (std::is_same_v<std::string, T1>)
                ss << t;
            else
                ss << "bad argument";
            ss << "\n";
            Print(std::move(ss));
        }
        void Print_exception(std::exception_ptr ptr, std::string f) {
            if (!ptr) return;
            std::stringstream ss;

            try {
                try {
                    std::rethrow_exception(ptr);
                } catch (const std::future_error& e) {
                    ss << e.what();
                } catch (const std::filesystem::filesystem_error& e) {
                    ss << e.what();
                } catch (const std::runtime_error& e) {
                    ss << e.what();
                } catch (const std::bad_exception& e) {
                    ss << e.what();
                } catch (const std::exception& e) {
                    ss << e.what();
                } catch (...) {
                    ss << "Unknown exception";
                }
                std::string s = ss.str();
                if (s.empty()) return;
                Print(s);

            } catch (...) {}
        }
    } cw{};

    class ChecksTokens {
    private:
        std::filesystem::path root_{};
        std::vector<pair_t> list_{};
        std::atomic<uint32_t> tokens_total{ 0 }, tokens_found{ 0 }, tokens_not_found{ 0 }, files_total{ 0 };
        const bool print_dir_name_{ false };

        uint32_t find_count_() {
            uint32_t cnt{ 0U };
            for (auto& p : list_)
                if (!p.second[0] && !p.second[1] && !p.second[2]) cnt++;
            return cnt;
        }
        const bool check_excludes_(std::string& path) {
            for (size_t i = 0; i < SIZE_(excludes_); i++)
                if (path.find(excludes_[i]) != std::string::npos)
                    return false;
            return true;
        }

        void check_(FileTypes ft, std::string token, std::string path, std::string pipe) {
            try {

                std::stringstream ss{};
                ss << "grep -R -w -c -o \"" << token << "\" " << path << " >" << pipe;

                if (std::system(ss.str().c_str()) == 0) {
                    try {
                        std::stringstream so{};
                        so << std::ifstream(pipe).rdbuf();
                        uint32_t val = std::stoul(so.str());

                        auto it = std::find_if(list_.begin(), list_.end(), [token](pair_t& p) {
                            if (p.first._Equal(token)) return true;
                            return false;
                        });
                        bool empty = (it == list_.end());

                        if (!val) {
                            if (empty) {
                                list_.push_back(
                                    pair_t(
                                        token,
                                        std::array<uint32_t, 3>{0U, 0U, 0U}
                                    )
                                );
                            }
                            return;
                        }
                        if (empty) {
                            list_.push_back(
                                pair_t(
                                    token,
                                    std::array<uint32_t, 3>{
                                        (ft == FileTypes::T_CPP) ? val : 0U,
                                        (ft == FileTypes::T_H)   ? val : 0U,
                                        (ft == FileTypes::T_RC)  ? val : 0U
                                    }
                                )
                            );
                        } else {
                            pair_t& t = static_cast<pair_t&>(*it);
                            switch (ft) {
                                case FileTypes::T_NONE: break;
                                case FileTypes::T_CPP: t.second[0] = (t.second[0] + val); break;
                                case FileTypes::T_H:   t.second[1] = (t.second[1] + val); break;
                                case FileTypes::T_RC:  t.second[2] = (t.second[2] + val); break;
                                default: break;
                            }
                        }

                    } catch (...) {
                        cw.Print_exception(std::current_exception(), __FUNCTION__);
                    }
                }
            } catch (...) {
                cw.Print_exception(std::current_exception(), __FUNCTION__);
            }
        }
        void run_(const char* arr[], size_t sz, std::string path, uint32_t id) {
            try {

                uint32_t count{ 0 };
                std::filesystem::path p(path);
                std::string pipe = "__token_found_" + std::to_string(id) + ".tmp";
                
                for (auto& a : std::filesystem::recursive_directory_iterator(p)) {

                    if (!a.is_regular_file()) {
                        if (print_dir_name_)
                            cw.Print((std::stringstream() << "\n\t[" << id << "]\t+Directory: " << a.path().string().substr(root_.string().length())));
                        continue;
                    }
                    
                    FileTypes ft = FileTypes::T_NONE;

                    std::string ext(a.path().extension().string());
                    if (ext._Equal(".cpp")) ft = FileTypes::T_H;
                    else if (ext._Equal(".h")) ft = FileTypes::T_CPP;
                    else if (ext._Equal(".rc")) ft = FileTypes::T_RC;
                    if (ft == FileTypes::T_NONE) continue;

                    count++;

                    std::string file(a.path().string());
                    if (!check_excludes_(file)) continue;

                    for (size_t i = 0; i < sz; i++)
                        check_(ft, arr[i], file, pipe);
                }
                if (std::filesystem::exists(pipe))
                    std::filesystem::remove(pipe);

                cw.Print((std::stringstream() << "\n\t[" << id << "]\t*End directory: " << path.substr(root_.string().length()) << " | files: " << count));
                (void) files_total.fetch_add(count, std::memory_order_release);

            } catch (...) {
                cw.Print_exception(std::current_exception(), __FUNCTION__);
            }
        }

    public:

        ChecksTokens(std::string s, bool isprint) : root_(s), print_dir_name_(isprint) {}

        void Check(const char* arr[], size_t sz) {
            try {
                std::vector<std::future<void>> v{};

                for (uint32_t i = 0; i < SIZE_(includes_); i++) {
                    v.push_back(
                        std::async(std::launch::async, [=]() {
                            std::filesystem::path p(root_);
                            p.append(includes_[i]);
                            run_(arr, sz, p.string(), i);
                        })
                    );
                }
                for (auto& a : v)
                    try {
                        (void) a.get();
                    } catch (const std::future_error& err) {
                        cw.Print_trace(__FUNCTION__, "Future exception", err.what());
                    }

                (void) tokens_not_found.fetch_add(find_count_(), std::memory_order_release);
                (void) tokens_total.fetch_add(static_cast<uint32_t>(sz), std::memory_order_release);
                (void) tokens_found.fetch_add(static_cast<uint32_t>(list_.size() - tokens_not_found), std::memory_order_release);

            } catch (...) {
                cw.Print_exception(std::current_exception(), __FUNCTION__);
            }
        }
        std::stringstream GetReport(bool show_empty) {
            std::stringstream ss{};
            try {
                for (auto& a : list_) {
                    if (show_empty) {
                        if (!a.second[0] && !a.second[1] && !a.second[2])
                            ss << "\n\t" << a.first << " - EMPTY";
                    } else {
                        if (a.second[0] || a.second[1] || a.second[2])
                            ss << "\n\t" << a.first << ", CPP:" << a.second[0] << ", H:" << a.second[1] << ", RC:" << a.second[2];
                        else
                            ss << "\n\t" << a.first << " - EMPTY";
                    }
                }
                ss << "\n\tTokens total:     " << tokens_total.load(std::memory_order_relaxed)
                   << "\n\tTokens found:     " << tokens_found.load(std::memory_order_relaxed)
                   << "\n\tTokens NOT found: " << tokens_not_found.load(std::memory_order_relaxed)
                   << "\n\tCheck files:      " << files_total.load(std::memory_order_relaxed)
                   << "\n";

            } catch (...) {
                cw.Print_exception(std::current_exception(), __FUNCTION__);
            }
            return ss;
        }
        std::string GetReportPath(std::string s) {
            std::filesystem::path p(root_);
            p.append(s);
            return p.string();
        }
    };

int main() {

    try {
        cw.Print((std::stringstream() << "\n\tTotal tokens_ in base: " << SIZE_(tokens_) << "\n\n"));
        std::unique_ptr<ChecksTokens> ptr = std::make_unique<ChecksTokens>(BUILDROOT, true);

        std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
        ptr->Check(tokens_, SIZE_(tokens_));

        std::stringstream ss_f = ptr->GetReport(false);
        std::stringstream ss_n = ptr->GetReport(true);

        std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();
        auto p = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        ss_n << "\n\tTotal times: " << std::format("{:%T}", std::chrono::floor<std::chrono::milliseconds>(p)) << "\n";
        cw.Print(ss_f.str());
        cw.Print(ss_n.str());

        std::ofstream f_n(ptr->GetReportPath("__Checks_NOT_FOUND_Report.txt"), std::ios::trunc);
        if (f_n.is_open()) {
            f_n << ss_n.str();
            f_n.close();
        }

        std::ofstream f_f(ptr->GetReportPath("__Checks_FOUND_Report.txt"), std::ios::trunc);
        if (f_f.is_open()) {
            f_f << ss_f.str();
            f_f.close();
        }

    } catch (...) {
        cw.Print_exception(std::current_exception(), __FUNCTION__);
    }
    return 0;
}
