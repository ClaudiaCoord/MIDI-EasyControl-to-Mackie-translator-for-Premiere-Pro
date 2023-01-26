/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIXER {

		uint32_t getId(uint8_t& s, uint8_t& k) {
			return (s * 1000) + k;
		}

		AudioSessionItemMidi::AudioSessionItemMidi() {
		}

		void AudioSessionItemMidi::Set(Common::MIDI::MixerUnit& m) {
			set_(m.type, m.target, m.longtarget, m.id);
		}
		void AudioSessionItemMidi::Set(Common::MIDI::MidiUnit& m) {
			set_(m.type, m.target, m.longtarget, getId(m.scene, m.key));
		}
		void AudioSessionItemMidi::Set(uint32_t ids[]) {
			Id[0] = ids[0];
			Id[1] = ids[1];
			Id[2] = ids[2];
		}
		void AudioSessionItemMidi::set_(Common::MIDI::MidiUnitType type, Common::MIDI::Mackie::Target target, Common::MIDI::Mackie::Target longtarget, uint32_t key) {
			try {
				if (target != Common::MIDI::Mackie::Target::VOLUMEMIX)
					return;

				switch (type) {
					case Common::MIDI::MidiUnitType::BTN:
					case Common::MIDI::MidiUnitType::BTNTOGGLE: {
						switch (longtarget) {
							case Common::MIDI::Mackie::Target::MAM:
							case Common::MIDI::Mackie::Target::MAS:
							case Common::MIDI::Mackie::Target::B11:
							case Common::MIDI::Mackie::Target::B12:
							case Common::MIDI::Mackie::Target::B13:
							case Common::MIDI::Mackie::Target::B14:
							case Common::MIDI::Mackie::Target::B15:
							case Common::MIDI::Mackie::Target::B16:
							case Common::MIDI::Mackie::Target::B17:
							case Common::MIDI::Mackie::Target::B18:
							case Common::MIDI::Mackie::Target::B19:
							case Common::MIDI::Mackie::Target::B21:
							case Common::MIDI::Mackie::Target::B22:
							case Common::MIDI::Mackie::Target::B23:
							case Common::MIDI::Mackie::Target::B24:
							case Common::MIDI::Mackie::Target::B25:
							case Common::MIDI::Mackie::Target::B26:
							case Common::MIDI::Mackie::Target::B27:
							case Common::MIDI::Mackie::Target::B28:
							case Common::MIDI::Mackie::Target::B29:
							case Common::MIDI::Mackie::Target::B31:
							case Common::MIDI::Mackie::Target::B32:
							case Common::MIDI::Mackie::Target::B33:
							case Common::MIDI::Mackie::Target::B34:
							case Common::MIDI::Mackie::Target::B35:
							case Common::MIDI::Mackie::Target::B36:
							case Common::MIDI::Mackie::Target::B37:
							case Common::MIDI::Mackie::Target::B38:
							case Common::MIDI::Mackie::Target::B39: {
								Id[2] = key;
								break;
							}
							default: break;
						}
						break;
					}
					case Common::MIDI::MidiUnitType::KNOB:
					case Common::MIDI::MidiUnitType::FADER:
					case Common::MIDI::MidiUnitType::SLIDER:
					case Common::MIDI::MidiUnitType::KNOBINVERT:
					case Common::MIDI::MidiUnitType::FADERINVERT:
					case Common::MIDI::MidiUnitType::SLIDERINVERT: {
						switch (longtarget) {
							case Common::MIDI::Mackie::Target::JOG:
							case Common::MIDI::Mackie::Target::MAV:
							case Common::MIDI::Mackie::Target::AV1:
							case Common::MIDI::Mackie::Target::AV2:
							case Common::MIDI::Mackie::Target::AV3:
							case Common::MIDI::Mackie::Target::AV4:
							case Common::MIDI::Mackie::Target::AV5:
							case Common::MIDI::Mackie::Target::AV6:
							case Common::MIDI::Mackie::Target::AV7:
							case Common::MIDI::Mackie::Target::AV8:
							case Common::MIDI::Mackie::Target::XV9:
							case Common::MIDI::Mackie::Target::MAP: {
								Id[0] = key;
								break;
							}
							case Common::MIDI::Mackie::Target::AP1:
							case Common::MIDI::Mackie::Target::AP2:
							case Common::MIDI::Mackie::Target::AP3:
							case Common::MIDI::Mackie::Target::AP4:
							case Common::MIDI::Mackie::Target::AP5:
							case Common::MIDI::Mackie::Target::AP6:
							case Common::MIDI::Mackie::Target::AP7:
							case Common::MIDI::Mackie::Target::AP8:
							case Common::MIDI::Mackie::Target::XP9: {
								if (Id[0] == 0U)
									Id[0] = key;
								else
									Id[1] = key;
								break;
							}
							default: break;
						}
						break;
					}
					default: break;
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}
		uint32_t* AudioSessionItemMidi::Get() {
			return Id;
		}
		AudioAction AudioSessionItemMidi::Get(uint32_t key) {
			if (Id[0] == key) return AudioAction::AUDIO_VOLUME;
			if (Id[1] == key) return AudioAction::AUDIO_VOLUME; /* AudioAction::AUDIO_PANORAMA */
			if (Id[2] == key) return AudioAction::AUDIO_MUTE;
			return AudioAction::AUDIO_NONE;
		}
		uint32_t AudioSessionItemMidi::Get(AudioAction act) {
			switch (act) {
				case AudioAction::AUDIO_VOLUME:
				case AudioAction::AUDIO_VOLUME_INCREMENT: return Id[0];
				case AudioAction::AUDIO_PANORAMA: return Id[1];
				case AudioAction::AUDIO_MUTE: return Id[2];
				case AudioAction::AUDIO_NONE:
				default: return 0U;
			}
		}
		const bool AudioSessionItemMidi::IsKeys() {
			return (Id[0] > 0) || (Id[1] > 0) || (Id[2] > 0);
		}
		const bool AudioSessionItemMidi::IsFound(uint32_t key) {
			return (Id[0] == key) || (Id[1] == key) || (Id[2] == key);
		}

		void AudioSessionItemMidi::Clear() {
			Id[0] = Id[1] = Id[2] = 0U;
		}

		void AudioSessionItemMidi::Copy(Common::MIXER::AudioSessionItem& a) {
			Copy(a.MidiId);
		}
		void AudioSessionItemMidi::Copy(Common::MIXER::AudioSessionItemMidi& a) {
			Set(a.Get());
		}

		const std::wstring AudioSessionItemMidi::ToString() {
			std::wstringstream s;
			if (Id[0] != 0U)
				s << L"Volume:[" << Id[0] << L"]";
			if (Id[1] != 0U)
				s << L", Pan:[" << Id[1] << L"]";
			if (Id[2] != 0U)
				s << L", Mute:[" << Id[2] << L"]";
			return s.str();
		}
	}
}
