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

		static constexpr uint8_t JOY_GROUP_1 = 10U;
		static constexpr uint8_t JOY_GROUP_2 = 30U;

		class JoistickContainer {
		private:
			MIDI::MidiUnit& unit_;
		public:

			JoistickContainer(MIDI::MidiUnit&);
			~JoistickContainer() = default;

			MIDI::MidiUnit& get() const;
		};

		class JoistickData {
		public:
			JOYCAPSW caps{};
			MMRESULT state{ 0 };
			bool is_offline{ true };
			bool is_error{ false };
			const uint16_t id;

			JoistickData(uint16_t);
			~JoistickData() = default;

			const bool enable() const;

			const std::wstring name() const;
			const std::wstring oem() const;
			const std::wstring label() const;
		};
	}
}
