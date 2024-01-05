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

		using namespace std::string_view_literals;

		typedef std::shared_ptr<bridge_out_event> event_out_t;
		typedef std::shared_ptr<bridge_in_event> event_in_t;
		typedef std::shared_ptr<bridge_sys> event_sys_t;

		typedef plugin_t (__cdecl* call_pluginopen)(std::wstring);

		template <typename T1>
		struct default_hmodule_deleter {
			void operator()(T1 h) {
				if (h != nullptr) ::FreeLibrary(h);
			}
		};

		#pragma region PluginObject
		class FLAG_EXPORT PluginObject {
		private:
			handle_ptr<HMODULE, default_hmodule_deleter<HMODULE>> handle_{};
			plugin_t plug_;

			void dispose_();
			void load_(std::wstring&);
			PluginObject(std::wstring&);

		public:

			static std::unique_ptr<PluginObject> Build(std::wstring);
			~PluginObject();

			void release();
			const bool empty() const;
			plugin_t& get();
			Plugin* plugin();
			HMODULE lib();
		};

		typedef std::unique_ptr<PluginObject> plugin_object_t;
		#pragma endregion

		#pragma region Plugins
		class FLAG_EXPORT Plugins {
		private:
			std::shared_mutex mtx_{};
			std::vector<plugin_object_t> plugins_{};

			void dispose_();

		public:

			static constexpr std::wstring_view PluginsRootDir = L"Plugins"sv;
			static constexpr std::wstring_view PluginsName = L"MMTPLUGINx"sv;
			static constexpr std::wstring_view PluginsExt = L".dll"sv;

			plugin_t& operator[] (GUID&);
			plugin_t& operator[] (uint16_t);
			plugin_t& operator[] (std::wstring);

			Plugins();
			~Plugins();

			const uint32_t init(event_sys_t&, event_in_t&, event_out_t&);
			const uint32_t load(std::shared_ptr<JSON::MMTConfig>&);
			const uint32_t stop();
			const uint32_t start();
			void release(event_sys_t&, event_in_t&, event_out_t&);

			const size_t count_in();
			const size_t count_out();
			const size_t count() const;

			const bool empty() const;
			const bool enabled();

			plugin_t& get_empty_plugin();
			plugin_t& get_plugin_index(uint16_t);

			template<typename T1, PluginTypeId T2>
			plugin_t& get_plugin(const T1& t) {
				try {
					if (plugins_.empty() || (T2 == PluginTypeId::BY_NONE)) return get_empty_plugin();
					std::unique_lock<std::shared_mutex> lock(mtx_);

					for (auto& obj : plugins_) {
						if (obj->empty()) continue;
						plugin_t& p = obj->get();
						if (!p) continue;
						IO::PluginInfo& pi = p.get()->GetPluginInfo();

						if constexpr (std::is_same_v<uint32_t, T1>) {
							if constexpr (T2 == PluginTypeId::BY_CONFIG_ID) {
								if (pi.ConfigId() == t) return p;
							}
							else if constexpr (T2 == PluginTypeId::BY_DIALOG_ID) {
								if (pi.DialogId() == t) return p;
							}
						}
						else if constexpr (std::is_same_v<std::wstring, T1>) {
							if constexpr (T2 == PluginTypeId::BY_NAME) {
								if (pi.Name()._Equal(t)) return p;
							}
							else if constexpr (T2 == PluginTypeId::BY_PATH) {
								if (pi.Path()._Equal(t)) return p;
							}
							else if constexpr (T2 == PluginTypeId::BY_FILE) {
								if (pi.Path().ends_with(t)) return p;
							}
						}
						else if constexpr (std::is_same_v<GUID, T1> && (T2 == PluginTypeId::BY_GUID)) {
							if (Utils::is_guid_equals(pi.Guid(), t)) return p;
						}
					}
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				return get_empty_plugin();
			}
		};

		typedef std::unique_ptr<Plugins> plugins_t;
		#pragma endregion
	}
}

