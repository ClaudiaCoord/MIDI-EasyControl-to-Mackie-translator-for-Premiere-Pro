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

		MidiControllerOut::MidiControllerOut(std::shared_ptr<MidiDriver> drv) : midi_out_handle__(nullptr), ismanualport__(false), MidiControllerBase(drv) {
			this_type__ = ClassTypes::ClassOutMidiMackie;
		}
		MidiControllerOut::~MidiControllerOut() {
			Dispose();
		}
		void MidiControllerOut::dispose_(bool b) {
			_set_se_translator(seh_exception_catch);
			try {
				HMIDIOUT h = midi_out_handle__;
				midi_out_handle__ = nullptr;
				if (h != nullptr) {
					(void)mdrv__->CheckMMRESULT(
						[&]() -> MMRESULT {
						return mdrv__->OutReset(h);
					},
						LogTag
					);
					(void)mdrv__->CheckMMRESULT(
						[&]() -> MMRESULT {
						return mdrv__->OutClose(h);
					},
						LogTag
					);
					if (ismanualport__)
						to_log::Get() << log_string().to_log_fomat(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_DEVICE_CLOSE),
							active_device__
						);
				}
				if (b && vmdev_ptr__) {
					vmdev_ptr__.get()->Stop();
					vmdev_ptr__.reset();
					isconnect__ = false;
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			isenable__ = false;
		}
		void MidiControllerOut::Dispose() {
			dispose_(true);
		}
		bool MidiControllerOut::BuildDeviceList(uint32_t& devid) {
			_set_se_translator(seh_exception_catch);
			try {
				devid = UINT_MAX;
				device_list__.clear();
				uint32_t cnt = mdrv__->OutGetNumDevs();
				for (size_t i = 0; i < cnt; ++i) {

					MIDIOUTCAPS mc{};
					MMRESULT m = mdrv__->OutGetDevCaps(i, &mc, sizeof(mc));
					if (m != S_OK) {
						Common::to_log::Get() << (log_string() << LogTag << Utils::MMRESULT_to_string(m) << L" [" << i << L"]");
						continue;
					}
					std::wstring name = std::wstring(mc.szPname);
					if (active_device__._Equal(name)) devid = static_cast<uint32_t>(i);
					device_list__.push_back(name);
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return !device_list__.empty();
		}

		bool MidiControllerOut::IsManualPort() {
			return ismanualport__;
		}
		void MidiControllerOut::IsManualPort(bool b) {
			ismanualport__ = b;
		}

		void MidiControllerOut::Stop() {
			Dispose();
		}
		const bool MidiControllerOut::Start(std::shared_ptr<MidiDevice>& cnf) {

			try {
				if (isenable__ || isconnect__) Dispose();

				if ((!cnf) || (cnf.get()->name.empty()))
					throw_common_error(common_error_id::err_NOT_CONFIG);

				active_device__ = cnf->name;
				uint32_t devid = UINT_MAX;
				if (!BuildDeviceList(devid))
					throw_common_error(log_string().to_log_fomat(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_DEVICE_NOT_CONNECT),
						cnf.get()->name, devid)
					);

				ismanualport__ = cnf.get()->manualport;
				if (!ismanualport__) {
					active_device__ = Utils::device_out_name(cnf.get()->name, MidiHelper::GetSuffixMackieOut());
					if (active_device__.empty())
						throw_common_error(log_string().to_log_fomat(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_NOT_DEVICE_NAME),
							L"-", devid)
						);

					if (!vmdev_ptr__)
						vmdev_ptr__.reset(new MidiControllerVirtual(mdrv__, active_device__));

					isenable__ = vmdev_ptr__.get()->IsEnable();
					if (!isenable__)
						isenable__ = vmdev_ptr__.get()->Start();

					if (!isenable__)
						throw_common_error(log_string().to_log_fomat(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_DEVICE_NOT_CONNECT),
							active_device__, devid)
						);

					isenable__ = isconnect__ = vmdev_ptr__.get()->IsEnable();
					to_log::Get() << log_string().to_log_fomat(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_DEVICE_OPEN),
						active_device__, devid
					);
					return isenable__;
				}

				dispose_(false);

				isenable__ = (devid < device_list__.size());
				if (!isenable__)
					throw_common_error(log_string().to_log_fomat(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_DEVICE_NOT_CONNECT),
						cnf.get()->name, devid)
					);

				active_device__ = device_list__.at(devid);
				isenable__ = mdrv__->CheckMMRESULT(
					[&]() -> MMRESULT {
						return mdrv__->OutOpen(
							&midi_out_handle__,
							static_cast<UINT>(devid),
							(DWORD_PTR)&MidiOutProc,
							(DWORD_PTR)this
						);
					},
					LogTag
				);
				if (!isenable__)
					throw_common_error(log_string().to_log_fomat(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_OPEN_MIDI_DEVICE),
						active_device__, devid)
					);

				to_log::Get() << log_string().to_log_fomat(
					__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_DEVICE_OPEN),
					active_device__, devid
				);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				Dispose();
			}
			return isenable__;
		}

		const bool MidiControllerOut::SendToPort(MidiUnit& u, DWORD& t) {
			_set_se_translator(seh_exception_catch);
			try {
				if (!isenable__ || !isconnect__) return false;

				Mackie::MIDIDATA m{};
				if (!Mackie::SelectorTarget(u, m, this_type__))
					return false;

				if (ismanualport__)
					return mdrv__->OutShortMsg(midi_out_handle__, m.send) == S_OK;
				return (vmdev_ptr__) ? vmdev_ptr__.get()->SendToPort(m, t) : false;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
	}
}