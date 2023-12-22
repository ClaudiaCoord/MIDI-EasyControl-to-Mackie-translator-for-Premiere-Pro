/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023, MIT

	MIDIMMT DLL + MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace IO {

		typedef std::function<void(std::wstring&)>					callToLog_t;
		typedef std::function<void(log_string&)>					callToLogS_t;
		typedef std::function<uint32_t(std::wstring&)>				callGetPid_t;
		typedef std::function<void(DWORD, DWORD)>					callIn1Cb_t;
		typedef std::function<void(MIDI::Mackie::MIDIDATA, DWORD)>	callIn2Cb_t;
		typedef std::function<void(MIDI::Mackie::MIDIDATA, DWORD)>	callOut1Cb_t;
		typedef std::function<void(MIDI::MidiUnit&, DWORD)>			callOut2Cb_t;
	}
}

