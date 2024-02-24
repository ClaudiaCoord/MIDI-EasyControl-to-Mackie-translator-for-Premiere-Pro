/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (SmartHomeKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace PLUGINS {

		using namespace std::string_view_literals;

		class FLAG_EXPORT MQTTPlugin : public IO::Plugin {
		private:
			std::atomic<DWORD> time_{ 0U };
			std::vector<std::pair<uint16_t, std::wstring>> export_list_{};
			std::unique_ptr<MQTT::Broker> broker_{};
			SmartHomeSetupDialog plugin_ui_;

			void dispose_();
			void load_(MQTT::BrokerConfig<std::wstring>&);
			void set_config_cb_(std::shared_ptr<JSON::MMTConfig>&);
			void cb_out_call_(MIDI::MidiUnit&, DWORD);

		public:

			MQTTPlugin(std::wstring, HWND);
			~MQTTPlugin();

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

typedef Common::PLUGINS::MQTTPlugin ThisPluginClass;
