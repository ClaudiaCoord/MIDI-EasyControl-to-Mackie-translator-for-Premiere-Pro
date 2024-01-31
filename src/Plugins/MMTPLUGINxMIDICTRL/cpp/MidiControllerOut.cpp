/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session values/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MMTPLUGINxMIDIDEV - Midi output controller

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIDI {

		MidiControllerOut::MidiControllerOut(IO::PluginCb& cb, std::shared_ptr<MIDI::MidiDriver>& drv, std::wstring dev)
			: midi_out_handle_(nullptr), MidiControllerBase(cb, drv, dev), issystemport_(false) {
			if (active_device_.empty())
				throw make_common_error(common_error_id::err_NOT_CONFIG);
		}
		MidiControllerOut::~MidiControllerOut() {
			dispose_();
		}

		void MidiControllerOut::dispose_() {
			_set_se_translator(seh_exception_catch);
			try {
				HMIDIOUT h = midi_out_handle_;
				midi_out_handle_ = nullptr;
				if (h) {
					(void)mdrv_->CheckMMRESULT(
						[&]() -> MMRESULT {
							return mdrv_->OutReset(h);
						},
						LogTag.data()
					);
					(void)mdrv_->CheckMMRESULT(
						[&]() -> MMRESULT {
							return mdrv_->OutClose(h);
						},
						LogTag.data()
					);

					log_string ls;
					cb_.ToLogRef(log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_DEVICE_CLOSE),
						active_device_
					));
				}
				if (vmdev_ptr_) {
					vmdev_ptr_.get()->Stop();
					vmdev_ptr_.reset();
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			isenable_ = isconnect_ = false;
		}

		bool MidiControllerOut::IsSystemPort() const {
			return issystemport_;
		}
		void MidiControllerOut::IsSystemPort(bool b, std::wstring s) {
			issystemport_ = b;
			if (!s.empty()) active_device_ = s;
		}

		void MidiControllerOut::Stop() {
			dispose_();
		}
		const bool MidiControllerOut::Start(std::shared_ptr<JSON::MMTConfig>&) {
			return Start();
		}
		const bool MidiControllerOut::Start() {

			try {
				if (active_device_.empty())
					throw make_common_error(log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_NOT_DEVICE_NAME),
						L"-", 0)
					);

				std::unique_lock<std::shared_mutex> lock(mtx_);

				if (isenable_ || isconnect_) dispose_();

				if (!issystemport_) {

					if (!vmdev_ptr_)
						vmdev_ptr_.reset(new MidiControllerVirtual(cb_, mdrv_, active_device_));

					if (!vmdev_ptr_.get()->IsEnable())
						isconnect_ = vmdev_ptr_.get()->Start();

					if (!isconnect_)
						throw make_common_error(log_string().to_log_format(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_MIDI_DEVICE_NOT_CONNECT),
							active_device_, 0)
						);

					isenable_ = vmdev_ptr_.get()->IsEnable();

					{
						log_string ls;
						cb_.ToLogRef(log_string().to_log_format(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_MIDI_DEVICE_OPEN),
							active_device_, 0
						));
					}

					return isenable_ && isconnect_;
				}

				/* is manual port */

				std::vector<std::wstring>& list = MidiDevices::Get().GetMidiOutDeviceList();
				if (list.empty()) {
					list = MidiDevices::Get().GetMidiOutDeviceList(true);
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

				isconnect_ = mdrv_->CheckMMRESULT(
					[&]() -> MMRESULT {
					return mdrv_->OutOpen(
							&midi_out_handle_,
							static_cast<UINT>(devid),
							(DWORD_PTR)&MidiOutProc_,
							(DWORD_PTR)this
						);
					},
					LogTag.data()
				);
				if (!isconnect_ || !midi_out_handle_)
					throw make_common_error(log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_OPEN_DEVICE),
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

		void MidiControllerOut::SendToPort(MIDI::MidiUnit& u, DWORD& t) {
			_set_se_translator(seh_exception_catch);
			try {
				if (!isenable_ || !isconnect_) return;

				MIDI::Mackie::MIDIDATA m{};
				if (!MIDI::Mackie::SelectorTarget(u, m, cb_.GetType()))
					return;

				if (issystemport_) {
					if (midi_out_handle_) mdrv_->OutShortMsg(midi_out_handle_, m.send);
					return;
				}
				vmdev_ptr_.get()->SendToPort(m, t);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void MidiControllerOut::SendToPort(MIDI::Mackie::MIDIDATA&, DWORD&) {
		}
	}
}