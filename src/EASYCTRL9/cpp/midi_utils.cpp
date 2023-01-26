/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	EASYCTRL9

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
    namespace MIDI {
		UINT     midi_utils::Run_midiInGetNumDevs() {
			__try {
				return ::midiInGetNumDevs();
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		UINT     midi_utils::Run_midiOutGetNumDevs() {
			__try {
				return ::midiOutGetNumDevs();
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT midi_utils::Run_midiInStart(HMIDIIN h) {
			__try {
				return ::midiInStart(h);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT midi_utils::Run_midiInStop(HMIDIIN h) {
			__try {
				return ::midiInStop(h);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT midi_utils::Run_midiInReset(HMIDIIN h) {
			__try {
				return ::midiInReset(h);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT midi_utils::Run_midiInClose(HMIDIIN h) {
			__try {
				return ::midiInClose(h);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT midi_utils::Run_midiOutReset(HMIDIOUT h) {
			__try {
				return ::midiOutReset(h);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT midi_utils::Run_midiOutClose(HMIDIOUT h) {
			__try {
				return ::midiOutClose(h);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT midi_utils::Run_midiInOpen(LPHMIDIIN h, UINT u, DWORD_PTR p1, DWORD_PTR p2) {
			__try {
				return ::midiInOpen(h, u, p1, p2, CALLBACK_FUNCTION);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT midi_utils::Run_midiOutOpen(LPHMIDIOUT h, UINT u, DWORD_PTR p1, DWORD_PTR p2) {
			__try {
				return ::midiOutOpen(h, u, p1, p2, CALLBACK_FUNCTION);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT midi_utils::Run_midiInGetDevCapsW(UINT_PTR p, LPMIDIINCAPSW c, UINT u) {
			__try {
				return ::midiInGetDevCapsW(p, c, u);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT midi_utils::Run_midiOutGetDevCapsW(UINT_PTR p, LPMIDIOUTCAPSW c, UINT u) {
			__try {
				return ::midiOutGetDevCapsW(p, c, u);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT midi_utils::Run_midiOutShortMsg(HMIDIOUT h, DWORD d) {
			__try {
				return ::midiOutShortMsg(h, d);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		LPCWSTR  midi_utils::Run_virtualMIDIGetDriverVersion() {
			__try {
				return ::virtualMIDIGetDriverVersion(nullptr, nullptr, nullptr, nullptr);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
		}
		bool     midi_utils::Run_virtualMIDISendData(LPVM_MIDI_PORT p, LPBYTE b, DWORD z) {
			__try {
				return ::virtualMIDISendData(p, b, z);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
		}
		void     midi_utils::Run_virtualMIDIClosePort(LPVM_MIDI_PORT p) {
			__try {
				return ::virtualMIDIClosePort(p);
			} __except (EXCEPTION_EXECUTE_HANDLER) {}
		}
		LPVM_MIDI_PORT midi_utils::Run_virtualMIDICreatePortEx2(LPCWSTR s, LPVM_MIDI_DATA_CB c, DWORD_PTR p) {
			__try {
				return ::virtualMIDICreatePortEx2(
					s, c, p,
					TE_VM_DEFAULT_BUFFER_SIZE,
					(TE_VM_FLAGS_PARSE_TX | TE_VM_FLAGS_INSTANTIATE_BOTH)
				);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
		}

		bool midi_utils::Check_MMRESULT(std::function<MMRESULT()> f, std::wstring tag) {
			_set_se_translator(seh_exception_catch);
			try {
				MMRESULT m = f();
				if (m == S_OK)
					return true;
				if (m == (UINT)-1)
					Common::to_log::Get() << (log_string() << tag << Utils::DefaulSehErrorFound());
				else
					Common::to_log::Get() << (log_string() << tag << Utils::MMRESULT_to_string(m));
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
	}
}
