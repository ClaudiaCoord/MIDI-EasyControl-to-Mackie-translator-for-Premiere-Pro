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

		/* NOT USED */
		static void CALLBACK VP_MIDIDATA_CB(LPVM_MIDI_PORT, LPBYTE, DWORD, DWORD_PTR) {}

		MidiControllerVirtual::MidiControllerVirtual(std::wstring& name) : midi_port__(nullptr) {
			this_type__ = ClassTypes::ClassVirtualMidi;
			active_device__ = name;
			if (active_device__.empty())
				throw new runtime_werror(log_string() << LogTag << L"configuration name not specified, abort");
		}
		MidiControllerVirtual::~MidiControllerVirtual() {
			Dispose();
		}

		void MidiControllerVirtual::Dispose() {
			_set_se_translator(seh_exception_catch);
			try {
				if (midi_port__ != nullptr) {
					midi_utils::Check_MMRESULT(
						[&]() {
							midi_utils::Run_virtualMIDIClosePort(midi_port__);
							return S_OK;
						}, LogTag);
					midi_port__ = nullptr;
					Common::to_log::Get() << (log_string() << LogTag << L"[" << active_device__ << L"] was closed (system)");
				}
			}
			catch (seh_exception& err) {
				Common::to_log::Get() << (log_string() << LogTag << L"SEH EXCEPTION: " << err.error());
			}
			catch (std::runtime_error& err) {
				Common::to_log::Get() << (log_string() << LogTag << __FUNCTIONW__ << L": " << err);
			}
			catch (...) {
				Common::to_log::Get() << (log_string() << LogTag << __FUNCTIONW__ << L": " << Utils::DefaulRuntimeError());
			}
			isenable__ = false;
		}
		const bool MidiControllerVirtual::Start() {

			if (isenable__) Dispose();

			isenable__ = false;
			(void) ::GetLastError();

			LPCWSTR v = midi_utils::Run_virtualMIDIGetDriverVersion();
			if (v != nullptr)
				Common::to_log::Get() << (log_string() << LogTag << L"driver version [" << v << "]");
			else 
				Common::to_log::Get() << (log_string() << LogTag << L"driver version trouble: [" << ::GetLastError() << "]");

			(void) ::GetLastError();
			midi_port__ = midi_utils::Run_virtualMIDICreatePortEx2(
				active_device__.c_str(),
				&VP_MIDIDATA_CB, 0
			);
			DWORD err = ::GetLastError();

			std::wstring s{};
			switch (err) {
				case ERR_NOT_ERRORS: {
					Common::to_log::Get() << (log_string() << LogTag << L"[" << active_device__ << "]: OK");
					isenable__ = (midi_port__ != nullptr);
					return isenable__;
				}
				case ERR_PATH_NOT_FOUND: {
					s = L"driver DLL - path not found";
					break;
				}
				case ERR_INVALID_HANDLE: {
					s = L"Invalid port handle";
					break;
				}
				case ERR_TOO_MANY_CMDS: {
					s = L"Too many commands";
					break;
				}
				case ERR_TOO_MANY_SESS: {
					s = L"Too many sessions";
					break;
				}
				case ERR_INVALID_NAME: {
					s = L"Invalid name";
					break;
				}
				case ERR_MOD_NOT_FOUND: {
					s = L"Module not found";
					break;
				}
				case ERR_BAD_ARGUMENTS: {
					s = L"Bad arguments";
					break;
				}
				case ERR_ALREADY_EXISTS: {
					s = L"Already exists";
					break;
				}
				case ERR_OLD_WIN_VERSION: {
					s = L"Old win version";
					break;
				}
				case ERR_REVISION_MISMATCH: {
					s = L"Revision mismatch";
					break;
				}
				case ERR_ALIAS_EXISTS: {
					s = L"Alias exists";
					break;
				}
				default:
					break;
			}
			Common::to_log::Get() << (log_string() << LogTag << L"[" << active_device__ << L"]: (" << err << L") " << s);
			return false;
		}
		void MidiControllerVirtual::Stop() {
			Dispose();
		}

		const bool MidiControllerVirtual::SendToPort(Mackie::MIDIDATA& m, DWORD& t) {
			_set_se_translator(seh_exception_catch);
			try {
				do {
					if (midi_port__ == nullptr) break;
					if (!midi_utils::Run_virtualMIDISendData(midi_port__, m.data, 3)) {
						Common::to_log::Get() << (log_string() << LogTag << L"[" << active_device__ << L"]: send bad values = " << GetLastError());
						break;
					}
					return true;
				} while (0);
			}
			catch (seh_exception& err) {
				Common::to_log::Get() << (log_string() << LogTag << L"SEH EXCEPTION: " << err.error());
			}
			catch (std::runtime_error& err) {
				Common::to_log::Get() << (log_string() << LogTag << __FUNCTIONW__ << L": " << err);
			}
			catch (...) {}
			return false;
		}
	}
}