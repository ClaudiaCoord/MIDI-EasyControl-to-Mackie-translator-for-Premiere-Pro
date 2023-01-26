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
		constexpr std::wstring_view strError1 = L"] was closed (system)"sv;
		constexpr std::wstring_view strError2 = L"configuration name not specified, abort"sv;
		constexpr std::wstring_view strError3 = L"devices not connected, abort"sv;
		constexpr std::wstring_view strError4 = L"active device not set name, abort"sv;
		constexpr std::wstring_view strError5 = L"device not connected, abort: "sv;
		constexpr std::wstring_view strError6 = L"device open: "sv;
		constexpr std::wstring_view strError7 = L": Cannot open MIDI device, abort: "sv;
		constexpr std::wstring_view strError8 = L"device open: "sv;

		static MidiControllerOut ctrl_midicontrollerout__;

		MidiControllerOut::MidiControllerOut() : midi_out_handle__(nullptr), ismanualport__(false) {
			this_type__ = ClassTypes::ClassOutMidiMackie;
		}
		MidiControllerOut::~MidiControllerOut() {
			Dispose();
		}
		void MidiControllerOut::Dispose() {
			Dispose(true);
		}
		void MidiControllerOut::Dispose(bool b) {
			_set_se_translator(seh_exception_catch);
			try {
				HMIDIOUT h = midi_out_handle__;
				midi_out_handle__ = nullptr;
				if (h != nullptr) {
					(void)midi_utils::Check_MMRESULT(
						[&]() -> MMRESULT {
							return midi_utils::Run_midiOutReset(h);
						},
						LogTag
					);
					(void)midi_utils::Check_MMRESULT(
						[&]() -> MMRESULT {
							return midi_utils::Run_midiOutClose(h);
						},
						LogTag
					);
					if (ismanualport__)
						Common::to_log::Get() << (log_string() << LogTag << strError0 << active_device__ << strError1);
				}
				if (b && vmdev_ptr__) {
					vmdev_ptr__.get()->Stop();
					vmdev_ptr__.reset();
					isconnect__ = false;
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			isenable__ = false;
		}
		bool MidiControllerOut::BuildDeviceList(uint32_t& devid) {
			_set_se_translator(seh_exception_catch);
			try {
				devid = UINT_MAX;
				device_list__.clear();
				uint32_t cnt = midi_utils::Run_midiOutGetNumDevs();
				for (size_t i = 0; i < cnt; ++i) {

					MIDIOUTCAPS mc{};
					MMRESULT m = midi_utils::Run_midiOutGetDevCapsW(i, &mc, sizeof(mc));
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
					throw runtime_werror(log_string() << LogTag << strError2);

				active_device__ = cnf->name;
				uint32_t devid = UINT_MAX;
				if (!BuildDeviceList(devid))
					throw runtime_werror(log_string() << LogTag << strError3);

				ismanualport__ = cnf.get()->manualport;
				
				if (!ismanualport__) {
					active_device__ = Utils::device_out_name(cnf.get()->name, MidiHelper::GetSuffixMackieOut());
					if (active_device__.empty())
						throw runtime_werror(log_string() << LogTag << strError4);

					if (!vmdev_ptr__)
						vmdev_ptr__.reset(new MidiControllerVirtual(active_device__));

					isenable__ = vmdev_ptr__.get()->IsEnable();
					if (!isenable__)
						isenable__ = vmdev_ptr__.get()->Start();

					if (!isenable__)
						throw runtime_werror(log_string() << LogTag << strError5 << active_device__ );

					isenable__ = isconnect__ = vmdev_ptr__.get()->IsEnable();
					Common::to_log::Get() << (log_string() << LogTag << strError6 << active_device__ << L"/" << devid);
					return isenable__;
				}

				Dispose(false);

				isenable__ = (devid < device_list__.size());
				if (!isenable__)
					throw runtime_werror(log_string() << LogTag << strError5 << cnf.get()->name);

				active_device__ = device_list__.at(devid);

				isenable__ = midi_utils::Check_MMRESULT(
					[&]() -> MMRESULT {
						return midi_utils::Run_midiOutOpen(
							&midi_out_handle__,
							static_cast<UINT>(devid),
							(DWORD_PTR)&MidiOutProc,
							(DWORD_PTR)this
						);
					},
					LogTag
				);
				if (!isenable__)
					throw runtime_werror(log_string() << LogTag << strError7 << active_device__ << L"/" << devid);
				
				Common::to_log::Get() << (log_string() << LogTag << strError8 << active_device__ << L"/" << devid);
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
					return midi_utils::Run_midiOutShortMsg(midi_out_handle__, m.send) == S_OK;
				return (vmdev_ptr__) ? vmdev_ptr__.get()->SendToPort(m, t) : false;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}

		MidiControllerOut& MidiControllerOut::Get() {
			return std::ref(ctrl_midicontrollerout__);
		}
	}
}