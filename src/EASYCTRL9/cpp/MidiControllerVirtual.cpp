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

		MidiControllerVirtual::MidiControllerVirtual(IO::PluginCb& cb, std::shared_ptr<MidiDriver>& drv, std::wstring dev)
			: midi_port_(nullptr), MidiControllerBase(cb, drv, dev) {
			if (active_device_.empty())
				throw make_common_error(common_error_id::err_NOT_CONFIG);
		}
		MidiControllerVirtual::~MidiControllerVirtual() {
			dispose_();
		}

		void MidiControllerVirtual::dispose_() {
			_set_se_translator(seh_exception_catch);
			try {
				LPVM_MIDI_PORT mp = midi_port_;
				midi_port_ = nullptr;
				if (mp) {
					(void) mdrv_->CheckMMRESULT(
						[&]() {
							return mdrv_->vShutdown(mp) ? S_OK : ((UINT)-1);
						}, LogTag.data());
					(void) mdrv_->CheckMMRESULT(
						[&]() {
							mdrv_->vClosePort(mp);
							return S_OK;
						}, LogTag.data());
					
					to_log::Get() << log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_DEVICE_CLOSE),
						active_device_
					);
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			isenable_ = false;
		}
		void MidiControllerVirtual::Stop() {
			dispose_();
		}
		const bool MidiControllerVirtual::Start() {

			if (isenable_) dispose_();
			if (!mdrv_ || !mdrv_->Check()) return false;

			std::unique_lock<std::shared_mutex> lock(mtx_);

			(void) ::GetLastError();

			LPCWSTR v = mdrv_->vGetDriverVersion();
			if (v)
				to_log::Get() << log_string().to_log_format(
					__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_MIDI_DRIVER_VER_OK),
					v
				);
			else 
				to_log::Get() << log_string().to_log_format(
					__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_MIDI_DRIVER_VER_ERROR),
					::GetLastError()
				);

			(void) ::GetLastError();
			midi_port_ = mdrv_->vCreatePortEx2(
				active_device_.c_str(),
				&VP_MIDIDATA_CB, 0
			);
			DWORD err = ::GetLastError();

			std::wstring_view s{};
			switch (err) {
				case MidiDriver::STAT::ERR_NOT_ERRORS: {
					to_log::Get() << log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_DEVICE_OPEN),
						active_device_, L"(system)"
					);
					isenable_ = (midi_port_ != nullptr);
					return isenable_;
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
			to_log::Get() << log_string().to_log_format(
				__FUNCTIONW__,
				common_error_code::Get().get_error(common_error_id::err_MIDI_OPEN_DEVICE),
				active_device_, s
			);
			return false;
		}
		const bool MidiControllerVirtual::Start(std::shared_ptr<JSON::MMTConfig>&) {
			return Start();
		}

		void MidiControllerVirtual::SendToPort(Mackie::MIDIDATA& m, DWORD& t) {
			_set_se_translator(seh_exception_catch);
			try {
				if (!midi_port_) return;
				if (!mdrv_->vSendData(midi_port_, m.data, 3)) {
					to_log::Get() << log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_SEND_BAD_VALUES),
						active_device_, ::GetLastError()
					);
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void MidiControllerVirtual::SendToPort(MIDI::MidiUnit&, DWORD&) {
		}
	}
}