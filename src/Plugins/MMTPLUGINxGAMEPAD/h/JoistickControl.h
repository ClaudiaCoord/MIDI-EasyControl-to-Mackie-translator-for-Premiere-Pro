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

		class JoistickControl {
		private:
			JoystickConfig& config_;
			std::atomic<JoystickControlsType> index_{ JoystickControlsType::CTRL_NONE };
			std::atomic<uint16_t> index_mix{ 0 };
			std::atomic<uint16_t> index_mqtt{ 0 };
			std::atomic<uint16_t> index_lights{ 0 };

			void update_mix_(std::vector<MIDI::MidiUnit>&);
			void update_mqtt_(std::vector<MIDI::MidiUnit>&);
			void update_lights_(std::vector<MIDI::MidiUnit>&);

			void set_scene_(const bool);
			void set_control_(const bool);
			MIDI::MidiUnit& get_();
			void update_(std::vector<MIDI::MidiUnit>&, std::vector<JoistickContainer>&, const uint8_t[3]);

		public:
			std::vector<JoistickContainer>	units_mix_{};
			std::vector<JoistickContainer>	units_mqtt_{};
			std::vector<JoistickContainer>	units_lights_{};

			JoistickControl();
			~JoistickControl() = default;

			void clear();
			
			void update(const JoystickConfig&);
			void update(std::shared_ptr<JSON::MMTConfig>&);
			const bool change(MIDI::Mackie::MIDIDATA&);
		};
	}
}
