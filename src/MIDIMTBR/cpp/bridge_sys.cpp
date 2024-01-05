/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	EASYCTRL9

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
    namespace IO {

		bridge_sys::bridge_sys() {
			PluginCb::id_ = Utils::random_hash(this);
			PluginCb::type_ = PluginClassTypes::ClassSys;
			PluginCb::using_ = (PluginCbType::LogCb | PluginCbType::LogsCb | PluginCbType::LogoCb | PluginCbType::ConfCb);

			PluginCb::log_cb_ = [=](std::wstring& s) {
				to_log::Get() << s;
			};
			PluginCb::logs_cb_ = [=](log_string& ls) {
				to_log::Get() << ls;
			};
		}
		bridge_sys::~bridge_sys() {}

		#pragma region PluginCbSet Interface
		void bridge_sys::SetCbConfig(PluginCb* cb) {
			callConfigCb_t f = cb->GetCbConfig();
			if (f) common_config::Get().add(f, cb->GetId());
		}
		void bridge_sys::RemoveCbConfig(uint32_t id) {
			common_config::Get().remove(id);
		}

		void bridge_sys::SetCbOutLog(PluginCb* cb) {
			callFromlog_t f = cb->GetCbLogOut();
			if (f) to_log::Get().add(f, cb->GetId());
		}
		void bridge_sys::RemoveCbOutLog(uint32_t id) {
			to_log::Get().remove(id);
		}
		#pragma endregion

		PluginCb& bridge_sys::GetCb() {
			return *static_cast<PluginCb*>(this);
		}
	}
}