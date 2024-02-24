/*
	MIDI-MT is a high level application driver for USB MIDI control surface.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once
#include <queue>

namespace Common {
	namespace SCRIPT {

		using namespace std::string_view_literals;

		class VmScripts {
		private:
			std::jthread task_{};
			std::atomic<DWORD> send_count_{ 0U };
			std::queue<uint32_t> script_pack_queue_{};
			std::unique_ptr<VmScriptsTask> vmscriptstask_{};
			std::shared_ptr<JSON::MMTConfig>& mmt_;
			IO::PluginCb& pcb_;
			SCRIPT::VmScriptConfig static_config_{};

			SCRIPT::VmScriptConfig& config_();
			std::vector<MIDI::MidiUnit>& units_();

			void dispose_();
			std::wstring get_default_directory_();
			std::tuple<bool, bool, bool> config_check_();
			std::tuple<bool, bool, bool> config_check_(const std::wstring&, const bool);

			std::shared_ptr<chaiscript::ChaiScript> script_init_();
			void script_bootstrap_(std::shared_ptr<chaiscript::Module>&);
			void script_puts_(const std::wstring&);
			void wait_script_();
			void start_();

		public:

			VmScripts(IO::PluginCb&);
			VmScripts(const VmScripts&) = default;
			~VmScripts();

			void start(const std::wstring&, const bool);
			void start();
			void stop();
			void rescan();
			void reset();

			size_t script_index_selector(const MIDI::Mackie::Target&);
			void call_update(const uint8_t, const uint8_t, const uint8_t);
			void call_script(const MIDI::Mackie::Target&);
			SCRIPT::VmScriptConfig& get_config();
			void set_config(std::shared_ptr<JSON::MMTConfig>&);
			void set_config(SCRIPT::VmScriptConfig&);

			void queue_task_command(const uint32_t);
			void queue_task_command(const PoolActions);

			std::wstring get_script_extension();
			scripts_list_t& get_list();
			scripts_run_t& get_run();

			void event_list();
			void event_list_add(scripts_list_event_t, const uint32_t);
			void event_list_remove(const uint32_t);
			
			void event_run_add(scripts_run_event_t, const uint32_t);
			void event_run_remove(const uint32_t);

			const bool isrun() const;

			template <typename T>
			void puts(const T&);

			template <typename T>
			void print(const T&);
		};
		extern template void VmScripts::puts<std::string>(const std::string&);
		extern template void VmScripts::puts<std::string_view>(const std::string_view&);
		extern template void VmScripts::puts<std::wstring>(const std::wstring&);
		extern template void VmScripts::puts<std::wstring_view>(const std::wstring_view&);
		extern template void VmScripts::puts<log_string>(const log_string&);

		extern template void VmScripts::print<MIDI::MidiUnit>(const MIDI::MidiUnit&);
		extern template void VmScripts::print<MIDI::MidiUnitValue>(const MIDI::MidiUnitValue&);
		extern template void VmScripts::print<MIDI::Mackie::Target>(const MIDI::Mackie::Target&);
		extern template void VmScripts::print<MIDI::Mackie::ClickType>(const MIDI::Mackie::ClickType&);
		extern template void VmScripts::print<MIDI::MidiUnitType>(const MIDI::MidiUnitType&);

		extern template void VmScripts::print<SCRIPT::ColorGroup>(const SCRIPT::ColorGroup&);
		extern template void VmScripts::print<SCRIPT::UnitDef>(const SCRIPT::UnitDef&);
		extern template void VmScripts::print<SCRIPT::RGBWColor>(const SCRIPT::RGBWColor&);

		extern template void VmScripts::print<std::string>(const std::string&);
		extern template void VmScripts::print<std::wstring>(const std::wstring&);
		extern template void VmScripts::print<chaiscript::Type_Info>(const chaiscript::Type_Info&);
		extern template void VmScripts::print<std::vector<MIDI::MidiUnit>>(const std::vector<MIDI::MidiUnit>&);

		extern template void VmScripts::print<int32_t>(const  int32_t&);
		extern template void VmScripts::print<int16_t>(const  int16_t&);
		extern template void VmScripts::print<int8_t>(const   int8_t&);
		extern template void VmScripts::print<uint32_t>(const uint32_t&);
		extern template void VmScripts::print<uint16_t>(const uint16_t&);
		extern template void VmScripts::print<uint8_t>(const  uint8_t&);
		extern template void VmScripts::print<double>(const   double&);
		extern template void VmScripts::print<float>(const    float&);
		extern template void VmScripts::print<size_t>(const   size_t&);
		extern template void VmScripts::print<bool>(const     bool&);
	}
}