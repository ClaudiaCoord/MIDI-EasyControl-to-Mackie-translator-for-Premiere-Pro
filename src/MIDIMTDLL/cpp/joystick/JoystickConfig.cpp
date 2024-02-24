/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Joystick support.
	+ GamePad support.
	(c) CC 2023-2024, MIT

	COMMON (Joystick || GamePad) Config

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace GAMEPAD {

		using namespace std::string_view_literals;

		class Names {
		public:
			#pragma region Helper Names
			static constexpr std::wstring_view NONE = L"none"sv;
			static constexpr std::wstring_view MIX = L"Audio Mixer controls"sv;
			static constexpr std::wstring_view MQTT = L"Smart Home controls"sv;
			static constexpr std::wstring_view LIGHT = L"Lights controls"sv;
			static constexpr std::wstring_view MIX_MQTT = L"Audio Mixer + Smart Home controls"sv;
			static constexpr std::wstring_view LIGHT_MQTT = L"Lights + Smart Home controls"sv;
			static constexpr std::wstring_view MIX_LIGHT_MQTT = L"Audio Mixer + Lights + Smart Home controls"sv;
			#pragma endregion
		};

		static std::wstring_view getJoystickControlsType(const JoystickControlsType& t) {
			switch (t) {
				using enum JoystickControlsType;
				case CTRL_MIX: return Names::MIX;
				case CTRL_MQTT: return Names::MQTT;
				case CTRL_LIGHTS: return Names::LIGHT;
				case CTRL_MIX_AND_MQTT: return Names::MIX_MQTT;
				case CTRL_LIGHTS_AND_MQTT: return Names::LIGHT_MQTT;
				case CTRL_LIGHTS_AND_MQTT_AND_MIX: return Names::MIX_LIGHT_MQTT;
				default: return Names::NONE;
			}
		}
		std::wstring_view JoystickHelper::GetJoystickControlsType(const JoystickControlsType& t) {
			return getJoystickControlsType(t);
		}
		std::wstring_view JoystickHelper::GetJoystickControlsType(uint16_t n) {
			JoystickControlsType t = static_cast<JoystickControlsType>(n);
			return JoystickHelper::GetJoystickControlsType(t);
		}

		void JoystickConfig::copy(const JoystickConfig& c) {
			enable = c.enable;
			scene = c.scene;
			step = c.step;
			polling = c.polling;
			rcontrols = c.rcontrols;
			rloglevel = c.rloglevel;
			directasbutton = c.directasbutton;
			horizontalmode = c.horizontalmode;
			type = c.type;
		}
		const bool JoystickConfig::empty() const {
			return (!rcontrols) ? scene == 0U : type == JoystickControlsType::CTRL_NONE;
		}
		log_string JoystickConfig::dump() {
			log_string ls{};
			ls << L"\tenable module: " << Utils::BOOL_to_string(enable)
			   << L"\n\tdirection as button: " << Utils::BOOL_to_string(directasbutton)
			   << L"\n\thorizontal JOG mode: " << Utils::BOOL_to_string(horizontalmode)
			   << L"\n\tscene: " << (uint16_t)scene << L", step: " << (uint16_t)step
			   << L"\n\tpolling: " << polling << L", log level: " << rloglevel
			   << L"\n\tremote controls: " << Utils::BOOL_to_string(rcontrols)
			   << L", controls type: " << JoystickHelper::GetJoystickControlsType(type);
			return ls;
		}
	}
}