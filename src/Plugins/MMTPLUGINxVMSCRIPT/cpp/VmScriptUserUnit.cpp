/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace SCRIPT {

		#pragma region UnitDef
		UnitDef::UnitDef() : type_(MIDI::MidiUnitType::UNITNONE) {
		}
		UnitDef::UnitDef(const uint8_t s, const uint8_t k, const uint8_t v, const MIDI::MidiUnitType t) : type_(t) {
			c_[0] = s;
			c_[1] = k;
			c_[2] = v;
		}
		UnitDef::UnitDef(const uint8_t s, const uint8_t k, const uint8_t v) : type_(MIDI::MidiUnitType::SLIDER) {
			c_[0] = s;
			c_[1] = k;
			c_[2] = v;
		}
		UnitDef::UnitDef(const std::vector<uint8_t>& v, const uint8_t val) : type_(MIDI::MidiUnitType::SLIDER) {
			c_[0] = (v.size() > 0) ? v[0] : 255U;
			c_[1] = (v.size() > 1) ? v[1] : 255U;
			c_[2] = val;
		}

		MIDI::MidiUnitType UnitDef::GetType() const {
			return type_;
		}
		uint8_t UnitDef::GetScene() const {
			return c_[0];
		}
		uint8_t UnitDef::GetKey() const {
			return c_[1];
		}
		uint8_t UnitDef::GetValue() const {
			return c_[2];
		}
		void UnitDef::SetValue(const uint8_t c) {
			c_[2] = c;
		}
		const bool UnitDef::empty() const {
			return (c_[0] == 255U) || (c_[1] == 255U);
		}
		std::wstring UnitDef::dump() const {
			return (log_string() << static_cast<uint16_t>(c_[1]) << L"/" << static_cast<uint16_t>(c_[0]) << L"|" << c_[2] << L"|");
		}
		std::string UnitDef::dump_s() const {
			return Utils::from_string(dump());
		}
		#pragma endregion
	}
}