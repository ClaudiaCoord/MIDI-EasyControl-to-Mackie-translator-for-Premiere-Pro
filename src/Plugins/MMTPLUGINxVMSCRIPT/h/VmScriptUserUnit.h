/*
	MIDI-MT is a high level application driver for USB MIDI control surface.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#pragma once

namespace Common {
	namespace SCRIPT {

		using namespace std::string_view_literals;

		class UnitDef {
		private:
			uint8_t c_[3]{ 255U };
			MIDI::MidiUnitType type_{ MIDI::MidiUnitType::UNITNONE };

		public:

			UnitDef();
			UnitDef(const UnitDef&) = default;
			UnitDef(const std::vector<uint8_t>&, const uint8_t);
			UnitDef(const uint8_t, const uint8_t, const uint8_t);
			UnitDef(const uint8_t, const uint8_t, const uint8_t, const MIDI::MidiUnitType);
			~UnitDef() = default;

			MIDI::MidiUnitType GetType() const;
			uint8_t GetScene() const;
			uint8_t GetKey() const;
			uint8_t GetValue() const;
			void SetValue(const uint8_t);

			const bool empty() const;
			std::wstring dump() const;
			std::string dump_s() const;
		};
	}
}