/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (MidiCtrlPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace PLUGINS {

		using namespace std::string_view_literals;

		class FLAG_EXPORT MidiCtrlPlugin : public IO::Plugin {
		private:
			std::vector<std::pair<uint16_t, std::wstring>> export_list_{};
			std::vector<std::unique_ptr<MIDI::MidiControllerIn>> dev_in_list_{};
			std::vector<std::unique_ptr<MIDI::MidiControllerOut>> dev_out_list_{};
			std::unique_ptr<MIDI::MidiControllerProxy> dev_proxy_{};
			MidiCtrlSetupDialog plugin_ui_;

			void dispose_();
			void load_(MIDI::MidiConfig&);
			void set_config_cb_(std::shared_ptr<JSON::MMTConfig>&);
			void cb_out_call_(MIDI::Mackie::MIDIDATA, DWORD);
			void cb_out_call_(MIDI::MidiUnit&, DWORD);

		public:

			MidiCtrlPlugin(std::wstring, HWND);
			~MidiCtrlPlugin();

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

typedef Common::PLUGINS::MidiCtrlPlugin ThisPluginClass;

