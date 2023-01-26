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

        class midi_utils
        {
        public:
            static UINT     Run_midiInGetNumDevs();
            static UINT     Run_midiOutGetNumDevs();
            static MMRESULT Run_midiInOpen(LPHMIDIIN, UINT, DWORD_PTR, DWORD_PTR);
            static MMRESULT Run_midiOutOpen(LPHMIDIOUT, UINT, DWORD_PTR, DWORD_PTR);

            static MMRESULT Run_midiInStart(HMIDIIN);
            static MMRESULT Run_midiInStop(HMIDIIN);
            static MMRESULT Run_midiInReset(HMIDIIN);
            static MMRESULT Run_midiInClose(HMIDIIN);

            static MMRESULT Run_midiOutReset(HMIDIOUT);
            static MMRESULT Run_midiOutClose(HMIDIOUT);

            static MMRESULT Run_midiInGetDevCapsW(UINT_PTR, LPMIDIINCAPSW, UINT);
            static MMRESULT Run_midiOutGetDevCapsW(UINT_PTR, LPMIDIOUTCAPSW, UINT);
            static MMRESULT Run_midiOutShortMsg(HMIDIOUT, DWORD);
            static LPCWSTR  Run_virtualMIDIGetDriverVersion();
            static bool     Run_virtualMIDISendData(LPVM_MIDI_PORT, LPBYTE, DWORD);
            static void     Run_virtualMIDIClosePort(LPVM_MIDI_PORT);
            static LPVM_MIDI_PORT Run_virtualMIDICreatePortEx2(LPCWSTR, LPVM_MIDI_DATA_CB, DWORD_PTR);

            static bool     Check_MMRESULT(std::function<MMRESULT()>, std::wstring);
        };
    }
}
