/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol.
	+ Art-Net protocol.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace SCRIPT {

        VmScriptConfig::VmScriptConfig() {
        }
        const bool VmScriptConfig::empty() {
            return script_directory.empty();
        }
        log_string VmScriptConfig::dump() {
            log_delimeter ld{};
            log_string ls{};
            ls << L"\tenable module:" << Utils::BOOL_to_string(enable)
               << L"\n\twatch: " << Utils::BOOL_to_string(script_watch)
               << L"\n\tdebug: " << Utils::BOOL_to_string(script_debug)
               << L"\n\tlib match: " << Utils::BOOL_to_string(lib_match)
               << L"\n\tlib string: " << Utils::BOOL_to_string(lib_string)
               << L"\n\tlib wstring: " << Utils::BOOL_to_string(lib_wstring);

            if (!script_directory.empty())
                ls << L"\n\tscript directory: " << script_directory.c_str();

            if (!script_list.empty()) {
                ls << L"\n\tscripts count: " << script_list.size();
                for (auto& a : script_list)
                    ls << ld << L"\n\t\t- " << a;
                ls.seekp(-1, std::ios_base::end);
            }
            ls << L"\n";
            return ls;
        }
        void VmScriptConfig::copy(const VmScriptConfig& c) {
            enable = c.enable;
            lib_match = c.lib_match;
            lib_string = c.lib_string;
            lib_wstring = c.lib_wstring;
            script_watch = c.script_watch;
            script_debug = c.script_debug;
            script_directory = c.script_directory;
            script_list.clear();
            script_list.assign_range(c.script_list);
        }

        void VmScriptConfig::add(const std::wstring& name) {
            script_list.push_back(name);
        }
        void VmScriptConfig::clear() {
            script_list.clear();
        }
        size_t VmScriptConfig::count() {
            return script_list.size();
        }
    }
}
