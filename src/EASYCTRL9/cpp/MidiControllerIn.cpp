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

		constexpr std::wstring_view strError0 = L"["sv;
		constexpr std::wstring_view strError1 = L"configuration name not specified, abort"sv;
		constexpr std::wstring_view strError2 = L"devices not connected, abort"sv;
		constexpr std::wstring_view strError3 = L": warning, inconsistency in the number of devices in the final list, system/list: ["sv;
		constexpr std::wstring_view strError4 = L": Cannot open MIDI device, abort: "sv;
		constexpr std::wstring_view strError5 = L": Cannot start MIDI device, abort: "sv;
		constexpr std::wstring_view strError6 = L"device not connected, abort: "sv;
		constexpr std::wstring_view strError7 = L"device found: "sv;
		constexpr std::wstring_view strError8 = L"device open: "sv;
		constexpr std::wstring_view strError9 = L"] was closed (system)"sv;

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
					Common::to_log::Get() << (log_string() << LogTag << strError0 << active_device__ << strError9);
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
					throw runtime_werror((log_string() << LogTag << strError1));

				uint32_t devcount = UINT_MAX;
				if (!BuildDeviceList(devcount))
					throw runtime_werror((log_string() << LogTag << strError2));

				if (devcount != device_list__.size())
					Common::to_log::Get()
						<< (log_string() << LogTag << strError3
						<< devcount << L"/" << device_list__.size() << L"]");

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
					throw runtime_werror(log_string() << LogTag << strError6 << active_device__);
				Common::to_log::Get() << (log_string() << LogTag << strError7 << device_list__.at(devid) << L"/" << devid);

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
					throw runtime_werror(log_string() << LogTag << strError4 << active_device__ << L"/" << devid);

				isenable__ = midi_utils::Check_MMRESULT(
					[&]() -> MMRESULT {
						return midi_utils::Run_midiInStart(midi_in_handle__);
					},
					LogTag
				);
				if (!isenable__) {
					Dispose();
					throw runtime_werror(log_string() << LogTag << strError5 << active_device__ << L"/" << devid);
				}
				Common::to_log::Get() << (log_string() << LogTag << strError8 << device_list__.at(devid).c_str() << L"/" << devid);
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