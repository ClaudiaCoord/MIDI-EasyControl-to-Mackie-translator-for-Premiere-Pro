/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Joystick support.
	+ GamePad support.
	(c) CC 2023-2024, MIT

	COMMON (Joystick || GamePad) Config

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace GAMEPAD {

		enum class JoystickControlsType : uint16_t {
			CTRL_NONE = 0,
			CTRL_MIX,
			CTRL_MQTT,
			CTRL_LIGHTS,
			CTRL_MIX_AND_MQTT,
			CTRL_LIGHTS_AND_MQTT,
			CTRL_LIGHTS_AND_MQTT_AND_MIX
		};

		class FLAG_EXPORT JoystickConfig {
		public:
			bool enable{ false };
			bool rcontrols{ false };
			bool directasbutton{ false };
			bool horizontalmode{ false };
			uint8_t scene{ 0U };
			uint8_t step{ 1U };
			uint16_t polling{ 150U };
			uint16_t rloglevel{ 0 };
			JoystickControlsType type{ JoystickControlsType::CTRL_NONE };

			void copy(const JoystickConfig&);
			const bool empty() const;
			log_string dump();
		};

		class FLAG_EXPORT JoystickHelper {
		public:
			static std::wstring_view GetJoystickControlsType(const JoystickControlsType&);
			static std::wstring_view GetJoystickControlsType(uint16_t);
		};
	}
}

