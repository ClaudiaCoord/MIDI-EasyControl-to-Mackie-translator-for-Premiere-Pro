/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIDI {

		bool Mackie::SelectorTarget(MidiUnit& unit, MIDIDATA& m, IO::PluginClassTypes t) {

			switch (t) {
				using enum IO::PluginClassTypes;
				case ClassIn:
				case ClassOut:
				case ClassOut1:
				case ClassOut2:
				case ClassSys:
				case ClassProxy:
				case ClassRemote:
				case ClassMonitor:
				case ClassOutMidi:
				case ClassVirtualMidi:   return true;
				case ClassMixer:         return (unit.target & Target::VOLUMEMIX);
				case ClassMediaKey:      return (unit.target & Target::MEDIAKEY);
				case ClassMqttKey:       return (unit.target & Target::MQTTKEY);
				case ClassLightKey:      return ((unit.target & Target::LIGHTKEY8B) || (unit.target & Target::LIGHTKEY16B));
				case ClassOutMidiMackie: break;
				case ClassNone:
				case ClassInMidi:
				default: return false;
			}
			switch (unit.target) {
				using enum Target;
				case NOTARGET: {
					if (unit.longtarget & Target::NOTARGET)
						return false;
					break;
				}
				case LIGHTKEY8B:
				case LIGHTKEY16B:
				case MEDIAKEY:
				case VOLUMEMIX: return false;
				default: break;
			}
			switch (unit.type) {
				using enum MidiUnitType;
				case BTN:
				case BTNTOGGLE: {
					switch (unit.target) {
						using enum Target;
						case MAM:
						case MAS:
						case B11:
						case B12:
						case B13:
						case B14:
						case B15:
						case B16:
						case B17:
						case B18:
						case B21:
						case B22:
						case B23:
						case B24:
						case B25:
						case B26:
						case B27:
						case B28:
						case B31:
						case B32:
						case B33:
						case B34:
						case B35:
						case B36:
						case B37:
						case B38: {
							int32_t num;
							Button btn;
							switch (unit.value.type & ClickType::ClickLong ? unit.longtarget : unit.target) {
								using enum Target;
								case B21: btn = Button::Mute; num = 0; break;
								case B11: btn = Button::Solo; num = 0; break;
								case B31: btn = Button::Select; num = 0; break;
								case B22: btn = Button::Mute; num = 1; break;
								case B12: btn = Button::Solo; num = 1; break;
								case B32: btn = Button::Select; num = 1; break;
								case B23: btn = Button::Mute; num = 2; break;
								case B13: btn = Button::Solo; num = 2; break;
								case B33: btn = Button::Select; num = 2; break;
								case B24: btn = Button::Mute; num = 3; break;
								case B14: btn = Button::Solo; num = 3; break;
								case B34: btn = Button::Select; num = 3; break;
								case B25: btn = Button::Mute; num = 4; break;
								case B15: btn = Button::Solo; num = 4; break;
								case B35: btn = Button::Select; num = 4; break;
								case B26: btn = Button::Mute; num = 5; break;
								case B16: btn = Button::Solo; num = 5; break;
								case B36: btn = Button::Select; num = 5; break;
								case B27: btn = Button::Mute; num = 6; break;
								case B17: btn = Button::Solo; num = 6; break;
								case B37: btn = Button::Select; num = 6; break;
								case B28: btn = Button::Mute; num = 7; break;
								case B18: btn = Button::Solo; num = 7; break;
								case B38: btn = Button::Select; num = 7; break;
								case MAM: btn = Button::Mute; num = 8; break;
								case MAS: btn = Button::Select; num = 8; break;
								default: return false;
							}
							Mackie::SetButton(btn, num, unit.value.lvalue, m);
							break;
						}
						case FUN11:
						case FUN12:
						case FUN13:
						case FUN14:
						case FUN15:
						case FUN16:
						case FUN17:
						case FUN18: {
							Function fn;
							switch (unit.value.type & ClickType::ClickLong ? unit.longtarget : unit.target) {
								using enum Target;
								case FUN11: fn = Function::F1; break;
								case FUN12: fn = Function::F2; break;
								case FUN13: fn = Function::F3; break;
								case FUN14: fn = Function::F4; break;
								case FUN15: fn = Function::F5; break;
								case FUN16: fn = Function::F6; break;
								case FUN17: fn = Function::F7; break;
								case FUN18: fn = Function::F8; break;
								default: return false;
							}
							Mackie::SetFunction(fn, unit.value.lvalue, m);
							break;
						}
						case FUN21:
						case FUN22:
						case FUN23:
						case FUN24:
						case FUN25:
						case FUN26:
						case FUN27:
						case FUN28: {
							Function fn;
							switch (unit.value.type & ClickType::ClickLong ? unit.longtarget : unit.target) {
								using enum Target;
								case FUN21: fn = Function::F1; break;
								case FUN22: fn = Function::F2; break;
								case FUN23: fn = Function::F3; break;
								case FUN24: fn = Function::F4; break;
								case FUN25: fn = Function::F5; break;
								case FUN26: fn = Function::F6; break;
								case FUN27: fn = Function::F7; break;
								case FUN28: fn = Function::F8; break;
								default: return false;
							}
							Mackie::SetFunctionOnce(fn, m);
							break;
						}
						case SYS_Rewind:
						case SYS_Forward:
						case SYS_Stop:
						case SYS_Play:
						case SYS_Record:
						case SYS_Up:
						case SYS_Down:
						case SYS_Left:
						case SYS_Right:
						case SYS_Zoom:
						case SYS_Scrub: {
							Control ctrl;
							switch (unit.value.type & ClickLong ? unit.longtarget : unit.target) {
								using enum Target;
								case SYS_Rewind:	ctrl = Control::Rewind; break;
								case SYS_Forward:	ctrl = Control::Forward; break;
								case SYS_Stop:		ctrl = Control::Stop; break;
								case SYS_Play:		ctrl = Control::Play; break;
								case SYS_Record:	ctrl = Control::Record; break;
								case SYS_Up:		ctrl = Control::Up; break;
								case SYS_Down:		ctrl = Control::Down; break;
								case SYS_Left:		ctrl = Control::Left; break;
								case SYS_Right:		ctrl = Control::Right; break;
								case SYS_Zoom:		ctrl = Control::Zoom; break;
								case SYS_Scrub:		ctrl = Control::Scrub; break;
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
						using enum Target;
						case MAV:
						case AV1:
						case AV2:
						case AV3:
						case AV4:
						case AV5:
						case AV6:
						case AV7:
						case AV8: {
							int32_t num;
							switch (unit.target) {
								using enum Target;
								case AV1: num = 0; break;
								case AV2: num = 1; break;
								case AV3: num = 2; break;
								case AV4: num = 3; break;
								case AV5: num = 4; break;
								case AV6: num = 5; break;
								case AV7: num = 6; break;
								case AV8: num = 7; break;
								case MAV: num = 8; break;
								default: return false;
							}
							Mackie::SetVolume(num, unit.value.value, m);
							break;
						}
						case MAP:
						case AP1:
						case AP2:
						case AP3:
						case AP4:
						case AP5:
						case AP6:
						case AP7:
						case AP8: {
							int num;
							switch (unit.target) {
								using enum Target;
								case AP1: num = 0; break;
								case AP2: num = 1; break;
								case AP3: num = 2; break;
								case AP4: num = 3; break;
								case AP5: num = 4; break;
								case AP6: num = 5; break;
								case AP7: num = 6; break;
								case AP8: num = 7; break;
								case MAP: num = 8; break;
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
				case UNITNONE:
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
		uint8_t Mackie::MIDIDATA::scene() const {
			return data[0];
		}
		uint8_t Mackie::MIDIDATA::key() const {
			return data[1];
		}
		uint8_t Mackie::MIDIDATA::value() const {
			return data[2];
		}
		Mackie::Target Mackie::MIDIDATA::target() const {
			return static_cast<Mackie::Target>(data[3]);
		}
		std::wstring Mackie::MIDIDATA::dump() {
			return (log_string() << L"\ttype:" << data[0] << L", id:" << data[1] << L", value:" << data[2]);
		}
		std::wstring Mackie::MIDIDATA::dump_ui() {
			return (log_string() << L"Scene: " << MidiHelper::GetScene(data[0]) << L"/" << data[0] << L",\tKey:" << data[1] << L",\tValue:" << data[2]);
		}
		const bool Mackie::MIDIDATA::empty() const {
			return (data[0] == 255U) || (data[1] == 255U) || (data[3] == 255U);
		}
		const bool Mackie::MIDIDATA::equals(MIDIDATA& m) const {
			return (data[0] == m.data[0]) && (data[1] == m.data[1]) && (data[2] == m.data[2]) && (data[3] == m.data[3]);
		}
	}
}