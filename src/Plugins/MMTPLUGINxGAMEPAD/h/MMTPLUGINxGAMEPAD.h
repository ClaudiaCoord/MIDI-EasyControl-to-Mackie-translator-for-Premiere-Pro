/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Joystick support.
	+ GamePad support.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (GamePad/Joistick Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace PLUGINS {

		using namespace std::string_view_literals;

		class FLAG_EXPORT GamePadPlugin : public IO::Plugin {
		private:
			std::atomic<bool> started_{ false };
			std::vector<std::pair<uint16_t, std::wstring>> devices_list{};
			GamePadSetupDialog plugin_ui_;
			GAMEPAD::JoystickConfig config_{};
			GAMEPAD::JoisticksBase jb_{};

			void dispose_();
			void load_(std::shared_ptr<JSON::MMTConfig>&);
			void set_config_cb_(std::shared_ptr<JSON::MMTConfig>&);
			void start_(GAMEPAD::JoystickConfig&);

		public:

			GamePadPlugin(std::wstring, HWND);
			~GamePadPlugin();

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

typedef Common::PLUGINS::GamePadPlugin ThisPluginClass;

