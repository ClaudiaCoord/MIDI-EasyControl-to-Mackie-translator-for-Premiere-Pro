/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Joystick support.
	+ GamePad support.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (GamePad/Joistick Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace GAMEPAD {

		#define CALC_VALUE_CONTROLS_(X) ((X > 0) ? 1U : 0U)
		#define CALC_ID_AS_BTN_(X,O,V) (O + JOY_BUTTON4 + V + ((X > 0) ? 1 : 0))
		static constexpr uint16_t DEFAULT_NOT_VALUE__{ 32511U };

		static inline uint8_t calc_value_x__(uint16_t x, uint8_t val, uint8_t step) {
			return (x > 0) ? (((val + step) < 127U) ? (val + step) : 127U) : ((val > step) ? (val - step) : 0U);
		}
		static inline uint8_t calc_value_y__(uint16_t y, uint8_t val, uint8_t step) {
			return (y > 0) ? ((val > step) ? (val - step) : 0U) : (((val + step) < 127U) ? (val + step) : 127U);
		}

		#pragma region private parse input
		const bool JoistickInput::parse_button_(MIDI::Mackie::MIDIDATA& m, const bool rcontrols, UINT c, WPARAM w) {
			UINT uw = static_cast<UINT>(w);
			uint8_t b{ 0U };
			if ((uw & JOY_BUTTON1) || (uw & JOY_BUTTON1CHG)) b = JOY_BUTTON1;
			else if ((uw & JOY_BUTTON2) || (uw & JOY_BUTTON2CHG)) b = JOY_BUTTON2;
			else if ((uw & JOY_BUTTON3) || (uw & JOY_BUTTON3CHG)) b = JOY_BUTTON3;
			else if ((uw & JOY_BUTTON4) || (uw & JOY_BUTTON4CHG)) b = JOY_BUTTON4;
			if (!b) return false;

			m.data[1] = b + (((c == MM_JOY1BUTTONDOWN) || (c == MM_JOY1BUTTONUP)) ? JOY_GROUP_1 : JOY_GROUP_2);
			m.data[2] = ((c == MM_JOY1BUTTONDOWN) || (c == MM_JOY2BUTTONDOWN)) ? 127U : 0U;
			if (rcontrols) m.data[3] = m.data[2] ? 1U : 0U;

			return true;
		}
		const bool JoistickInput::parse_moved_(MIDI::Mackie::MIDIDATA& m, const bool rcontrols, const bool horizontalmode, uint8_t step, UINT c, LPARAM l) {
			uint16_t x = LOWORD(l), y = HIWORD(l);
			if ((x == DEFAULT_NOT_VALUE__) && (y == DEFAULT_NOT_VALUE__)) return false;
			const bool is_x_val = (x != DEFAULT_NOT_VALUE__);

			switch (c) {
				case MM_JOY1MOVE: {
					if (is_x_val) {
						m.data[1] = 21;
						m.data[2] = calc_value_x__(x, values[0], step);
						if (!rcontrols && !horizontalmode && values[0] && (values[0] == m.data[2])) return false;
						values[0] = m.data[2];
					}
					else {
						m.data[1] = 22;
						m.data[2] = calc_value_y__(y, values[1], step);
						if (!rcontrols && values[1] && (values[1] == m.data[2])) return false;
						values[1] = m.data[2];
					}
					break;
				}
				case MM_JOY1ZMOVE: {
					if (is_x_val) {
						m.data[1] = 23;
						m.data[2] = calc_value_x__(x, values[2], step);
						if (!rcontrols && values[2] && (values[2] == m.data[2])) return false;
						values[2] = m.data[2];
					}
					else {
						m.data[1] = 24;
						m.data[2] = calc_value_y__(y, values[3], step);
						if (!rcontrols && values[3] && (values[3] == m.data[2])) return false;
						values[3] = m.data[2];
					}
					break;
				}
				case MM_JOY2MOVE: {
					if (is_x_val) {
						m.data[1] = 41;
						m.data[2] = calc_value_x__(x, values[4], step);
						if (!rcontrols && !horizontalmode && values[4] && (values[4] == m.data[2])) return false;
						values[4] = m.data[2];
					}
					else {
						m.data[1] = 42;
						m.data[2] = calc_value_y__(y, values[5], step);
						if (!rcontrols && values[5] && (values[5] == m.data[2])) return false;
						values[5] = m.data[2];
					}
					break;
				}
				case MM_JOY2ZMOVE: {
					if (is_x_val) {
						m.data[1] = 43;
						m.data[2] = calc_value_x__(x, values[6], step);
						if (!rcontrols && values[6] && (values[6] == m.data[2])) return false;
						values[6] = m.data[2];
					}
					else {
						m.data[1] = 44;
						m.data[2] = calc_value_y__(y, values[7], step);
						if (!rcontrols && values[7] && (values[7] == m.data[2])) return false;
						values[7] = m.data[2];
					}
					break;
				}
				default: return false;
			}
			if (rcontrols) {
				if (is_x_val) m.data[3] = CALC_VALUE_CONTROLS_(x);
				else m.data[3] = CALC_VALUE_CONTROLS_(y);
			}
			return true;
		}
		const bool JoistickInput::parse_moved_as_button_(MIDI::Mackie::MIDIDATA& m, UINT c, LPARAM l) {
			uint16_t x = LOWORD(l), y = HIWORD(l);
			if ((x == DEFAULT_NOT_VALUE__) && (y == DEFAULT_NOT_VALUE__)) return false;
			const bool is_x_val = (x != DEFAULT_NOT_VALUE__);

			switch (c) {
				case MM_JOY1MOVE: {
					if (is_x_val) m.data[1] = CALC_ID_AS_BTN_(x, JOY_GROUP_1, 1);
					else m.data[1] = CALC_ID_AS_BTN_(y, JOY_GROUP_1, 3);
					break;
				}
				case MM_JOY1ZMOVE: {
					if (is_x_val) m.data[1] = CALC_ID_AS_BTN_(x, JOY_GROUP_1, 5);
					else m.data[1] = CALC_ID_AS_BTN_(y, JOY_GROUP_1, 7);
					break;
				}
				case MM_JOY2MOVE: {
					if (is_x_val) m.data[1] = CALC_ID_AS_BTN_(x, JOY_GROUP_2, 1);
					else m.data[1] = CALC_ID_AS_BTN_(y, JOY_GROUP_2, 3);
					break;
				}
				case MM_JOY2ZMOVE: {
					if (is_x_val) m.data[1] = CALC_ID_AS_BTN_(x, JOY_GROUP_2, 5);
					else m.data[1] = CALC_ID_AS_BTN_(y, JOY_GROUP_2, 7);
					break;
				}
				default: return false;
			}
			m.data[2] = 127U;
			return true;
		}
		#pragma endregion

		void JoistickInput::reset() {
			std::fill(std::begin(values), std::begin(values) + std::size(values), 0U);
		}
		MIDI::Mackie::MIDIDATA JoistickInput::parse(UINT c, WPARAM w, LPARAM l, GAMEPAD::JoystickConfig& config) {
			MIDI::Mackie::MIDIDATA m{};
			switch (c) {
				case MM_JOY1BUTTONUP:
				case MM_JOY2BUTTONUP:
				case MM_JOY1BUTTONDOWN:
				case MM_JOY2BUTTONDOWN: {
					if (!parse_button_(std::ref(m), config.rcontrols, c, w))
						return MIDI::Mackie::MIDIDATA();
					break;
				}
				case MM_JOY1MOVE:
				case MM_JOY2MOVE:
				case MM_JOY1ZMOVE:
				case MM_JOY2ZMOVE: {
					if (config.directasbutton && !config.rcontrols) {
						if (!parse_moved_as_button_(std::ref(m), c, l))
							return MIDI::Mackie::MIDIDATA();
					}
					else {
						if (!parse_moved_(std::ref(m), config.rcontrols, config.horizontalmode, config.step, c, l))
							return MIDI::Mackie::MIDIDATA();
					}
					break;
				}
				default: break;
			}
			m.data[0] = config.scene;
			return m;
		}

		const bool JoistickInput::filter(UINT c, WPARAM w, LPARAM l, bool& ismoved) {
			ismoved = false;
			switch (c) {
				case MM_JOY1BUTTONUP:
				case MM_JOY2BUTTONUP:
				case MM_JOY1BUTTONDOWN:
				case MM_JOY2BUTTONDOWN: {
					if ((w & JOY_BUTTON1CHG) ||
						(w & JOY_BUTTON2CHG) ||
						(w & JOY_BUTTON3CHG) ||
						(w & JOY_BUTTON4CHG)) return true;
					break;
				}
				case MM_JOY1MOVE:
				case MM_JOY2MOVE:
				case MM_JOY1ZMOVE:
				case MM_JOY2ZMOVE: {
					ismoved = true;
					if ((LOWORD(l) == DEFAULT_NOT_VALUE__) && (HIWORD(l) == DEFAULT_NOT_VALUE__))
						break;
					return true;
				}
				default: break;
			}
			return false;
		}
	}
}