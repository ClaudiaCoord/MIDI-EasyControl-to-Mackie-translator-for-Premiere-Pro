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

        enum class ClassTypes : int {
            ClassNone = 0,
            ClassProxy,
            ClassMixer,
            ClassMonitor,
            ClassMediaKey,
            ClassInMidi,
            ClassOutMidi,
            ClassOutMidiMackie,
            ClassVirtualMidi
        };

        enum FLAG_EXPORT MidiUnitType : uint8_t {
            FADER,
            SLIDER,
            KNOB,
            BTN,
            BTNTOGGLE,
            KNOBINVERT,
            FADERINVERT,
            SLIDERINVERT,
            UNITNONE = 255
        };

        enum FLAG_EXPORT MidiUnitScene : uint8_t {
            SC1 = 176,
            SC2 = 177,
            SC3 = 178,
            SC4 = 179,
            SC1KNOB = 192,
            SC2KNOB = 193,
            SC3KNOB = 194,
            SC4KNOB = 211,
            SC4BTN = 243,
            SCNONE = 255
        };

    }
}
