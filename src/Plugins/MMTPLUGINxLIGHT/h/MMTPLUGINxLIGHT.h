/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (LightKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace PLUGINS {

		using namespace std::string_view_literals;

		class FLAG_EXPORT LightsPlugin : public IO::Plugin {
		private:
			std::atomic<DWORD> time_{ 0U };
			std::atomic<DWORD> packet_id_{ 0U };
			std::atomic<bool> dmx_pool_active_{ false };
			std::atomic<bool> dmx_pause_{ false };

			std::unique_ptr<LIGHT::DMXSerial> dmx_;
			std::unique_ptr<LIGHT::DMXArtnet> artnet_;
			std::shared_ptr<locker_awaiter> lock_{};
			LIGHT::DMXPacket dmx_packet_{};

			std::vector<std::pair<uint16_t, std::wstring>> export_list_{};
			LightsSetupDialog plugin_ui_;

			void dispose_();
			void load_(LIGHT::LightsConfig&);
			void set_config_cb_(std::shared_ptr<JSON::MMTConfig>&);
			void cb_out_call_(MIDI::MidiUnit&, DWORD);

			LIGHT::DMXPacket getDMXPacket_();
			void poolDMXPacket_();

		public:

			LightsPlugin(std::wstring, HWND);
			~LightsPlugin();

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

typedef Common::PLUGINS::LightsPlugin ThisPluginClass;

