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
	namespace GAMEPAD {

		class JoisticksBase {
		private:
			DWORD_PTR clz_{ 0 };
			hwnd_ptr<hwnd_deleter> hwnj_{};
			JoystickConfig& config_;
			JoistickInput ji_{};
			JoistickControl jc_;
			uint32_t wid_{ 0U };
			uint16_t devs_total_{ 0U };
			uint16_t devs_online_{ 0U };
			uint16_t devs_started_{ 0U };

			const uint16_t build_devices_list_();
			const uint16_t start_devices_(HWND);
			void print_state_(MMRESULT) const;
			void dispose_(const bool = true);

			static LRESULT CALLBACK event_callback_(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

		public:
			static JoystickConfig jsconfig_empty;

			std::vector<JoistickData> data{};
			std::function<void(const std::wstring&)> log_cb = [](const std::wstring&) {};
			std::function<void(MIDI::Mackie::MIDIDATA)> event_cb = [](MIDI::Mackie::MIDIDATA) {};

			JoisticksBase();
			~JoisticksBase();

			const uint16_t TotalDevices() const;
			const uint16_t OnlineDevices() const;
			const uint16_t StartDevices() const;

			const bool empty() const;
			const bool init();
			const bool start(HWND, const JoystickConfig&);
			void stop();
			void update(std::shared_ptr<JSON::MMTConfig>&);
			void clear();
			
		};
	}
}

