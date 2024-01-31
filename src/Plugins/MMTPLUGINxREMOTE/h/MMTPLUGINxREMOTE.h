/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (Remote Controls Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace PLUGINS {

		using namespace std::string_view_literals;

		class FLAG_EXPORT RemotePlugin : public IO::Plugin {
		private:
			std::atomic<bool> started_{ false };
			std::atomic<bool> wakepos_{ false };
			std::atomic<uint16_t> loglevel_{ 1 };
			std::atomic<MIDI::Mackie::MIDIDATA> m_{ MIDI::Mackie::MIDIDATA() };
			std::vector<std::pair<uint16_t, std::wstring>> export_list_{};
			RemoteSetupDialog plugin_ui_;
			WS::SocketServer<WS::AWS> ws_{};
			WS::SocketServerBase<WS::AWS>::Endpoint& wse_;
			std::thread wsthread_{};

			void dispose_();
			void load_(REMOTE::RemoteConfig<std::wstring>&);
			bool start_();
			bool stop_();
			void cb_out_call_(MIDI::MidiUnit&, DWORD);
			void set_config_cb_(std::shared_ptr<JSON::MMTConfig>&);
			const std::string get_build_config_(std::shared_ptr<JSON::MMTConfig>&);

		public:

			RemotePlugin(std::wstring, HWND);
			~RemotePlugin();

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

typedef Common::PLUGINS::RemotePlugin ThisPluginClass;

