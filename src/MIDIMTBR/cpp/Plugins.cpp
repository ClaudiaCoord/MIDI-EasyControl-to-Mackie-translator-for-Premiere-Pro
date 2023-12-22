/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023, MIT

	MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace IO {

		using namespace std::placeholders;
		static plugin_t plugin_empty_{};

		/* Private static */

		static HMODULE get_dll(const wchar_t* s) {
			__try {
				return ::LoadLibraryExW(s, 0, LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
			} __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
		}

		#pragma region PluginObject
		std::unique_ptr<PluginObject> PluginObject::Build(std::wstring s) {
			return std::unique_ptr<PluginObject>(new PluginObject(s));
		}
		PluginObject::PluginObject(std::wstring& s) {
			load_(s);
		}
		PluginObject::~PluginObject() {
			dispose_();
		}

		void PluginObject::load_(std::wstring& s) {
			_set_se_translator(seh_exception_catch);
			try {
				handle_.reset(get_dll(s.c_str()));
				if (!handle_) throw make_common_error(common_error_id::err_PLUGIN_DLL_ERROR);

				void* f = ::GetProcAddress(handle_, "OpenPlugin");
				if (!f) throw make_common_error(common_error_id::err_PLUGIN_DLL_NOT_PLUGIN);

				call_pluginopen p = reinterpret_cast<call_pluginopen>(f);
				plug_ = p(s);
				if (!plug_) throw make_common_error(common_error_id::err_PLUGIN_DLL_NOT_PLUGIN);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void PluginObject::dispose_() {
			#if defined (_DEBUG)
				::OutputDebugStringW(L"\t* PluginObject call dispose..\n");
			#endif
			plug_.reset();
			handle_.reset();
		}
		void PluginObject::release() {
			dispose_();
		}

		const bool PluginObject::empty() const {
			return (!plug_) || (!handle_);
		}
		plugin_t& PluginObject::get() {
			return std::ref(plug_);
		}
		IO::Plugin* PluginObject::plugin() {
			return plug_.get();
		}
		HMODULE PluginObject::lib() {
			return handle_.get();
		}
		#pragma endregion

		#pragma region Plugins
		Plugins::Plugins() {
		}
		Plugins::~Plugins() {
			dispose_();
		}

		plugin_t& Plugins::operator[] (uint16_t idx) {
			return get_plugin_index(idx);
		}
		plugin_t& Plugins::operator[] (std::wstring s) {
			return get_plugin<std::wstring, PluginTypeId::BY_NAME>(s);
		}
		plugin_t& Plugins::operator[] (GUID& g) {
			return get_plugin<GUID, PluginTypeId::BY_GUID>(g);
		}

		void Plugins::dispose_() {
			#if defined (_DEBUG)
				::OutputDebugStringW((L"\t* Plugins (" + std::to_wstring(plugins_.size()) + L") call dispose..\n").c_str());
			#endif
			try {
				if (plugins_.empty()) return;
				std::unique_lock<std::shared_mutex> lock(mtx_);
				for (auto& obj : plugins_) obj.reset();
				plugins_.clear();
			} catch (...) {}
		}
		void Plugins::release(event_sys_t& ev_sys, event_in_t& ev_in, event_out_t& ev_out) {
			try {
				if (plugins_.empty()) return;
				std::unique_lock<std::shared_mutex> lock(mtx_);
				for (auto& obj : plugins_) {
					if (obj->empty()) continue;
					IO::PluginCb& pcb = obj->plugin()->GetPluginCb();
					if (pcb.empty()) continue;

					pcb.UnSetCb(ev_in->GetCb());
					pcb.UnSetCb(ev_out->GetCb());
					pcb.UnSetCb(ev_sys->GetCb());
				}
			} catch (...) {}
		}

		const uint32_t Plugins::init(event_sys_t& ev_sys, event_in_t& ev_in, event_out_t& ev_out) {
			_set_se_translator(seh_exception_catch);
			try {
				if (!plugins_.empty()) {
					release(ev_sys, ev_in, ev_out);
					dispose_();
				}
				std::unique_lock<std::shared_mutex> lock(mtx_);
				std::filesystem::path p(Utils::app_dir());
				if (p.empty()) return 0U;
				p.append(Plugins::PluginsRootDir);

				if (std::filesystem::exists(p)) {
					for (const auto& f : std::filesystem::directory_iterator(p)) {
						if (!f.path().extension().wstring()._Equal(Plugins::PluginsExt.data()) || 
							!f.path().filename().wstring().starts_with(Plugins::PluginsName.data())) continue;

						try {
							auto obj = PluginObject::Build(f.path().wstring());
							if (obj->empty()) continue;
							IO::PluginCb& pcb = obj->plugin()->GetPluginCb();
							if (pcb.empty()) continue;

							pcb.SetCb(ev_sys->GetCb());
							pcb.SetCb(ev_out->GetCb());
							pcb.SetCb(ev_in->GetCb());
							plugins_.push_back(std::move(obj));

						} catch (...) {
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
							continue;
						}
					}
				}
				return static_cast<uint32_t>(plugins_.size());
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return 0;
		}
		const uint32_t Plugins::load(std::shared_ptr<JSON::MMTConfig>& cnf) {
			try {
				if (plugins_.empty()) return 0U;

				uint32_t cnt{ 0U };

				std::unique_lock<std::shared_mutex> lock(mtx_);
				for (auto& obj : plugins_) {
					if (obj->empty()) continue;
					IO::Plugin* p = obj->plugin();
					if (!p) continue;

					(void) p->load(cnf);
					std::filesystem::path f(p->GetPluginInfo().Path());
					f = f.replace_extension(L".cnf");
					if (std::filesystem::exists(f))
						(void) p->load(f.wstring());

					if (p->enabled() && p->configure()) cnt++;
				}
				return cnt;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return 0U;
		}
		const uint32_t Plugins::stop() {
			try {
				if (plugins_.empty()) return 0U;

				uint32_t cnt{ 0U };

				std::unique_lock<std::shared_mutex> lock(mtx_);
				for (auto& obj : plugins_) {
					if (obj->empty()) continue;
					IO::Plugin* p = obj->plugin();

					if (p->started()) {
						(void) p->stop();
						if (!p->started()) cnt++;
					}
				}
				return cnt;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return 0U;
		}
		const uint32_t Plugins::start() {
			try {
				if (plugins_.empty()) return 0U;

				uint32_t cnt{ 0U };

				std::shared_ptr<JSON::MMTConfig>& cnf = common_config::Get().GetConfig();
				if (cnf->empty()) throw make_common_error(common_error_id::err_NOT_CONFIG);
				std::unique_lock<std::shared_mutex> lock(mtx_);

				for (auto& obj : plugins_) {
					if (obj->empty()) continue;
					IO::Plugin* p = obj->plugin();

					if (p->started()) (void) p->stop();
					if (p->start(cnf) && p->started()) cnt++;
					else (void) p->release();
				}
				return cnt;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return 0U;
		}

		const size_t Plugins::count() const {
			return plugins_.size();
		}
		const size_t Plugins::count_in() {
			size_t count = 0;
			try {
				if (plugins_.empty()) return count;
				std::unique_lock<std::shared_mutex> lock(mtx_);

				for (auto& obj : plugins_) {
					if (obj->empty()) continue;
					IO::Plugin* p = obj->plugin();

					if (p->enabled()) {
						IO::PluginCb& pcb = p->GetPluginCb();
						if (pcb.IsCbType(PluginCbType::In1Cb) || pcb.IsCbType(PluginCbType::In2Cb)) count++;
					} 
				}
			} catch (...) {}
			return count;
		}
		const size_t Plugins::count_out() {
			size_t count = 0;
			try {
				if (plugins_.empty()) return count;
				std::unique_lock<std::shared_mutex> lock(mtx_);

				for (auto& obj : plugins_) {
					if (obj->empty()) continue;
					IO::Plugin* p = obj->plugin();

					if (p->enabled()) {
						IO::PluginCb& pcb = p->GetPluginCb();
						if (pcb.IsCbType(PluginCbType::Out1Cb) || pcb.IsCbType(PluginCbType::Out2Cb)) count++;
					}
				}
			} catch (...) {}
			return count;
		}

		const bool   Plugins::empty() const {
			return plugins_.empty();
		}
		const bool   Plugins::enabled() {
			try {
				if (plugins_.empty()) return false;
				std::unique_lock<std::shared_mutex> lock(mtx_);

				for (auto& obj : plugins_) {
					if (obj->empty()) continue;
					IO::Plugin* p = obj->plugin();

					if (p->enabled() && p->configure()) return true;
				}
			} catch (...) {}
			return false;
		}

		plugin_t& Plugins::get_plugin_index(uint16_t idx) {
			if (idx >= plugins_.size()) return get_empty_plugin();
			return plugins_[idx]->get();
		}
		plugin_t& Plugins::get_empty_plugin() {
			return std::ref(plugin_empty_);
		}
		#pragma endregion
	}
}
