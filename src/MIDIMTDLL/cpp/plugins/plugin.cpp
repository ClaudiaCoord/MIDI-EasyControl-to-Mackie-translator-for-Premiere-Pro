/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023, MIT

	MIDIMMT DLL + MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace IO {

		CLSID Plugin::GuidFromString(const char* s) {
			CLSID clsid = Utils::guid_from_string(s);
			if (clsid == GUID_NULL)
				throw make_common_error(common_error_id::err_NOT_CLSID_CONVERT);
			return clsid;
		}

		PluginContainer::PluginContainer(Plugin& p) : plugin(p) {}
		PluginContainerDeleter::PluginContainerDeleter(PluginContainer* p) : plc(p) {}
		PluginContainerDeleter::~PluginContainerDeleter() {
			PluginContainer* p = plc;
			plc = nullptr;
			if (p) delete p;
		}

		Plugin::Plugin(uint32_t id, uint32_t did, GUID g, std::wstring p, std::wstring_view n, std::wstring_view d, void* clz, PluginClassTypes t, PluginCbType cbt)
			: plugin_info_(PluginInfo(id, did, g, p, n, d)), PluginCb::PluginCb(clz, t, cbt) {
		}
		Plugin::Plugin(uint32_t id, uint32_t did, GUID g, std::wstring p, std::string_view n, std::string_view d, void* clz, PluginClassTypes t, PluginCbType cbt)
			: plugin_info_(PluginInfo(id, did, g, p, Utils::to_string(n), Utils::to_string(d))), PluginCb::PluginCb(clz, t, cbt) {
		}

		const bool Plugin::configure() {
			return is_config_;
		}
		const bool Plugin::enabled() {
			return is_enable_;
		}
		const bool Plugin::started() {
			return is_started_;
		}

		IO::PluginCb& Plugin::GetPluginCb() {
			return std::ref(*static_cast<PluginCb*>(this));
		}
		IO::PluginInfo& Plugin::GetPluginInfo() {
			return std::ref(plugin_info_);
		}
	}
}
