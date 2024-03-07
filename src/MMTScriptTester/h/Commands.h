/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ VM Chai script support (https://github.com/ChaiScript/ChaiScript/)
	(c) CC 2023-2024, MIT

	MMT Script Tester (VM Chai script tester)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#pragma once

using namespace Common;
using namespace tcolor;
using namespace std::placeholders;

namespace APP {

    #pragma region CommandID

    enum class CommandID : uint16_t {
        none = 0U,
        // :0
        help,
        show,
        scan,
        run,
        stop,
        debug,
        terminate,
        // :1
        listscript,
        listrunning,
        config,
        source,
        directory,
    };

    #pragma region Command
    class Command {
    private:
        CommandID cmd1_{ CommandID::none };
        CommandID cmd2_{ CommandID::none };
        uint16_t  cmd3_{ UINT16_MAX };
        std::wstring cmd4_{};
        Common::MIDI::Mackie::Target cmd5_{ Common::MIDI::Mackie::Target::NOTARGET };

    public:
        Command() {
        }
        ~Command() = default;

        const bool empty() const {
            return (cmd1_ == CommandID::none);
        }
        void set_cmd1(const CommandID& id) {
            cmd1_ = id;
        }
        void set_cmd2(const CommandID& id) {
            cmd2_ = id;
        }
        void set_cmd3(const uint16_t idx) {
            cmd3_ = (idx > 0) ? (idx - 1) : idx;
        }
        void set_cmd4(const std::wstring& s) {
            cmd4_ = s;
        }
        void set_cmd5(const uint16_t idx) {
            switch (idx) {
                case  1: cmd5_ = Common::MIDI::Mackie::Target::B11; break;
                case  2: cmd5_ = Common::MIDI::Mackie::Target::B12; break;
                case  3: cmd5_ = Common::MIDI::Mackie::Target::B13; break;
                case  4: cmd5_ = Common::MIDI::Mackie::Target::B14; break;
                case  5: cmd5_ = Common::MIDI::Mackie::Target::B15; break;
                case  6: cmd5_ = Common::MIDI::Mackie::Target::B16; break;
                case  7: cmd5_ = Common::MIDI::Mackie::Target::B17; break;
                case  8: cmd5_ = Common::MIDI::Mackie::Target::B18; break;
                case  9: cmd5_ = Common::MIDI::Mackie::Target::B19; break;
                case 10: cmd5_ = Common::MIDI::Mackie::Target::B21; break;
                case 11: cmd5_ = Common::MIDI::Mackie::Target::B22; break;
                case 12: cmd5_ = Common::MIDI::Mackie::Target::B23; break;
                case 13: cmd5_ = Common::MIDI::Mackie::Target::B24; break;
                case 14: cmd5_ = Common::MIDI::Mackie::Target::B25; break;
                case 15: cmd5_ = Common::MIDI::Mackie::Target::B26; break;
                case 16: cmd5_ = Common::MIDI::Mackie::Target::B27; break;
                case 17: cmd5_ = Common::MIDI::Mackie::Target::B28; break;
                case 18: cmd5_ = Common::MIDI::Mackie::Target::B29; break;
                case 19: cmd5_ = Common::MIDI::Mackie::Target::B31; break;
                case 20: cmd5_ = Common::MIDI::Mackie::Target::B32; break;
                case 21: cmd5_ = Common::MIDI::Mackie::Target::B33; break;
                case 22: cmd5_ = Common::MIDI::Mackie::Target::B34; break;
                case 23: cmd5_ = Common::MIDI::Mackie::Target::B35; break;
                case 24: cmd5_ = Common::MIDI::Mackie::Target::B36; break;
                case 25: cmd5_ = Common::MIDI::Mackie::Target::B37; break;
                case 26: cmd5_ = Common::MIDI::Mackie::Target::B38; break;
                case 27: cmd5_ = Common::MIDI::Mackie::Target::B39; break;
                default: return;
            }
        }

        const CommandID get_cmd1() const {
            return cmd1_;
        }
        const CommandID get_cmd2() const {
            return cmd2_;
        }
        const uint16_t get_cmd3() const {
            return cmd3_;
        }
        const std::wstring& get_cmd4() const {
            return cmd4_;
        }
        const Common::MIDI::Mackie::Target& get_cmd5() const {
            return cmd5_;
        }

        const std::wstring dump() const {
            return
                (Common::log_string()
                    << L"cmd1:" << (uint16_t)cmd1_
                    << L", cmd2:" << (uint16_t)cmd2_
                    << L", cmd3:" << cmd3_
                    << L", cmd4:[" << cmd4_ << L"]"
                    << L", cmd5:" << (uint16_t)cmd5_);
        }
    };
    #pragma endregion

    #pragma region Commands
    class Commands {
    private:
    public:
        static Command parse_Commands(const std::wstring& s) {
            try {
                Command cmd{};
                if (s.empty()) return cmd;

                uint16_t idx{ 0U };
                std::wstringstream ss(s);
                std::wstring word{};

                while (ss >> word) {
                    switch (idx) {
                        case 0U: {
                                 if (word._Equal(L"show") || word._Equal(L"print") || word._Equal(L"sh")) cmd.set_cmd1(CommandID::show);
                            else if (word._Equal(L"scan") || word._Equal(L"rescan") || word._Equal(L"reload")) cmd.set_cmd1(CommandID::scan);
                            else if (word._Equal(L"run")  || word._Equal(L"call") || word._Equal(L"start"))  cmd.set_cmd1(CommandID::run);
                            else if (word._Equal(L"stop") || word._Equal(L"end")) cmd.set_cmd1(CommandID::stop);
                            else if (word._Equal(L"help") || word._Equal(L"?")) cmd.set_cmd1(CommandID::help);
                            else if (word._Equal(L"debug") || word._Equal(L"debugging")) cmd.set_cmd1(CommandID::debug);
                            else if (word._Equal(L"term") || word._Equal(L"terminate")) cmd.set_cmd1(CommandID::terminate);
                            break;
                        }
                        case 1U: {
                            if ((cmd.get_cmd1() == CommandID::run) || (cmd.get_cmd1() == CommandID::stop)) {
                                if (std::all_of(word.begin(), word.end(), ::iswdigit))
                                    cmd.set_cmd5(static_cast<uint16_t>(std::stoul(word)));
                                else
                                    cmd.set_cmd4(word);
                                break;
                            }
                            else if ((cmd.get_cmd1() == CommandID::show) && word._Equal(L"help")) {
                                cmd.set_cmd1(CommandID::help);
                                break;
                            }
                            else if (word._Equal(L"scripts") || word._Equal(L"list")) cmd.set_cmd2(CommandID::listscript);
                            else if (word._Equal(L"run") || word._Equal(L"running") || word._Equal(L"launch")) cmd.set_cmd2(CommandID::listrunning);
                            else if (word._Equal(L"conf") || word._Equal(L"config") || word._Equal(L"configuration")) cmd.set_cmd2(CommandID::config);
                            else if (word._Equal(L"dir") || word._Equal(L"directory")) cmd.set_cmd2(CommandID::directory);
                            else if (word._Equal(L"src") || word._Equal(L"source") || word._Equal(L"code") || word._Equal(L"body")) cmd.set_cmd2(CommandID::source);
                            break;
                        }
                        case 2U: {
                            if (cmd.get_cmd2() == CommandID::source) {
                                if (std::all_of(word.begin(), word.end(), ::iswdigit))
                                    cmd.set_cmd3(static_cast<uint16_t>(std::stoul(word)));
                                else
                                    cmd.set_cmd4(word);
                                break;
                            }
                            break;
                        }
                        default: break;
                    }
                    if (idx == 0U) {
                        if ((cmd.get_cmd1() == CommandID::none) ||
                            (cmd.get_cmd1() == CommandID::scan) ||
                            (cmd.get_cmd1() == CommandID::help) ||
                            (cmd.get_cmd1() == CommandID::debug) ||
                            (cmd.get_cmd1() == CommandID::terminate)) break;
                    }
                    else if (idx == 1U) {
                        if ((cmd.get_cmd1() == CommandID::run) ||
                            (cmd.get_cmd1() == CommandID::stop)) break;
                    }
                    else if (idx == 2U) {
                        break;
                    }
                    idx++;
                }
                return cmd;

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return Command();
        }
    };
    #pragma endregion
}
