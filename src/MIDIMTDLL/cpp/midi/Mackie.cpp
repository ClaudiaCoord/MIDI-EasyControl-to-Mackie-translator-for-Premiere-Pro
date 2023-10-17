/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIDI {

		bool Mackie::SelectorTarget(MidiUnit& unit, MIDIDATA& m, ClassTypes t) {

			switch (t) {
				case ClassTypes::ClassProxy:
				case ClassTypes::ClassMonitor:
				case ClassTypes::ClassOutMidi:
				case ClassTypes::ClassVirtualMidi:   return true;
				case ClassTypes::ClassMixer:         return (unit.target & Target::VOLUMEMIX);
				case ClassTypes::ClassMediaKey:      return (unit.target & Target::MEDIAKEY);
				case ClassTypes::ClassMqttKey:       return (unit.target & Target::MQTTKEY);
				case ClassTypes::ClassLightKey:      return ((unit.target & Target::LIGHTKEY8B) || (unit.target & Target::LIGHTKEY16B));
				case ClassTypes::ClassOutMidiMackie: break;
				case ClassTypes::ClassNone:
				case ClassTypes::ClassInMidi:
				default: return false;
			}
			switch (unit.target) {
				case Target::NOTARGET: {
					if (unit.longtarget & Target::NOTARGET)
						return false;
					break;
				}
				case Target::LIGHTKEY8B:
				case Target::LIGHTKEY16B:
				case Target::MEDIAKEY:
				case Target::VOLUMEMIX: return false;
				default: break;
			}
			switch (unit.type) {
				case BTN:
				case BTNTOGGLE: {
					switch (unit.target) {
						case Target::MAM:
						case Target::MAS:
						case Target::B11:
						case Target::B12:
						case Target::B13:
						case Target::B14:
						case Target::B15:
						case Target::B16:
						case Target::B17:
						case Target::B18:
						case Target::B21:
						case Target::B22:
						case Target::B23:
						case Target::B24:
						case Target::B25:
						case Target::B26:
						case Target::B27:
						case Target::B28:
						case Target::B31:
						case Target::B32:
						case Target::B33:
						case Target::B34:
						case Target::B35:
						case Target::B36:
						case Target::B37:
						case Target::B38: {
							int num;
							Button btn;
							switch (unit.value.type & ClickType::ClickLong ? unit.longtarget : unit.target)
							{
								case Target::B21: btn = Button::Mute; num = 0; break;
								case Target::B11: btn = Button::Solo; num = 0; break;
								case Target::B31: btn = Button::Select; num = 0; break;
								case Target::B22: btn = Button::Mute; num = 1; break;
								case Target::B12: btn = Button::Solo; num = 1; break;
								case Target::B32: btn = Button::Select; num = 1; break;
								case Target::B23: btn = Button::Mute; num = 2; break;
								case Target::B13: btn = Button::Solo; num = 2; break;
								case Target::B33: btn = Button::Select; num = 2; break;
								case Target::B24: btn = Button::Mute; num = 3; break;
								case Target::B14: btn = Button::Solo; num = 3; break;
								case Target::B34: btn = Button::Select; num = 3; break;
								case Target::B25: btn = Button::Mute; num = 4; break;
								case Target::B15: btn = Button::Solo; num = 4; break;
								case Target::B35: btn = Button::Select; num = 4; break;
								case Target::B26: btn = Button::Mute; num = 5; break;
								case Target::B16: btn = Button::Solo; num = 5; break;
								case Target::B36: btn = Button::Select; num = 5; break;
								case Target::B27: btn = Button::Mute; num = 6; break;
								case Target::B17: btn = Button::Solo; num = 6; break;
								case Target::B37: btn = Button::Select; num = 6; break;
								case Target::B28: btn = Button::Mute; num = 7; break;
								case Target::B18: btn = Button::Solo; num = 7; break;
								case Target::B38: btn = Button::Select; num = 7; break;
								case Target::MAM: btn = Button::Mute; num = 8; break;
								case Target::MAS: btn = Button::Select; num = 8; break;
								default: return false;
							}
							Mackie::SetButton(btn, num, unit.value.lvalue, m);
							break;
						}
						case Target::FUN11:
						case Target::FUN12:
						case Target::FUN13:
						case Target::FUN14:
						case Target::FUN15:
						case Target::FUN16:
						case Target::FUN17:
						case Target::FUN18: {
							Function fn;
							switch (unit.value.type & ClickType::ClickLong ? unit.longtarget : unit.target)
							{
								case Target::FUN11: fn = Function::F1; break;
								case Target::FUN12: fn = Function::F2; break;
								case Target::FUN13: fn = Function::F3; break;
								case Target::FUN14: fn = Function::F4; break;
								case Target::FUN15: fn = Function::F5; break;
								case Target::FUN16: fn = Function::F6; break;
								case Target::FUN17: fn = Function::F7; break;
								case Target::FUN18: fn = Function::F8; break;
								default: return false;
							}
							Mackie::SetFunction(fn, unit.value.lvalue, m);
							break;
						}
						case Target::FUN21:
						case Target::FUN22:
						case Target::FUN23:
						case Target::FUN24:
						case Target::FUN25:
						case Target::FUN26:
						case Target::FUN27:
						case Target::FUN28: {
							Function fn;
							switch (unit.value.type & ClickType::ClickLong ? unit.longtarget : unit.target)
							{
								case Target::FUN21: fn = Function::F1; break;
								case Target::FUN22: fn = Function::F2; break;
								case Target::FUN23: fn = Function::F3; break;
								case Target::FUN24: fn = Function::F4; break;
								case Target::FUN25: fn = Function::F5; break;
								case Target::FUN26: fn = Function::F6; break;
								case Target::FUN27: fn = Function::F7; break;
								case Target::FUN28: fn = Function::F8; break;
								default: return false;
							}
							Mackie::SetFunctionOnce(fn, m);
							break;
						}
						case Target::SYS_Rewind:
						case Target::SYS_Forward:
						case Target::SYS_Stop:
						case Target::SYS_Play:
						case Target::SYS_Record:
						case Target::SYS_Up:
						case Target::SYS_Down:
						case Target::SYS_Left:
						case Target::SYS_Right:
						case Target::SYS_Zoom:
						case Target::SYS_Scrub: {
							Control ctrl;
							switch (unit.value.type & ClickLong ? unit.longtarget : unit.target)
							{
								case Target::SYS_Rewind: ctrl = Control::Rewind; break;
								case Target::SYS_Forward: ctrl = Control::Forward; break;
								case Target::SYS_Stop: ctrl = Control::Stop; break;
								case Target::SYS_Play: ctrl = Control::Play; break;
								case Target::SYS_Record: ctrl = Control::Record; break;
								case Target::SYS_Up: ctrl = Control::Up; break;
								case Target::SYS_Down: ctrl = Control::Down; break;
								case Target::SYS_Left: ctrl = Control::Left; break;
								case Target::SYS_Right: ctrl = Control::Right; break;
								case Target::SYS_Zoom: ctrl = Control::Zoom; break;
								case Target::SYS_Scrub: ctrl = Control::Scrub; break;
								default: return false;
							}
							Mackie::SetButton(ctrl, unit.value.lvalue, m);
							break;
						}
						default: return false;
					}
					break;
				}
				case FADER:
				case SLIDER: {
					switch (unit.target) {
					case Target::MAV:
					case Target::AV1:
					case Target::AV2:
					case Target::AV3:
					case Target::AV4:
					case Target::AV5:
					case Target::AV6:
					case Target::AV7:
					case Target::AV8: {
						int num;
						switch (unit.target)
						{
							case Target::AV1: num = 0; break;
							case Target::AV2: num = 1; break;
							case Target::AV3: num = 2; break;
							case Target::AV4: num = 3; break;
							case Target::AV5: num = 4; break;
							case Target::AV6: num = 5; break;
							case Target::AV7: num = 6; break;
							case Target::AV8: num = 7; break;
							case Target::MAV: num = 8; break;
							default: return false;
						}
						Mackie::SetVolume(num, unit.value.value, m);
						break;
					}
					case Target::MAP:
					case Target::AP1:
					case Target::AP2:
					case Target::AP3:
					case Target::AP4:
					case Target::AP5:
					case Target::AP6:
					case Target::AP7:
					case Target::AP8: {
						int num;
						switch (unit.target)
						{
							case Target::AP1: num = 0; break;
							case Target::AP2: num = 1; break;
							case Target::AP3: num = 2; break;
							case Target::AP4: num = 3; break;
							case Target::AP5: num = 4; break;
							case Target::AP6: num = 5; break;
							case Target::AP7: num = 6; break;
							case Target::AP8: num = 7; break;
							case Target::MAP: num = 8; break;
							default: return false;
						}
						Mackie::SetPan(num, unit.value.lvalue, m);
						break;
					}
					default: return false;
					}
					break;
				}
				case KNOB: {
					if (unit.target != Target::JOG)
						return false;
					Mackie::SetWheel(unit.value.lvalue, m);
					break;
				}
				case Target::NOTARGET:
				default: return false;
			}
			return true;
		}
		void Mackie::SetFunctionOnce(Function id, MIDIDATA& m) {
			SetFunction(id, true, m);
		}
		void Mackie::SetFunction(Function id, bool status, MIDIDATA& m) {
			m.Set(
				status ? static_cast<uint8_t>(Types::NoteOn) : static_cast<uint8_t>(Types::NoteOff),
				static_cast<uint8_t>(id),
				status ? 0x7f : 0x40
			);
		}
		void Mackie::SetVolume(int value, MIDIDATA& m) {
			SetVolume(8, value, m);
		}
		void Mackie::SetVolume(int number, int value, MIDIDATA& m) {
			m.Set(
				static_cast<uint8_t>(Types::PitchBend) | static_cast<uint8_t>(number),
				0x0,
				static_cast<uint8_t>(value)
			);
		}
		void Mackie::SetPan(bool value, MIDIDATA& m) {
			SetPan(24, value, m);
		}
		void Mackie::SetPan(int number, bool value, MIDIDATA& m) {
			m.Set(
				static_cast<uint8_t>(Types::ControlChange),
				static_cast<uint8_t>(Control::PanFade) + static_cast<uint8_t>(number),
				value ? 0x41 : 0x01
			);
		}
		void Mackie::SetWheel(bool value, MIDIDATA& m) {
			m.Set(
				static_cast<uint8_t>(Types::ControlChange),
				static_cast<uint8_t>(Control::JogWheel),
				value ? 0x41 : 0x01
			);
		}

		void Mackie::MIDIDATA::Set(uint8_t type, uint8_t id, uint8_t val, uint8_t ext) {
			data[0] = type;
			data[1] = id;
			data[2] = val;
			data[3] = ext;
		}
		void Mackie::MIDIDATA::SetValue(bool b) {
			data[2] = b ? 0x7f : 0x40;
		}
		uint8_t Mackie::MIDIDATA::scene() {
			return data[0];
		}
		uint8_t Mackie::MIDIDATA::key() {
			return data[1];
		}
		uint8_t Mackie::MIDIDATA::value() {
			return data[2];
		}
		std::wstring Mackie::MIDIDATA::Dump() {
			return (log_string() << L"\ttype:" << data[0] << L", id:" << data[1] << L", value:" << data[2]);
		}
		std::wstring Mackie::MIDIDATA::UiDump() {
			return (log_string() << L"Scene: " << MidiHelper::GetScene(data[0]) << L"/" << data[0] << L",\tKey:" << data[1] << L",\tValue:" << data[2]);
		}
	}
}