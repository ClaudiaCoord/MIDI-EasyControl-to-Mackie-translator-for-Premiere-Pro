/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session values/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MMTPLUGINxMIDIDEV - Virtual Midi proxy controller

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIDI {

		MidiControllerProxy::MidiControllerProxy(IO::PluginCb& cb, std::shared_ptr<MidiDriver>& drv, std::wstring dev)
			: MidiControllerBase(cb, drv, dev) {
			if (active_device_.empty())
				throw make_common_error(common_error_id::err_NOT_CONFIG);
		}
		MidiControllerProxy::~MidiControllerProxy() {
			dispose_();
		}
		void MidiControllerProxy::dispose_() {
			try {
				if (vmdev_ptr_.size() > 0) {
					for (auto& ptr : vmdev_ptr_) {
						if (ptr) {
							ptr.get()->Stop();
							ptr.reset();
						}
					}
					vmdev_ptr_.clear();
				}
				MidiDevices::Get().GetMidiProxyDeviceList().clear();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			isenable_ = isconnect_ = false;
		}

		void MidiControllerProxy::SetProxyCount(uint32_t u) {
			proxy_count_ = u;
		}
		const uint32_t MidiControllerProxy::GetProxyCount() {
			return proxy_count_;
		}
		const uint32_t MidiControllerProxy::GetRunningProxyCount() {
			return static_cast<uint32_t>(vmdev_ptr_.size());
		}

		const bool MidiControllerProxy::Start(std::shared_ptr<JSON::MMTConfig>& cnf) {
			return Start();
		}
		const bool MidiControllerProxy::Start() {
			try {
				if (active_device_.empty())
					throw make_common_error(log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_NOT_DEVICE_NAME),
						L"-", 0)
					);
				if (!proxy_count_)
					throw make_common_error(common_error_id::err_MIDI_NOT_CONFIG_PROXY);

				std::unique_lock<std::shared_mutex> lock(mtx_);

				if (isenable_ || isconnect_) dispose_();

				(void) ::GetLastError();
				vmdev_ptr_.clear();
				std::vector<std::wstring>& list = MidiDevices::Get().GetMidiProxyDeviceList();
				list.clear();

				for (uint32_t i = 0U; i < proxy_count_; i++) {
					try {
						std::wstring dev = active_device_ + std::to_wstring(i + 1);
						std::shared_ptr<MidiControllerVirtual> vmidi = std::make_unique<MidiControllerVirtual>(cb_, mdrv_, dev);

						if (!vmidi.get()->Start()) {
							log_string ls;
							cb_.ToLogRef(log_string().to_log_format(
								__FUNCTIONW__,
								common_error_code::Get().get_error(common_error_id::err_MIDI_DEVICE_NOT_CONNECT),
								dev, i
							));
							continue;
						}
						if (vmidi.get()->IsEnable()) {
							vmdev_ptr_.push_back(vmidi);
							list.push_back(dev);
						}
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						dispose_();
					}
				}
				isenable_ = isconnect_ = (vmdev_ptr_.size() > 0U);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				dispose_();
			}
			return isenable_;
		}
		void       MidiControllerProxy::Stop() {
			dispose_();
		}

		void MidiControllerProxy::SendToPort(Mackie::MIDIDATA& m, DWORD& t) {
			try {
				if (!isenable_ || vmdev_ptr_.empty()) return;

				for (auto& v : vmdev_ptr_)
					if (v) v->SendToPort(m, t);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void MidiControllerProxy::SendToPort(MIDI::MidiUnit&, DWORD&) {
		}
	}
}