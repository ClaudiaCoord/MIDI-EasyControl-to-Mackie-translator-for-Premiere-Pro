/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (MultimediaKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace PLUGINS {

		using namespace std::string_view_literals;

		class FLAG_EXPORT MMKeysPlugin : public IO::Plugin {
		private:
			std::atomic<bool> started_{ false };
			std::vector<std::pair<uint16_t, std::wstring>> empty_list{};
			MMKeysSetupDialog plugin_ui_;

			void dispose_();
			void load_(MIDI::MMKeyConfig&);
			void cb_out_call_(MIDI::MidiUnit&, DWORD);
			void set_config_cb_(std::shared_ptr<JSON::MMTConfig>&);

		public:

			MMKeysPlugin(std::wstring);
			~MMKeysPlugin();

			bool load(std::shared_ptr<JSON::MMTConfig>&) override;
			bool load(std::wstring) override;
			bool start(std::shared_ptr<JSON::MMTConfig>&) override;
			bool stop() override;
			void release() override;

			std::vector<std::pair<uint16_t, std::wstring>>& GetDeviceList() override;
			std::any GetDeviceData() override;
			IO::PluginUi& GetPluginUi() override;
		};
	}
}

typedef Common::PLUGINS::MMKeysPlugin ThisPluginClass;

