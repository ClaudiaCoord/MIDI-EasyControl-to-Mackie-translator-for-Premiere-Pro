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
            log_string ls{};
            ls << L"\tenable module:" << Utils::BOOL_to_string(enable);
            ls << L"\n\twatch: " << Utils::BOOL_to_string(script_watch);

            if (!script_directory.empty())
                ls << L"\n\tscript directory: " << script_directory.c_str();

            if (!script_list.empty()) {
                ls << L"\n\tscripts count: " << script_list.size();
                for (auto& a : script_list)
                    ls << L"\n\t\t- " << a << L",";
                ls.seekp(-1, std::ios_base::end);
            }
            ls << L"\n";
            return ls;
        }
        void VmScriptConfig::copy(const VmScriptConfig& c) {
            enable = c.enable;
            script_watch = c.script_watch;
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
