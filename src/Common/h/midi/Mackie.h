/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	COMMON::MIDI

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIDI {

		class FLAG_EXPORT MidiUnit;

		class FLAG_EXPORT Mackie
		{
		public:
			union FLAG_EXPORT MIDIDATA {
				uint8_t data[4];
				DWORD send;
				void Set(uint8_t type, uint8_t id, uint8_t val, uint8_t ext = 0x0);
				void SetValue(bool b);
				uint8_t scene();
				uint8_t key();
				uint8_t value();
				std::wstring Dump();
				std::wstring UiDump();
			};

			enum class Types : uint8_t {
				NoteOn = 0x90,
				NoteOff = 0x80,
				Aftertouch = 0xA0,
				ControlChange = 0xB0,
				ProgramChange = 0xC0,
				AftertouchChannel = 0xD0,
				PitchBend = 0xE0,
				SystemExclusive = 0xF0,
				SystemReset = 0xFF,
				InvalidType = 0x0
			};

			enum Button : uint8_t {
				Arm = 0,
				Solo = 8,
				Mute = 16,
				Select = 24
			};

			enum Function : uint8_t {
				F1 = 54,
				F2 = 55,
				F3 = 56,
				F4 = 57,
				F5 = 58,
				F6 = 59,
				F7 = 60,
				F8 = 61
			};

			enum Control : uint8_t {
				Rewind = 91,
				Forward = 92,
				Stop = 93,
				Play = 94,
				Record = 95,
				Up = 96,
				Down = 97,
				Left = 98,
				Right = 99,
				Zoom = 100,
				Scrub = 101,
				JogWheel = 60,
				PanFade = 16
			};

			enum Target : uint8_t {
				MAV, MAP, MAM, MAS, JOG,
				AV1, AV2, AV3, AV4, AV5, AV6, AV7, AV8, XV9, // 1-9 Volume
				AP1, AP2, AP3, AP4, AP5, AP6, AP7, AP8, XP9, // 1-9 Pan
				B11, B12, B13, B14, B15, B16, B17, B18, B19, // 1-9 Solo
				B21, B22, B23, B24, B25, B26, B27, B28, B29, // 1-9 Mute
				B31, B32, B33, B34, B35, B36, B37, B38, B39, // 1-9 Select
				FUN11, FUN12, FUN13, FUN14, FUN15, FUN16, FUN17, FUN18, // Standart touch function 1-8
				FUN21, FUN22, FUN23, FUN24, FUN25, FUN26, FUN27, FUN28, // One touch function 1-8
				SYS_Rewind,
				SYS_Forward,
				SYS_Stop,
				SYS_Play,
				SYS_Record,
				SYS_Up,
				SYS_Down,
				SYS_Left,
				SYS_Right,
				SYS_Zoom,
				SYS_Scrub,
				LIGHTKEY16B = 250,
				LIGHTKEY8B = 251,
				MQTTKEY = 252,
				MEDIAKEY = 253,
				VOLUMEMIX = 254,
				NOTARGET = 255
			};

			enum ClickType : uint8_t {
				ClickOnce,
				ClickLong,
				ClickTrigger,
				ClickSlider,
				ClickUnknown = 255
			};

			static bool SelectorTarget(MidiUnit& unit, MIDIDATA& m, ClassTypes t);
			static void SetFunctionOnce(Function id, MIDIDATA& m);
			static void SetFunction(Function id, bool status, MIDIDATA& m);
			static void SetVolume(int value, MIDIDATA& m);
			static void SetVolume(int number, int value, MIDIDATA& m);
			static void SetPan(bool value, MIDIDATA& m);
			static void SetPan(int number, bool value, MIDIDATA& m);
			static void SetWheel(bool value, MIDIDATA& m);

			template<typename T>
			static void SetButton(T id, bool status, MIDIDATA& m) {
				SetButton(id, 0, status, m);
			}
			template<typename T>
			static void SetButton(T id, int number, MIDIDATA& m) {
				SetButton(id, number, true, m);
			}
			template<typename T>
			static void SetButton(T id, int number, bool status, MIDIDATA& m) {
				m.Set(
					status ? static_cast<uint8_t>(Types::NoteOn) : static_cast<uint8_t>(Types::NoteOff),
					static_cast<uint8_t>(id) | static_cast<uint8_t>(number),
					status ? 0x7f : 0x40
				);
			}
		};
	}
}
