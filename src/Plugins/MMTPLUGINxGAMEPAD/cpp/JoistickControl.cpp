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

		static MIDI::MidiUnit munit_empty__{};
		static inline uint8_t calc_slider_value__(uint8_t x, uint8_t val, uint8_t step) {
			return (x > 0) ? ((val > step) ? (val - step) : 0U) : (((val + step) < 127U) ? (val + step) : 127U);
		}
		static inline const bool is_value_invert__(MIDI::MidiUnitType& t) {
			return ((t == MIDI::MidiUnitType::FADERINVERT) ||
					(t == MIDI::MidiUnitType::SLIDERINVERT) ||
					(t == MIDI::MidiUnitType::KNOBINVERT));
		}
		static inline void log_debug_message__(const common_error_id& id, const JoystickControlsType& t, uint16_t i) {
			to_log::Get() << log_string().to_log_format(__FUNCTIONW__,
				common_error_code::Get().get_error(id),
				JoystickHelper::GetJoystickControlsType(t), i);
		}

		#pragma region JoistickControl
		JoistickControl::JoistickControl()
			: config_(JoisticksBase::jsconfig_empty), index_(JoystickControlsType::CTRL_NONE) {
		}

		#pragma region private
		void JoistickControl::update_mix_(std::vector<MIDI::MidiUnit>& u) {
			try { units_mix_.clear(); } catch (...) {}
			worker_background::Get().to_async(std::async(std::launch::async, [=](std::vector<MIDI::MidiUnit>& v) {
					uint8_t arr[]{ MIDI::Mackie::Target::VOLUMEMIX, 0, 0 };
					update_(v, std::ref(units_mix_), arr);
				}, std::ref(u))
			);
		}
		void JoistickControl::update_mqtt_(std::vector<MIDI::MidiUnit>& u) {
			try { units_mqtt_.clear(); } catch (...) {}
			worker_background::Get().to_async(std::async(std::launch::async, [=](std::vector<MIDI::MidiUnit>& v) {
					uint8_t arr[]{ MIDI::Mackie::Target::MQTTKEY, 0, 0 };
					update_(v, std::ref(units_mqtt_), arr);
				}, std::ref(u))
			);
		}
		void JoistickControl::update_lights_(std::vector<MIDI::MidiUnit>& u) {
			try { units_lights_.clear(); } catch (...) {}
			worker_background::Get().to_async(std::async(std::launch::async, [=](std::vector<MIDI::MidiUnit>& v) {
					uint8_t arr[]{ MIDI::Mackie::Target::LIGHTKEY8B, MIDI::Mackie::Target::LIGHTKEY16B, 0 };
					update_(v, std::ref(units_lights_), arr);
				}, std::ref(u))
			);
		}
		void JoistickControl::update_(std::vector<MIDI::MidiUnit>& src, std::vector<JoistickContainer>& dst, const uint8_t ids[3]) {
			try {
				if (src.empty()) return;
				for (auto& a : src)
					for (uint8_t i = 0U; i < 3; i++) {
						if (!ids[i]) break;
						if ((a.target == ids[i]) && (a.type != MIDI::MidiUnitType::BTN) && (a.type != MIDI::MidiUnitType::BTNTOGGLE))
							dst.push_back(JoistickContainer(a));
					}

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}

		void JoistickControl::set_scene_(const bool next) {
			switch (config_.type) {
				using enum JoystickControlsType;
				case CTRL_NONE: {
					if (index_.load() != JoystickControlsType::CTRL_NONE)
						index_.store(JoystickControlsType::CTRL_NONE);
					break;
				}
				case CTRL_MIX: {
					if (index_.load() != JoystickControlsType::CTRL_MIX)
						index_.store(JoystickControlsType::CTRL_MIX);
					break;
				}
				case CTRL_MQTT: {
					if (index_.load() != JoystickControlsType::CTRL_MQTT)
						index_.store(JoystickControlsType::CTRL_MQTT);
					break;
				}
				case CTRL_LIGHTS: {
					if (index_.load() != JoystickControlsType::CTRL_LIGHTS)
						index_.store(JoystickControlsType::CTRL_LIGHTS);
					break;
				}
				case CTRL_MIX_AND_MQTT: {
					switch (index_) {
						using enum JoystickControlsType;
						case CTRL_MIX: {
							index_.store(JoystickControlsType::CTRL_MQTT);
							break;
						}
						case CTRL_MQTT: {
							index_.store(JoystickControlsType::CTRL_MIX);
							break;
						}
						default: {
							index_.store(JoystickControlsType::CTRL_MIX);
							break;
						}
					}
					break;
				}
				case CTRL_LIGHTS_AND_MQTT: {
					switch (index_) {
						using enum JoystickControlsType;
						case CTRL_LIGHTS: {
							index_.store(JoystickControlsType::CTRL_MQTT);
							break;
						}
						case CTRL_MQTT: {
							index_.store(JoystickControlsType::CTRL_LIGHTS);
							break;
						}
						default: {
							index_.store(JoystickControlsType::CTRL_LIGHTS);
							break;
						}
					}
					break;
				}
				case CTRL_LIGHTS_AND_MQTT_AND_MIX: {
					switch (index_) {
						using enum JoystickControlsType;
						case CTRL_LIGHTS: {
							index_.store(JoystickControlsType::CTRL_MIX);
							break;
						}
						case CTRL_MIX: {
							index_.store(JoystickControlsType::CTRL_MQTT);
							break;
						}
						case CTRL_MQTT: {
							index_.store(JoystickControlsType::CTRL_LIGHTS);
							break;
						}
						default: {
							index_.store(JoystickControlsType::CTRL_LIGHTS);
							break;
						}
					}
					break;
				}
				default: break;
			}
			if (config_.rloglevel > 3)
				log_debug_message__(common_error_id::err_JOY_CHANGE_SCENE, index_.load(), 1U);
		}
		void JoistickControl::set_control_(const bool next) {
			switch (index_.load()) {
				using enum JoystickControlsType;
				case CTRL_NONE: break;
				case CTRL_MIX: {
					if (!units_mix_.size()) break;
					uint16_t idx = next ? (index_mix.load() + 1) : (index_mix.load() - 1);
					index_mix.store((idx < units_mix_.size()) ? idx : 0U);
					if (config_.rloglevel > 3)
						log_debug_message__(common_error_id::err_JOY_CHANGE_CTRL, index_.load(), index_mix.load());
					break;
				}
				case CTRL_MQTT: {
					if (!units_mqtt_.size()) break;
					uint16_t idx = next ? (index_mqtt.load() + 1) : (index_mqtt.load() - 1);
					index_mqtt.store((idx < units_mqtt_.size()) ? idx : 0U);
					if (config_.rloglevel > 3)
						log_debug_message__(common_error_id::err_JOY_CHANGE_CTRL, index_.load(), index_mqtt.load());
					break;
				}
				case CTRL_LIGHTS: {
					if (!units_lights_.size()) break;
					uint16_t idx = next ? (index_lights.load() + 1) : (index_lights.load() - 1);
					index_lights.store((idx < units_lights_.size()) ? idx : 0U);
					if (config_.rloglevel > 3)
						log_debug_message__(common_error_id::err_JOY_CHANGE_CTRL, index_.load(), index_lights.load());
					break;
				}
				default: break;
			}
		}
		MIDI::MidiUnit& JoistickControl::get_() {
			try {
				switch (index_.load()) {
					using enum JoystickControlsType;
					case CTRL_NONE: return std::ref(munit_empty__);
					case CTRL_MIX: {
						if (!units_mix_.size()) return std::ref(munit_empty__);
						index_mix.store((index_mix.load() < units_mix_.size()) ? index_mix.load() : 0U);
						return units_mix_[index_mix.load()].get();
					}
					case CTRL_MQTT: {
						if (!units_mqtt_.size()) return std::ref(munit_empty__);
						index_mqtt.store((index_mqtt.load() < units_mqtt_.size()) ? index_mqtt.load() : 0U);
						return units_mqtt_[index_mqtt.load()].get();
					}
					case CTRL_LIGHTS: {
						if (!units_lights_.size()) return std::ref(munit_empty__);
						index_lights.store((index_lights.load() < units_lights_.size()) ? index_lights.load() : 0U);
						return units_lights_[index_lights.load()].get();
					}
					default: break;
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return std::ref(munit_empty__);
		}
		#pragma endregion

		void JoistickControl::clear() {
			units_mix_.clear();
			units_mqtt_.clear();
			units_lights_.clear();
			index_mix.store(0U);
			index_mqtt.store(0U);
			index_lights.store(0U);
			index_.store(JoystickControlsType::CTRL_NONE);
		}
		void JoistickControl::update(const JoystickConfig& cnf) {
			config_ = cnf;
		}
		void JoistickControl::update(std::shared_ptr<JSON::MMTConfig>& mmt) {
			try {
				clear();
				if (mmt->empty()) return;

				config_ = std::ref(mmt->gamepadconf);
				index_.store(mmt->gamepadconf.type);

				if (mmt->gamepadconf.type == JoystickControlsType::CTRL_NONE) {
					clear();
					return;
				}

				switch (index_.load()) {
					using enum JoystickControlsType;
					case CTRL_NONE: break;
					case CTRL_MIX: {
						update_mix_(mmt->units);
						break;
					}
					case CTRL_MQTT: {
						update_mqtt_(mmt->units);
						break;
					}
					case CTRL_LIGHTS: {
						update_lights_(mmt->units);
						break;
					}
					case CTRL_MIX_AND_MQTT: {
						index_ = JoystickControlsType::CTRL_MIX;
						update_mix_(mmt->units);
						update_mqtt_(mmt->units);
						break;
					}
					case CTRL_LIGHTS_AND_MQTT: {
						index_ = JoystickControlsType::CTRL_MQTT;
						update_mqtt_(mmt->units);
						update_lights_(mmt->units);
						break;
					}
					case CTRL_LIGHTS_AND_MQTT_AND_MIX: {
						index_ = JoystickControlsType::CTRL_MIX;
						update_mix_(mmt->units);
						update_mqtt_(mmt->units);
						update_lights_(mmt->units);
						break;
					}
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}

		const bool JoistickControl::change(MIDI::Mackie::MIDIDATA& m) {
			try {
				if (config_.type == JoystickControlsType::CTRL_NONE) {
					clear();
					return false;
				}
				switch (m.key()) {
					case 22:
					case 24:
					case 42:
					case 44: {
						MIDI::MidiUnit& u = get_();
						if (u.empty()) break;
						if ((u.type == MIDI::MidiUnitType::BTN) || (u.type == MIDI::MidiUnitType::BTNTOGGLE)) {
							m.clear();
							break;
						}
						m.data[0] = u.scene;
						m.data[1] = u.key;
						
						u.value.value = calc_slider_value__(m.data[3], u.value.value, config_.step);
						m.data[2] = u.value.value;
						m.data[3] = 0U;
						return true;
					}
					case 21:
					case 23:
					case 41:
					case 43: {
						set_control_(m.data[3] > 0);
						m.clear();
						break;
					}
					case (JOY_BUTTON1 + JOY_GROUP_1):
					case (JOY_BUTTON1 + JOY_GROUP_2): {
						if (m.data[3] > 0)
							set_scene_(false);
						m.clear();
						break;
					}
					case (JOY_BUTTON2 + JOY_GROUP_1):
					case (JOY_BUTTON2 + JOY_GROUP_2): {
						if (m.data[3] > 0)
							set_scene_(true);
						m.clear();
						break;
					}
					case (JOY_BUTTON3 + JOY_GROUP_1):
					case (JOY_BUTTON3 + JOY_GROUP_2): {
						if (!m.data[3]) break;

						MIDI::MidiUnit& u = get_();
						if (u.empty()) break;
						if ((u.type == MIDI::MidiUnitType::BTN) || (u.type == MIDI::MidiUnitType::BTNTOGGLE)) {
							m.clear();
							break;
						}
						m.data[0] = u.scene;
						m.data[1] = u.key;
						m.data[2] = (m.data[3] > 0) ? 64U : u.value.value;
						m.data[3] = 0U;
						return true;
					}
					case (JOY_BUTTON4 + JOY_GROUP_1):
					case (JOY_BUTTON4 + JOY_GROUP_2): {
						if (!m.data[3]) break;

						MIDI::MidiUnit& u = get_();
						if (u.empty()) break;
						if ((u.type == MIDI::MidiUnitType::BTN) || (u.type == MIDI::MidiUnitType::BTNTOGGLE)) {
							m.clear();
							break;
						}
						m.data[0] = u.scene;
						m.data[1] = u.key;

						u.value.value = (m.data[3] > 0) ? 127U : 0U;
						m.data[2] = u.value.value;
						m.data[3] = 0U;
						return true;
					}
					default: break;
				}

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
		#pragma endregion
	}
}
