/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL + MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace IO {
		
		using namespace std::string_view_literals;
		class FLAG_EXPORT Plugin;

		class FLAG_EXPORT PluginContainer {
		public:
			Plugin& plugin;
			PluginContainer(Plugin&);
		};
		class FLAG_EXPORT PluginContainerDeleter {
		public:
			PluginContainer* plc;
			PluginContainerDeleter(PluginContainer*);
			~PluginContainerDeleter();
		};

		class FLAG_EXPORT Plugin : public PluginCb {
		protected:
			bool is_config_{ false }, is_enable_{ false }, is_started_{ false };
			hwnd_ptr<empty_deleter> mhwnd_{};
			PluginInfo plugin_info_;

		public:

			Plugin(uint32_t, uint32_t, GUID, std::wstring, std::wstring_view, std::wstring_view, void*, PluginClassTypes, PluginCbType, HWND = nullptr);
			Plugin(uint32_t, uint32_t, GUID, std::wstring, std::string_view, std::string_view, void*, PluginClassTypes, PluginCbType, HWND = nullptr);
			Plugin(const Plugin&) = default;
			virtual ~Plugin() = default;

			virtual bool load(std::shared_ptr<JSON::MMTConfig>&) = 0;
			virtual bool load(std::wstring) = 0;
			virtual bool start(std::shared_ptr<JSON::MMTConfig>&) = 0;
			virtual bool stop() = 0;
			virtual void release() = 0;
			virtual IO::PluginUi& GetPluginUi() = 0;

			virtual IO::export_list_t& GetDeviceList() = 0;
			virtual std::any GetDeviceData() = 0;

			const bool configure();
			const bool enabled();
			const bool started();

			IO::PluginCb& GetPluginCb();
			IO::PluginInfo& GetPluginInfo();
			HWND GetMainHWWND();

			static CLSID GuidFromString(const char*);
		};

		typedef std::shared_ptr<Common::IO::Plugin> plugin_t;
	}
}

