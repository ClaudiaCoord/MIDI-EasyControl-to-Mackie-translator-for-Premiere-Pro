/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace PLUGINS {

		using namespace std::string_view_literals;

		class FLAG_EXPORT VmScriptPlugin : public IO::Plugin {
		private:
			const uint32_t id_;
			std::atomic<bool> started_{ false };
			std::atomic<bool> call_command_{ false };
			std::atomic<bool> build_list_{ false };
			std::atomic<bool> build_run_{ false };
			std::atomic<uint16_t> loglevel_{ 1 };
			std::atomic<MIDI::Mackie::MIDIDATA> m_{ MIDI::Mackie::MIDIDATA() };
			std::vector<std::pair<uint16_t, std::wstring>> export_list_{};
			std::shared_ptr<SCRIPT::VmScripts> vmscripts_{};
			VmScriptSetupDialog plugin_ui_;

			void dispose_();
			void load_(SCRIPT::VmScriptConfig&);
			void cb_out_call_(MIDI::MidiUnit&, DWORD);
			void set_config_cb_(std::shared_ptr<JSON::MMTConfig>&);
			void build_export_list_(SCRIPT::scripts_list_t&);
			void build_export_run_(SCRIPT::scripts_run_t&);

		public:

			VmScriptPlugin(std::wstring, HWND);
			~VmScriptPlugin();

			bool load(std::shared_ptr<JSON::MMTConfig>&) override;
			bool load(std::wstring) override;
			bool start(std::shared_ptr<JSON::MMTConfig>&) override;
			bool stop() override;
			void release() override;

			IO::export_list_t& GetDeviceList() override;
			std::any GetDeviceData() override;
			IO::PluginUi& GetPluginUi() override;
		};
	}
}

typedef Common::PLUGINS::VmScriptPlugin ThisPluginClass;

