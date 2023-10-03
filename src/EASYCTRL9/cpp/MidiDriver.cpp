/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	+ Smart House controls.
	+ Lighting controls (DMX, Artnet).
	(c) CC 2023, MIT

	EASYCTRL9

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIDI {

		#define TE_VM_LOGGING_MISC	1
		#define TE_VM_LOGGING_RX	2
		#define TE_VM_LOGGING_TX	4
		#define TE_VM_DEFAULT_BUFFER_SIZE	0x1fffe
		#define TE_VM_FLAGS_PARSE_RX			(1)
		#define TE_VM_FLAGS_PARSE_TX			(2)
		#define TE_VM_FLAGS_INSTANTIATE_RX_ONLY		(4)
		#define TE_VM_FLAGS_INSTANTIATE_TX_ONLY		(8)
		#define TE_VM_FLAGS_INSTANTIATE_BOTH		(12)
		#define TE_VM_FLAGS_SUPPORTED (TE_VM_FLAGS_PARSE_RX | TE_VM_FLAGS_PARSE_TX | TE_VM_FLAGS_INSTANTIATE_RX_ONLY | TE_VM_FLAGS_INSTANTIATE_TX_ONLY)


		MidiDriver::MidiDriver() {

		}
		MidiDriver::~MidiDriver() {
			dispose_();
		}
		const bool MidiDriver::IsEmpty() const {
			return (hdrv__ == nullptr) ||
				(vCreatePortEx2_ == nullptr) ||
				(vCreatePortEx3 == nullptr) ||
				(vClosePort_ == nullptr) ||
				(vSendData_ == nullptr) ||
				(vGetData == nullptr) ||
				(vGetProcesses == nullptr) ||
				(vShutdown == nullptr) ||
				(vGetVersion == nullptr) ||
				(vGetDriverVer_ == nullptr) ||
				(vLogging == nullptr);
		}
		const bool MidiDriver::Check() {
			return init_();
		}

		void MidiDriver::dispose_() {
			HMODULE hdrv = hdrv__;
			hdrv__ = nullptr;
			if (hdrv && (hdrv != INVALID_HANDLE_VALUE)) {
				try {
					(void)::FreeLibrary(hdrv);
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
			}
		}

		void MidiDriver::clear_() {
			dispose_();
			vCreatePortEx2_ = nullptr;
			vCreatePortEx3 = nullptr;
			vClosePort_ = nullptr;
			vSendData_ = nullptr;
			vGetData = nullptr;
			vGetProcesses = nullptr;
			vShutdown = nullptr;
			vGetVersion = nullptr;
			vGetDriverVer_ = nullptr;
			vLogging = nullptr;
		}

		bool MidiDriver::init_() {
			if (hdrv__) return true;
			try {

				hdrv__ = ::LoadLibraryExW(L"tevirtualMIDI", 0, LOAD_LIBRARY_SEARCH_SYSTEM32);
				if (!hdrv__ || (hdrv__ == INVALID_HANDLE_VALUE)) {
					clear_();
					return false;
				}
				vCreatePortEx2_	= reinterpret_cast<vMIDICreatePortEx2>(::GetProcAddress(hdrv__, "virtualMIDICreatePortEx2"));
				vCreatePortEx3	= reinterpret_cast<vMIDICreatePortEx3>(::GetProcAddress(hdrv__, "virtualMIDICreatePortEx3"));
				vClosePort_		= reinterpret_cast<vMIDIClosePort>(::GetProcAddress(hdrv__, "virtualMIDIClosePort"));
				vSendData_		= reinterpret_cast<vMIDISendData>(::GetProcAddress(hdrv__, "virtualMIDISendData"));
				vGetData		= reinterpret_cast<vMIDIGetData>(::GetProcAddress(hdrv__, "virtualMIDIGetData"));
				vGetProcesses	= reinterpret_cast<vMIDIGetProcesses>(::GetProcAddress(hdrv__, "virtualMIDIGetProcesses"));
				vShutdown		= reinterpret_cast<vMIDIShutdown>(::GetProcAddress(hdrv__, "virtualMIDIShutdown"));
				vGetVersion		= reinterpret_cast<vMIDIGetVersion>(::GetProcAddress(hdrv__, "virtualMIDIGetVersion"));
				vLogging		= reinterpret_cast<vMIDILogging>(::GetProcAddress(hdrv__, "virtualMIDILogging"));
				vGetDriverVer_	= reinterpret_cast<vMIDIGetDriverVersion>(::GetProcAddress(hdrv__, "virtualMIDIGetDriverVersion"));

				if (IsEmpty()) {
					clear_();
					return false;
				}
				return true;

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			clear_();
			return false;
		}

		const bool MidiDriver::CheckMMRESULT(std::function<MMRESULT()> f, std::wstring tag) {
			_set_se_translator(seh_exception_catch);
			try {
				MMRESULT m = f();
				if (m == S_OK)
					return true;
				if (m == (UINT)-1)
					Common::to_log::Get() << (log_string() << tag << Utils::DefaulSehErrorFound());
				else
					Common::to_log::Get() << (log_string() << tag << Utils::MMRESULT_to_string(m));
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}

		/* API virtual midi */
		LPCWSTR MidiDriver::vGetDriverVersion() {
			__try {
				if (!vGetDriverVer_) return nullptr;
				return vGetDriverVer_(nullptr, nullptr, nullptr, nullptr);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
		}
		bool MidiDriver::vSendData(LPVM_MIDI_PORT p, LPBYTE b, DWORD z) {
			__try {
				if (!vSendData_) return false;
				return vSendData_(p, b, z);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
		}
		void MidiDriver::vClosePort(LPVM_MIDI_PORT p) {
			__try {
				if (!vClosePort_) return;
				return vClosePort_(p);
			} __except (EXCEPTION_EXECUTE_HANDLER) {}
		}
		LPVM_MIDI_PORT MidiDriver::vCreatePortEx2(LPCWSTR s, LPVM_MIDI_DATA_CB c, DWORD_PTR p) {
			__try {
				if (!vCreatePortEx2_) return nullptr;
				return vCreatePortEx2_(
					s, c, p,
					TE_VM_DEFAULT_BUFFER_SIZE,
					(TE_VM_FLAGS_PARSE_TX | TE_VM_FLAGS_INSTANTIATE_BOTH)
				);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
		}

		/* standart API midi */
		UINT MidiDriver::InGetNumDevs() {
			__try {
				return ::midiInGetNumDevs();
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		UINT MidiDriver::OutGetNumDevs() {
			__try {
				return ::midiOutGetNumDevs();
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT MidiDriver::InStart(HMIDIIN h) {
			__try {
				return ::midiInStart(h);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT MidiDriver::InStop(HMIDIIN h) {
			__try {
				return ::midiInStop(h);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT MidiDriver::InReset(HMIDIIN h) {
			__try {
				return ::midiInReset(h);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT MidiDriver::InClose(HMIDIIN h) {
			__try {
				return ::midiInClose(h);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT MidiDriver::OutReset(HMIDIOUT h) {
			__try {
				return ::midiOutReset(h);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT MidiDriver::OutClose(HMIDIOUT h) {
			__try {
				return ::midiOutClose(h);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT MidiDriver::InOpen(LPHMIDIIN h, UINT u, DWORD_PTR p1, DWORD_PTR p2) {
			__try {
				return ::midiInOpen(h, u, p1, p2, CALLBACK_FUNCTION);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT MidiDriver::OutOpen(LPHMIDIOUT h, UINT u, DWORD_PTR p1, DWORD_PTR p2) {
			__try {
				return ::midiOutOpen(h, u, p1, p2, CALLBACK_FUNCTION);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT MidiDriver::InGetDevCaps(UINT_PTR p, LPMIDIINCAPSW c, UINT u) {
			__try {
				return ::midiInGetDevCapsW(p, c, u);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT MidiDriver::OutGetDevCaps(UINT_PTR p, LPMIDIOUTCAPSW c, UINT u) {
			__try {
				return ::midiOutGetDevCapsW(p, c, u);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
		MMRESULT MidiDriver::OutShortMsg(HMIDIOUT h, DWORD d) {
			__try {
				return ::midiOutShortMsg(h, d);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return (UINT)-1; }
		}
	}
}