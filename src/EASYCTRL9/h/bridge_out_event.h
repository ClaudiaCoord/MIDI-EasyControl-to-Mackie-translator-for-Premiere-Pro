/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	EASYCTRL9

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace MIDI {

        class bridge_out_event
        {
        private:
            std::shared_ptr<MidiDevice>& cnf__;
            Common::common_event<MidiInstance> event__;

        public:

            bridge_out_event() = delete;
            bridge_out_event(std::shared_ptr<MidiDevice>&);
            void SetConfig(std::shared_ptr<MidiDevice>&);

            void CallCbOut(Mackie::MIDIDATA&, DWORD&);

            void clear();
            void add(MidiInstance&);
            void add(MidiControllerBase&);
            void remove(uint32_t);
            void remove(MidiInstance&);
            void remove(MidiControllerBase&);
        };
    }
}

