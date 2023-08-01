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

		static MidiControllerIn ctrl_midicontrollerin__;

		MidiControllerIn::MidiControllerIn() : midi_in_handle__(nullptr) {
			this_type__ = ClassTypes::ClassInMidi;
		}
		MidiControllerIn::~MidiControllerIn() {
			Dispose();
		}
		void MidiControllerIn::Dispose() {
			_set_se_translator(seh_exception_catch);
			try {
				HMIDIIN h = midi_in_handle__;
				midi_in_handle__ = nullptr;
				if (h != nullptr) {
					(void) midi_utils::Check_MMRESULT(
						[&]() -> MMRESULT {
							return midi_utils::Run_midiInStop(h);
						},
						LogTag
					);
					(void) midi_utils::Check_MMRESULT(
						[&]() -> MMRESULT {
							return midi_utils::Run_midiInReset(h);
						},
						LogTag
					);
					(void) midi_utils::Check_MMRESULT(
						[&]() -> MMRESULT {
							return midi_utils::Run_midiInClose(h);
						},
						LogTag
					);
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
			isenable__ = isconnect__ = false;
		}
		void MidiControllerIn::Stop() {
			Dispose();
		}
		const bool MidiControllerIn::Start(std::shared_ptr<MidiDevice>& cnf) {

			try {
				if (isenable__ || isconnect__) Dispose();

				if ((!cnf) || (cnf.get()->name.empty()))
					throw_common_error(common_error_id::err_NOT_CONFIG);

				uint32_t devcount = UINT_MAX;
				if (!BuildDeviceList(devcount))
					throw_common_error(common_error_id::err_NOT_DEVICES);

				if (devcount != device_list__.size())
					to_log::Get() << log_string().to_log_fomat(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_COUNT_DEVICES),
						devcount, device_list__.size()
					);

				active_device__ = cnf->name;
				uint32_t devid = UINT_MAX;
				uint32_t id = 0U;
				for (auto& a : device_list__) {
					if (active_device__._Equal(a)) {
						devid = id;
						break;
					}
					id++;
				}
				isenable__ = (devid < device_list__.size());
				if (!isenable__)
					throw_common_error(log_string().to_log_fomat(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_DEVICE_NOT_CONNECT),
						active_device__, devid)
					);
				to_log::Get() << log_string().to_log_fomat(
					__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_DEVICE_FOUND),
					device_list__.at(devid), devid
				);

				isenable__ = midi_utils::Check_MMRESULT(
					[&]() -> MMRESULT {
						return midi_utils::Run_midiInOpen(
							&midi_in_handle__,
							static_cast<UINT>(devid),
							(DWORD_PTR)&MidiInProc,
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

				isenable__ = midi_utils::Check_MMRESULT(
					[&]() -> MMRESULT {
						return midi_utils::Run_midiInStart(midi_in_handle__);
					},
					LogTag
				);
				if (!isenable__) {
					Dispose();
					throw_common_error(log_string().to_log_fomat(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_START_MIDI_DEVICE),
						active_device__, devid)
					);
				}
				to_log::Get() << log_string().to_log_fomat(
					__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_DEVICE_OPEN),
					device_list__.at(devid), devid
				);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				Dispose();
			}
			return isenable__;
		}
		bool MidiControllerIn::BuildDeviceList(uint32_t& devcnt) {
			_set_se_translator(seh_exception_catch);
			try {
				devcnt = UINT_MAX;
				device_list__.clear();
				uint32_t cnt = midi_utils::Run_midiInGetNumDevs();
				for (size_t i = 0; i < cnt; ++i) {

					MIDIINCAPS mc{};
					MMRESULT m = midi_utils::Run_midiInGetDevCapsW(i, &mc, sizeof(mc));
					if (m != S_OK) {
						Common::to_log::Get() << (log_string() << LogTag << Utils::MMRESULT_to_string(m) << L" [" << i << L"]");
						continue;
					}
					device_list__.push_back(std::wstring(mc.szPname));
				}
				devcnt = cnt;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return !device_list__.empty();
		}

		std::vector<std::wstring>& MidiControllerIn::GetReBuildDeviceList() {
			if (!device_list__.empty())
				return std::ref(device_list__);
			uint32_t cnt = UINT_MAX;
			(void) BuildDeviceList(cnt);
			return std::ref(device_list__);
		}

		MidiControllerIn& MidiControllerIn::Get() noexcept {
			return std::ref(ctrl_midicontrollerin__);
		}
	}
}