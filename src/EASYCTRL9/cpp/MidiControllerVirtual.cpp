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

		using namespace std::string_view_literals;

		/* NOT USED */
		static void CALLBACK VP_MIDIDATA_CB(LPVM_MIDI_PORT, LPBYTE, DWORD, DWORD_PTR) {}

		MidiControllerVirtual::MidiControllerVirtual(std::shared_ptr<MidiDriver> drv, std::wstring name) : midi_port__(nullptr), MidiControllerBase(drv) {
			this_type__ = ClassTypes::ClassVirtualMidi;
			active_device__ = name;
			if (active_device__.empty())
				throw_common_error(common_error_id::err_NOT_CONFIG);
		}
		MidiControllerVirtual::~MidiControllerVirtual() {
			Dispose();
		}

		void MidiControllerVirtual::Dispose() {
			_set_se_translator(seh_exception_catch);
			try {
				if (midi_port__ != nullptr) {
					(void) mdrv__->CheckMMRESULT(
						[&]() {
						mdrv__->vClosePort(midi_port__);
							return S_OK;
						}, LogTag);
					midi_port__ = nullptr;
					to_log::Get() << log_string().to_log_fomat(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_DEVICE_CLOSE),
						active_device__
					);
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			isenable__ = false;
		}
		const bool MidiControllerVirtual::Start() {

			if (isenable__) Dispose();
			isenable__ = false;

			if (!mdrv__ || !mdrv__->Check()) return false;

			(void) ::GetLastError();

			LPCWSTR v = mdrv__->vGetDriverVersion();
			if (v != nullptr)
				to_log::Get() << log_string().to_log_fomat(
					__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_DRIVER_VER_OK),
					v
				);
			else 
				to_log::Get() << log_string().to_log_fomat(
					__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_DRIVER_VER_ERROR),
					::GetLastError()
				);

			(void) ::GetLastError();
			midi_port__ = mdrv__->vCreatePortEx2(
				active_device__.c_str(),
				&VP_MIDIDATA_CB, 0
			);
			DWORD err = ::GetLastError();

			std::wstring_view s{};
			switch (err) {
				case MidiDriver::STAT::ERR_NOT_ERRORS: {
					to_log::Get() << log_string().to_log_fomat(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_DEVICE_OPEN),
						active_device__, L"(system)"
					);
					isenable__ = (midi_port__ != nullptr);
					return isenable__;
				}
				case MidiDriver::STAT::ERR_PATH_NOT_FOUND: {
					s = L"Driver DLL - path not found"sv;
					break;
				}
				case MidiDriver::STAT::ERR_INVALID_HANDLE: {
					s = L"Invalid port handle"sv;
					break;
				}
				case MidiDriver::STAT::ERR_TOO_MANY_CMDS: {
					s = L"Too many commands"sv;
					break;
				}
				case MidiDriver::STAT::ERR_TOO_MANY_SESS: {
					s = L"Too many sessions"sv;
					break;
				}
				case MidiDriver::STAT::ERR_INVALID_NAME: {
					s = L"Invalid name"sv;
					break;
				}
				case MidiDriver::STAT::ERR_MOD_NOT_FOUND: {
					s = L"Module not found"sv;
					break;
				}
				case MidiDriver::STAT::ERR_BAD_ARGUMENTS: {
					s = L"Bad arguments"sv;
					break;
				}
				case MidiDriver::STAT::ERR_ALREADY_EXISTS: {
					s = L"Already exists"sv;
					break;
				}
				case MidiDriver::STAT::ERR_OLD_WIN_VERSION: {
					s = L"Old win version"sv;
					break;
				}
				case MidiDriver::STAT::ERR_REVISION_MISMATCH: {
					s = L"Revision mismatch"sv;
					break;
				}
				case MidiDriver::STAT::ERR_ALIAS_EXISTS: {
					s = L"Alias exists"sv;
					break;
				}
				default: break;
			}
			to_log::Get() << log_string().to_log_fomat(
				__FUNCTIONW__,
				common_error_code::Get().get_error(common_error_id::err_OPEN_MIDI_DEVICE),
				active_device__, s
			);
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
					if (!mdrv__->vSendData(midi_port__, m.data, 3)) {
						to_log::Get() << log_string().to_log_fomat(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_SEND_BAD_VALUES),
							active_device__, ::GetLastError()
						);
						break;
					}
					return true;
				} while (0);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
	}
}