/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace IO {

		typedef std::function<uint32_t(std::wstring&)> callGetPid;

		enum class FixedPlugin : uint16_t {
			PLUG_MIDI = 0,
			PLUG_MMKEYS,
			PLUG_MQTT,
			PLUG_LIGHTS
		};

		class FLAG_EXPORT IOBridge {
		private:
			std::array<uint32_t, 4> state_{};
			hwnd_ptr<empty_deleter> mhwnd_{};
			plugins_t   plugins_;
			event_sys_t event_sys_;
			event_out_t event_out_;
			event_in_t  event_in_;

			void dispose_();

		public:

			plugin_t& operator[] (IO::FixedPlugin);
			plugin_t& operator[] (uint16_t);
			plugin_t& operator[] (std::wstring);

			IOBridge();
			~IOBridge();

			const bool Init(HWND = nullptr);
			const bool Start();
			const bool Stop();
			const bool Reload(HWND = nullptr);

			const bool IsLoaded() const;
			const bool IsStarted() const;
			const bool IsStoped() const;

			const bool   PluginEmpty() const;
			const bool   PluginEnabled() const;
			const size_t PluginCount();
			const size_t PluginCountIn();
			const size_t PluginCountOut();

			void SetCb(IO::PluginCb&);
			void UnSetCb(IO::PluginCb&);

			std::array<uint32_t, 4>& GetState();

			static IOBridge& Get();
			plugin_t& GetEmptyPlugin();

			template<typename T1, PluginTypeId T2 = PluginTypeId::BY_NONE>
			plugin_t& GetPlugin(T1& t) {
				return plugins_->get_plugin<T1,T2>(t);
			}
		};
	}
}

