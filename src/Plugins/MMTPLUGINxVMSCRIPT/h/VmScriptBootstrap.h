/*
	MIDI-MT is a high level application driver for USB MIDI control surface.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#pragma once

namespace Common {
	namespace SCRIPT {

		class VmScript;
		class VmScripts;
		class ScriptEntry {
		public:
			uint64_t time{ 0U };
			uint32_t hash{ 0U };
			std::string name{};
			std::string body{};

			ScriptEntry(const std::string&, const std::string&, uint32_t = 0U, uint64_t = 0U);
			~ScriptEntry() = default;

			std::wstring namew();
		};
		typedef std::vector<ScriptEntry> scripts_list_t;
		typedef std::vector<std::unique_ptr<VmScript>> scripts_run_t;
		typedef std::function<void(scripts_list_t&)> scripts_list_event_t;
		typedef std::function<void(scripts_run_t&)> scripts_run_event_t;

		class ScriptBootstrap {
		public:
			static std::shared_ptr<chaiscript::Module> script_module();
			static void script_enum_unittype(std::shared_ptr<chaiscript::ChaiScript>&);
			static void script_enum_clicktype(std::shared_ptr<chaiscript::ChaiScript>&);
			static void script_enum_colorgroups(std::shared_ptr<chaiscript::ChaiScript>&);
			static void script_enum_mackietarget(std::shared_ptr<chaiscript::ChaiScript>&);
		};
	}
}