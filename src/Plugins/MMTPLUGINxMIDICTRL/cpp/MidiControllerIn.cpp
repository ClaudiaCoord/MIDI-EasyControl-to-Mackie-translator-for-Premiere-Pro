/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session values/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MMTPLUGINxMIDIDEV - Midi input controller

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIDI {

		MidiControllerIn::MidiControllerIn(IO::PluginCb& cb, std::shared_ptr<MidiDriver>& drv, std::wstring dev)
			: midi_in_handle_(nullptr), MidiControllerBase(cb, drv, dev) {
			if (active_device_.empty())
				throw make_common_error(common_error_id::err_NOT_CONFIG);
		}
		MidiControllerIn::~MidiControllerIn() {
			dispose_();
		}

		void MidiControllerIn::dispose_() {
			_set_se_translator(seh_exception_catch);
			try {
				HMIDIIN h = midi_in_handle_;
				midi_in_handle_ = nullptr;
				if (h) {
					(void)mdrv_->CheckMMRESULT(
						[&]() -> MMRESULT {
							return mdrv_->InReset(h);
						},
						LogTag.data()
					);
					(void) mdrv_->CheckMMRESULT(
						[&]() -> MMRESULT {
							return mdrv_->InStop(h);
						},
						LogTag.data()
					);
					(void) mdrv_->CheckMMRESULT(
						[&]() -> MMRESULT {
							return mdrv_->InClose(h);
						},
						LogTag.data()
					);
					log_string ls;
					cb_.ToLogRef(ls.to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_DEVICE_CLOSE),
						active_device_
					));
					isenable_ = isconnect_ = false;
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			isenable_ = isconnect_ = false;
		}
		void MidiControllerIn::Stop() {
			dispose_();
		}
		const bool MidiControllerIn::Start() {

			try {
				if (active_device_.empty())
					throw make_common_error(log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_NOT_DEVICE_NAME),
						L"-", 0)
					);

				std::unique_lock<std::shared_mutex> lock(mtx_);

				if (isenable_ || isconnect_) dispose_();

				std::vector<std::wstring>& list = MidiDevices::Get().GetMidiInDeviceList();
				if (list.empty()) {
					list = MidiDevices::Get().GetMidiInDeviceList(true);
					if (list.empty())
						throw make_common_error(common_error_id::err_NOT_CONFIG);
				}

				uint32_t id = 0U, devid = (UINT_MAX - 1);
				for (auto& a : list) {
					if (active_device_._Equal(a)) {
						devid = id;
						break;
					}
					id++;
				}
				isenable_ = (devid != (UINT_MAX - 1));
				if (!isenable_)
					throw make_common_error(log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_DEVICE_NOT_CONNECT),
						active_device_, id)
					);

				{
					log_string ls;
					cb_.ToLogRef(log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_DEVICE_FOUND),
						active_device_, devid
					));
				}

				isenable_ = mdrv_->CheckMMRESULT(
					[&]() -> MMRESULT {
						return mdrv_->InOpen(
							&midi_in_handle_,
							static_cast<UINT>(devid),
							(DWORD_PTR)&MidiInProc_,
							(DWORD_PTR)this
						);
					},
					LogTag.data()
				);
				if (!isenable_ || !midi_in_handle_)
					throw make_common_error(log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_OPEN_DEVICE),
						active_device_, devid)
					);

				isconnect_ = mdrv_->CheckMMRESULT(
					[&]() -> MMRESULT {
						return mdrv_->InStart(midi_in_handle_);
					},
					LogTag.data()
				);
				if (!isconnect_)
					throw make_common_error(log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_START_DEVICE),
						active_device_, devid)
					);

				{
					log_string ls;
					cb_.ToLogRef(log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_DEVICE_OPEN),
						active_device_, devid
					));
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				dispose_();
			}
			return isenable_ && isconnect_;
		}
		const bool MidiControllerIn::Start(std::shared_ptr<JSON::MMTConfig>&) {
			return Start();
		}

		void MidiControllerIn::SendToPort(MIDI::Mackie::MIDIDATA&, DWORD&) {
		}
		void MidiControllerIn::SendToPort(MIDI::MidiUnit&, DWORD&) {
		}
	}
}