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

		MidiControllerProxy::MidiControllerProxy(std::shared_ptr<MidiDriver> drv) : MidiControllerBase(drv) {
			this_type__ = ClassTypes::ClassVirtualMidi;
		}
		MidiControllerProxy::~MidiControllerProxy() {
			Dispose();
		}

		const bool MidiControllerProxy::Start(std::shared_ptr<MidiDevice>& cnf) {

			if (isenable__) Dispose();

			if ((!cnf) || (cnf.get()->name.empty()))
				throw_common_error(common_error_id::err_NOT_CONFIG);

			if (cnf.get()->proxy == 0U)
				throw_common_error(common_error_id::err_NOT_CONFIG_PROXY);
			
			(void) ::GetLastError();
			vmdev_ptr__.clear();
			device_list__.clear();

			uint32_t count = cnf.get()->proxy;
			for (uint32_t i = 0U; i < count; i++) {
				try {
					std::wstring dev = Utils::device_out_name(cnf.get()->name, MidiHelper::GetSuffixProxyOut() + std::to_wstring(i + 1));
					std::shared_ptr<MidiControllerVirtual> vmidi = std::make_unique<MidiControllerVirtual>(mdrv__, dev);
					if (!vmidi.get()->Start()) {
						to_log::Get() << log_string().to_log_fomat(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_DEVICE_NOT_CONNECT),
							dev, i
						);
						continue;
					}
					if (vmidi.get()->IsEnable()) {
						vmdev_ptr__.push_back(vmidi);
						device_list__.push_back(dev);
					}
				}
				catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					Dispose();
				}
			}
			isenable__ = (vmdev_ptr__.size() > 0U);
			return isenable__;
		}
		void MidiControllerProxy::Stop() {
			Dispose();
		}
		void MidiControllerProxy::Dispose() {
			try {
				if (vmdev_ptr__.size() > 0) {
					for (auto& ptr : vmdev_ptr__) {
						if ((ptr != nullptr) && (ptr)) {
							ptr.get()->Stop();
							ptr.reset();
						}
					}
					vmdev_ptr__.clear();
					device_list__.clear();
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			isenable__ = false;
		}

		const uint32_t MidiControllerProxy::GetProxyCount() {
			if (vmdev_ptr__.size() > 0)
				return static_cast<uint32_t>(vmdev_ptr__.size());
			return 0;
		}

		const bool MidiControllerProxy::SendToPort(Mackie::MIDIDATA& m, DWORD& t) {
			bool b = false;
			try {
				if (!isenable__ || !isconnect__ || vmdev_ptr__.empty()) return false;

				for (auto& ptr : vmdev_ptr__) {
					if (ptr) b = ptr.get()->SendToPort(m, t);
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return b;
		}
	}
}