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

		class JoistickInput {
		private:
			uint8_t  values[8]{0U};

			const bool parse_button_(MIDI::Mackie::MIDIDATA&, const bool, UINT, WPARAM);
			const bool parse_moved_(MIDI::Mackie::MIDIDATA&, const bool, const bool, uint8_t, UINT, LPARAM);
			const bool parse_moved_as_button_(MIDI::Mackie::MIDIDATA&, UINT, LPARAM);

		public:

			JoistickInput() = default;
			~JoistickInput() = default;

			void reset();
			MIDI::Mackie::MIDIDATA parse(UINT, WPARAM, LPARAM, GAMEPAD::JoystickConfig&);

			static const bool filter(UINT, WPARAM, LPARAM, bool&);
		};
	}
}