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

    #pragma region CallBacks
    class CallBacks : public IO::PluginCb {
    private:
        uint32_t id_{ 0U };
        uint32_t last_run_index_{ UINT32_MAX };
        std::shared_ptr<SCRIPT::VmScripts> vmscripts_{};

        const uint16_t get_script_index_(const std::wstring& s) {
            auto& list = vmscripts_->get_list();
            for (uint16_t i = 0U; i < list.size(); i++)
                if (s._Equal(list[i].namew())) return i;
            return UINT16_MAX;
        }

    public:
        Common::MIDI::Mackie::Target run_by_target{ Common::MIDI::Mackie::Target::NOTARGET };
        std::wstring run_by_name{};
        std::atomic<bool> list_loaded_{ false };

        CallBacks()
            : PluginCb::PluginCb(
                this,
                IO::PluginClassTypes::ClassVmScript,
                (IO::PluginCbType::In2Cb | IO::PluginCbType::Out2Cb | IO::PluginCbType::LogCb | IO::PluginCbType::LogsCb | IO::PluginCbType::ConfCb)) {

            out2_cb_ = std::bind(static_cast<void(CallBacks::*)(MIDI::MidiUnit&, DWORD)>(&CallBacks::out2cb), this, _1, _2);
            in2_cb_ = std::bind(static_cast<void(CallBacks::*)(MIDI::Mackie::MIDIDATA, DWORD)>(&CallBacks::in2cb), this, _1, _2);
            log_cb_ = std::bind(static_cast<void(CallBacks::*)(const std::wstring&)>(&CallBacks::logcb), this, _1);
            logs_cb_ = std::bind(static_cast<void(CallBacks::*)(Common::log_string&)>(&CallBacks::logscb), this, _1);

            id_ = Utils::random_hash(this);
            to_log::Get().add(
                std::bind(static_cast<void(CallBacks::*)(const std::wstring&)>(&CallBacks::mainlogcb), this, _1),
                id_);
        }
        ~CallBacks() {
            to_log::Get().remove(id_);
        }
        void out2cb(MIDI::MidiUnit& u, DWORD d) {
            Print::out_dump(L"MIDIUNIT", u.dump(), d);
        }
        void in2cb(MIDI::Mackie::MIDIDATA m, DWORD d) {
            Print::out_dump(L"MIDIDATA", m.dump(), d);
        }
        void logcb(const std::wstring& s) {
            if (s.contains(L"error:")) Print::out_error(s);
            else Print::out_log(L"LOG-CB", s);
        }
        void logscb(Common::log_string& s) {
            std::wstring w{ s.str() };
            if (w.contains(L"[SCRIPT.VmScript.exec]")) Print::out_error(w);
            else Print::out_log(L"LOGS-CB", w);
        }
        void mainlogcb(const std::wstring& s) {
            Print::out_log_main(L"MAIN-LOG", s);
        }

        void print_list(Common::SCRIPT::scripts_list_t& list) {
            Print::out_list(L"Loaded scripts list:", 1U);
            uint32_t count{ 1U };
            for (auto& a : list)
                Print::out_list((Common::log_string()
                    << std::right << std::setw(4)
                    << count++ << std::setw(0) << L") - " << std::left << std::setw(30)
                    << (Common::log_string() << Utils::to_string(a.name) << L",").str() << std::setw(24)
                    << (Common::log_string() << L"size: " << a.body.size() << L" bytes,").str() << std::setw(22)
                    << (Common::log_string() << L"modify: " << Utils::NANOSECONDS_to_string(a.time)).str()),
                2U);
            Print::new_line();

            list_loaded_.store(true);
            if ((run_by_target != Common::MIDI::Mackie::Target::NOTARGET) || !run_by_name.empty()) {
                const bool b = !run_by_name.empty();
                Print::out_call(
                    (Common::log_string()
                        << L"Script"
                        << (b ? L" '" : L" ") << (b ? run_by_name : L"") << (b ? L"' " : L"")
                        << L"loaded, press  'Enter'  to start..")
                );
            }
        }
        void print_run(Common::SCRIPT::scripts_run_t& list) {
            Print::out_list(L"Running scripts list:", 1U);
            uint32_t count{ 1U };
            for (auto& a : list)
                Print::out_list((Common::log_string()
                    << count++ << L") - "
                    << L"name:" << a->namew().c_str()
                    << L", run:" << a->isrun()
                    << L", end:" << a->isend()
                    << L", empty:" << a->empty()
                    << L", hash:" << a->hash()).str(),
                2U);
            Print::new_line();
        }
        void print_script_source(const std::wstring& s) {
            if (s.empty()) return;
            const uint16_t i = get_script_index_(s);
            if (i != UINT16_MAX) print_script_source(i);
        }
        void print_script_source(const uint16_t id = UINT16_MAX) {
            try {
                uint32_t idx = (id != UINT16_MAX) ? id : last_run_index_;
                if (idx == UINT32_MAX) {
                    Print::out_error(L"You haven't run any script yet");
                    return;
                }
                auto& list = vmscripts_->get_list();
                if (idx >= list.size()) {
                    Print::out_error(
                        (Common::log_string() << L"The previous running script by index " << idx << L" is missing or deleted")
                    );
                    last_run_index_ = UINT32_MAX;
                    return;
                }
                auto& i = list[idx];
                Print::out_source(L"SCRIPT SOURCE CODE", i.namew(), Utils::to_string(i.body));

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
        }
        void print_script_directory() {
            Print::out_dump(L"SCRIPT DIRECTORY", vmscripts_->get_config().script_directory, 0U);
        }
        void print_config() {
            Print::out_dump(L"VM-SCRIPT CONFIG", vmscripts_->get_config().dump(), 0U);
        }
        void print_list_script() {
            print_list(vmscripts_->get_list());
        }
        void print_list_run() {
            print_run(vmscripts_->get_run());
        }
        void print_help() {
            Print::out_help(L"* key command help", L"");
            Print::out_help(L"?", L"call this help..");
            Print::out_help(L"show help", L"to call full commands help");
            Print::out_help(L"1-9", L"call script by number");
            Print::out_help(L"s", L"scan script directory");
            Print::out_help(L"l", L"show loaded scripts list");
            Print::out_help(L"r", L"show running scripts list");
            Print::out_help(L"c", L"show configuration");
            Print::out_help(L"b", L"show the source code of the last running script");
            Print::out_help(L"d", L"set script debugging output to a special program");
            Print::out_help(L"t", L"terminate all running scripts");
            Print::out_help(L"x' or 'q", L"stop and exit");
            Print::new_line();
        }
        void print_help_full() {
            Print::out_help(L"* string command help", L"");
            Print::out_help(L"help", L"call this help..");
            Print::out_help(L"sh|show|print [list|scripts]", L"show loaded scripts list");
            Print::out_help(L"sh|show|print [run|running|launch]", L"show running scripts list");
            Print::out_help(L"sh|show|print [conf|config|configuration]", L"show configuration");
            Print::out_help(L"sh|show|print [dir|directory]", L"show scripts directory");
            Print::out_help(L"sh|show|print [src|code|source] and [number|string]", L"show the source code of the last running script");
            Print::out_help(L"run|call|start [number|string]", L"call script by number or name");
            Print::out_help(L"stop|end [number|string]", L"stop execute running script by number or name");
            Print::out_help(L"scan|rescan|reload", L"scan scripts directory");
            Print::out_help(L"terminate|term", L"terminate all running scripts");
            Print::out_help(L"debug|debugging", L"set script debugging output to a special program");
            Print::new_line();
            print_help();
        }

        void init() {
            try {
                Print::out_prompt(L"Initialize...");
                vmscripts_ = std::make_shared<SCRIPT::VmScripts>(*static_cast<PluginCb*>(this));
                vmscripts_->event_list_add(
                    std::bind(static_cast<void(CallBacks::*)(Common::SCRIPT::scripts_list_t&)>(&CallBacks::print_list), this, _1),
                    id_
                );
                vmscripts_->event_run_add(
                    std::bind(static_cast<void(CallBacks::*)(Common::SCRIPT::scripts_run_t&)>(&CallBacks::print_run), this, _1),
                    id_
                );
                Print::out_prompt(L" -> end");

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
        }
        void reset() const {
            try {
                Print::out_prompt(L"Closing...");
                vmscripts_->reset();
                Print::out_prompt(L" -> end");

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
        }
        void setconfig(SCRIPT::VmScriptConfig& c) {
            try {
                Print::out_prompt(L"Set configuration...");
                common_config& conf = common_config::Get();
                if (!conf.Load()) {
                    Print::out_error(L" -> error: configuration not loaded!");
                    return;
                }
                auto& cnf = conf.GetConfig();
                cnf->vmscript.script_debug = false;
                vmscripts_->set_config(cnf);
                if (!c.empty()) vmscripts_->set_config(c);

                Print::out_prompt((Common::log_string() << L" -> end (load total controls: " << cnf->units.size() << L")"));
                Print::out_dump(L"VM-SCRIPT CONFIG", cnf->vmscript.dump(), 0U);
                if (vmscripts_->get_config().script_debug)
                    Print::out_log(L"WARNING, DEBUG OUTPUT ON", L" -> all output script messages send to a special program..");
                vmscripts_->start();

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
        }
        void call(const std::wstring& s) {
            if (s.empty()) return;
            const uint16_t i = get_script_index_(s);
            if (i != UINT16_MAX)
                call(
                    static_cast<Common::MIDI::Mackie::Target>(i + static_cast<uint16_t>(MIDI::Mackie::Target::B11))
                );
        }
        void call(const MIDI::Mackie::Target& t) {
            try {
                if (!list_loaded_.load()) {
                    Print::out_error(L"Please wait, scripts are still loading..");
                    return;
                }
                Print::out_call((Common::log_string() << "Call -> " << Common::MIDI::MackieHelper::GetScriptTarget(t)));
                vmscripts_->call_script(t);
                Print::out_call(L" -> end");
                Print::new_line();
                last_run_index_ = (static_cast<uint32_t>(t) - static_cast<uint32_t>(MIDI::Mackie::Target::B11));

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
        }
        void rescan_scripts() {
            vmscripts_->rescan();
        }
        void terminate_scripts() {
            vmscripts_->stop_scripts();
        }
        void output_debug() {
            auto& c = vmscripts_->get_config();
            c.script_debug = !c.script_debug;
            Print::out_log(
                std::wstring() + L"DEBUG OUTPUT " + (c.script_debug ? L"ON" : L"OFF"),
                std::wstring() + L" -> all output script messages " + (c.script_debug ? L"send to a special program.." : L"print to local")
            );
        }
        void command_exec(const Command& cmd) {
            try {
                if (cmd.empty()) return;
                // std::wcout << cmd.dump() << L"\n";

                switch (cmd.get_cmd1()) {
                    using enum CommandID;
                    case show: {
                        switch (cmd.get_cmd2()) {
                            using enum CommandID;
                            case config: {
                                print_config();
                                break;
                            }
                            case source: {
                                if (cmd.get_cmd3() != UINT16_MAX)
                                    print_script_source(cmd.get_cmd3());
                                else if (!cmd.get_cmd4().empty())
                                    print_script_source(cmd.get_cmd4());
                                else
                                    print_script_source();
                                break;
                            }
                            case directory: {
                                print_script_directory();
                                break;
                            }
                            case listscript: {
                                print_list_script();
                                break;
                            }
                            case listrunning: {
                                print_list_run();
                                break;
                            }
                            default: break;
                        }
                        break;
                    }
                    case help: {
                        print_help_full();
                        break;
                    }
                    case debug: {
                        output_debug();
                        break;
                    }
                    case run:
                    case stop: {
                        if (cmd.get_cmd5() != Common::MIDI::Mackie::Target::NOTARGET)
                            call(
                                cmd.get_cmd5()
                            );
                        else if (!cmd.get_cmd4().empty())
                            call(
                                cmd.get_cmd4()
                            );
                        else if (last_run_index_ != UINT32_MAX)
                            call(
                                static_cast<Common::MIDI::Mackie::Target>(last_run_index_ + static_cast<uint32_t>(MIDI::Mackie::Target::B11))
                            );
                        else if (run_by_target != Common::MIDI::Mackie::Target::NOTARGET)
                            call(
                                run_by_target
                            );
                        break;
                    }
                    case scan: {
                        rescan_scripts();
                        break;
                    }
                    case terminate: {
                        terminate_scripts();
                        break;
                    }
                    default: break;
                }

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
        }
        void loop() {
            try {
                uint16_t last{ 0U };
                while (true) {
                    uint16_t c = std::wcin.get();
                    if ((c >= 49) && (c <= 57)) {
                             if (c == 49)  call(Common::MIDI::Mackie::Target::B11);
                        else if (c == 50)  call(Common::MIDI::Mackie::Target::B12);
                        else if (c == 51)  call(Common::MIDI::Mackie::Target::B13);
                        else if (c == 52)  call(Common::MIDI::Mackie::Target::B14);
                        else if (c == 53)  call(Common::MIDI::Mackie::Target::B15);
                        else if (c == 54)  call(Common::MIDI::Mackie::Target::B16);
                        else if (c == 55)  call(Common::MIDI::Mackie::Target::B17);
                        else if (c == 56)  call(Common::MIDI::Mackie::Target::B18);
                        else if (c == 57)  call(Common::MIDI::Mackie::Target::B19);
                    }
                    else if ((c == 63) || (c == 72) || (c == 104)) print_help();
                    else if ((c == 66) || (c == 98))  print_script_source();
                    else if ((c == 67) || (c == 99))  print_config();
                    else if ((c == 68) || (c == 100)) output_debug();
                    else if ((c == 76) || (c == 108)) print_list_script();
                    else if ((c == 81) || (c == 88) || (c == 113) || (c == 120)) break;
                    else if ((c == 82) || (c == 114)) print_list_run();
                    else if ((c == 83) || (c == 115)) rescan_scripts();
                    else if ((c == 84) || (c == 116)) terminate_scripts();
                    else if ((c == 10) && (last == 10)) {
                        Print::new_cursor(L" -> enter string command");
                        std::wstring s{};
                        std::getline(std::wcin, s);
                        command_exec(Commands::parse_Commands(s));
                        Print::new_cursor();
                        last = 0;
                        continue;
                    }
                    else if ((c == 98) || (c == 10)) {
                        if (list_loaded_.load()) {
                            if (run_by_target != Common::MIDI::Mackie::Target::NOTARGET) {
                                call(std::ref(run_by_target));
                                run_by_target = Common::MIDI::Mackie::Target::NOTARGET;
                            }
                            else if (!run_by_name.empty()) {
                                call(run_by_name);
                                run_by_name = std::wstring();
                            }
                        }
                    }
                    if (c != 10) Print::new_cursor();
                    // #define DEBUG_INPUT 1
                    #if defined (DEBUG_INPUT)
                    std::wcout << (uint16_t)c << L"/" << last << L"\n";
                    #endif
                    last = c;
                }
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
        }
    };
    #pragma endregion
}
