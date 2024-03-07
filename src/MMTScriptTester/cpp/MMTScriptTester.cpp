/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ VM Chai script support (https://github.com/ChaiScript/ChaiScript/)
	(c) CC 2023-2024, MIT

	MMT Script Tester (VM Chai script tester)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

#include <iostream>
#include "Print.h"
#include "Commands.h"
#include "CallBacks.h"
#include "..\..\Common\rc\resource_version.h"

    LONG WINAPI uexception_handler(PEXCEPTION_POINTERS ep) {
        if ((ep) && (ep->ExceptionRecord))
            Common::to_log::Get() << (Common::log_string() << __FUNCTIONW__ << L", Unhandled exception: " << std::hex << ep->ExceptionRecord->ExceptionCode);
        return EXCEPTION_CONTINUE_SEARCH;
    }

    void print_header() {
        Common::log_string ls{};
        ls << L"VM Chai sript tester, part of " << VER_GUI_EN << L'\n'
           << VER_COPYRIGHT << L'\n';
        APP::Print::out_normal(ls);
    }

    int wmain(int argc, const wchar_t* argv[]) {
        
        ::SetUnhandledExceptionFilter(uexception_handler);
        std::set_terminate([]() {
            try {
                Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            } catch (...) {
                Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            #if defined(_DEBUG)
            std::abort();
            #endif
        });

        APP::CallBacks pcb{};

        try {

            ::setControlMode(control::Force);
            ::setWinTermMode(winTerm::Native);

            argparse::ArgumentParser args(L"MMTScriptTester.exe", L"");
            args.add_argument()
                .names({ L"-d", L"--directory" })
                .description(L"Set scripts directory, full path");

            args.add_argument()
                .names({ L"-r", L"--run" })
                .description(L"Run script by number, valid 1-9");

            args.add_argument()
                .names({ L"-o", L"--open" })
                .description(L"Run script by name, require full path");

            args.add_argument()
                .names({ L"-w", L"--watch" })
                .description(L"Set scripts watcher enable");

            args.add_argument()
                .names({ L"-b", L"--debug" })
                .description(L"Set script debugging output to a special program");

            args.add_argument()
                .names({ L"-s", L"--no-string-lib" })
                .description(L"Disable using string libraries");

            args.add_argument()
                .names({ L"-u", L"--no-wstring-lib" })
                .description(L"Disable using wide string libraries");

            args.add_argument()
                .names({ L"-m", L"--no-math-lib" })
                .description(L"Disable using math libraries");

            args.enable_help();
            auto err = args.parse(argc, argv);
            if (err) {
                print_header();
                APP::Print::out_error(err.what());
                args.print_help();
                return 0;
            }
            if (args.exists(L"help")) {
                print_header();
                args.print_help();
                return 0;
            }

            Common::worker_background::Get().start();
            SCRIPT::VmScriptConfig config{};

            if (args.exists(L"o")) {
                std::filesystem::path p(args.get<std::wstring>(L"o"));
                if (!std::filesystem::exists(p)) {
                    APP::Print::out_error((Common::log_string() << L"File not found: " << p.wstring()));
                    return -1;
                }
                if (!p.extension().wstring()._Equal(L".mmts")) {
                    APP::Print::out_error((Common::log_string() << L"File extension is not MIDI-MT script: " << p.filename().wstring()));
                    return -1;
                }
                config.enable = true;
                config.script_watch = true;
                config.script_directory = p.parent_path().wstring();
                if (p.has_stem())
                    pcb.run_by_name = p.stem().wstring();
            }
            else if (args.exists(L"d")) {
                config.enable = true;
                config.lib_match = !args.exists(L"m");
                config.lib_string = !args.exists(L"s");
                config.lib_wstring = !args.exists(L"u");
                config.script_debug = args.exists(L"b");
                config.script_watch = args.exists(L"w");
                config.script_directory = args.get<std::wstring>(L"d");
            }
            if (args.exists(L"r"))
                pcb.run_by_target = static_cast<Common::MIDI::Mackie::Target>(args.get<uint32_t>(L"r") + 22U);

            pcb.init();
            APP::Print::out_prompt(L"Init -> successful");
            pcb.setconfig(std::ref(config));
            APP::Print::out_prompt(L"Press  '?'  to call help,  'x' or 'q'  to stop and exit");
            APP::Print::new_line();
            APP::Print::new_cursor();

            pcb.loop();

        } catch (...) {
            Utils::get_exception(std::current_exception(), __FUNCTIONW__);
        }

        pcb.reset();
        APP::Print::out_prompt(L"Reset -> successful");
        Common::worker_background::Get().stop();
        APP::Print::out_prompt(L"Stopped successful -> exit");

        return 0;
    }

