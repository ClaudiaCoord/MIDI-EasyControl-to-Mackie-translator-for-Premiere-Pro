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

        MidiInstance::MidiInstance() :
            id__(0U), type__(ClassTypes::ClassNone), 
            out1__([](Common::MIDI::Mackie::MIDIDATA&, DWORD&) { return false; }),
            out2__([](Common::MIDI::MidiUnit&, DWORD&) { return false; }),
            log__([](const std::wstring&) {}),
            in__([](DWORD, DWORD) {}) {
        }
        const uint32_t MidiInstance::GetId() const {
            return id__;
        }
        ClassTypes MidiInstance::GetType() {
            return type__;
        }
        callMidiOut1Cb MidiInstance::GetCbOut1() {
            return out1__;
        }
        callMidiOut2Cb MidiInstance::GetCbOut2() {
            return out2__;
        }
        logFnType      MidiInstance::GetCbLog() {
            return log__;
        }

        void MidiInstance::InCallbackSet(callMidiInCb f) {
            in__ = f;
        }
        void MidiInstance::InCallbackRemove() {
            in__ = [](DWORD, DWORD) {};
        }
        void MidiInstance::InCallbackCall(DWORD d, DWORD t) {
            in__(d, t);
        }
    }
}
