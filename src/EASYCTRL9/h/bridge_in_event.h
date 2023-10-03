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

        class bridge_in_event
        {
        private:
            worker_background& wb__;
            std::atomic<bool> jogscenefilter__;
            std::unique_ptr<bridge_out_event> & outevent__;

            void cbInCall(DWORD, DWORD);
            void cbIn(DWORD&, DWORD&);

        public:

            bridge_in_event() = delete;
            bridge_in_event(std::unique_ptr<bridge_out_event>&);
            ~bridge_in_event();

            const bool IsJogSceneFilter();
            void       IsJogSceneFilter(bool);

            void add(MidiInstance& mi);
            void add(MidiControllerBase&);
            void remove(MidiInstance& mi);
            void remove(MidiControllerBase&);
            void remove(MidiInstance* mi);
            void remove(MidiControllerBase*);
        };
    }
}

