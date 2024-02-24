/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace PLUGINS {

		#include "plugininfo.h"

		using namespace Common::IO;
		using namespace Common::SCRIPT;
		using namespace std::placeholders;

		VmScriptPlugin::VmScriptPlugin(std::wstring path, HWND hwnd)
			: vmscripts_(std::make_shared<VmScripts>(*static_cast<PluginCb*>(this))),
			  plugin_ui_(*static_cast<PluginCb*>(this), std::ref(vmscripts_), hwnd),
			  IO::Plugin(
				Utils::to_hash(path), DLG_PLUG_VMSCRIPT_WINDOW,
				Plugin::GuidFromString(_PLUGINGUID),
				path,
				PLUG_HEADER,
				PLUG_DESCRIPTION,
				this,
				IO::PluginClassTypes::ClassVmScript,
				(IO::PluginCbType::In2Cb | IO::PluginCbType::Out2Cb | IO::PluginCbType::LogCb | IO::PluginCbType::LogsCb | IO::PluginCbType::ConfCb),
				hwnd
			  ),
			id_(Utils::random_hash(this)) {

			PluginCb::out2_cb_ = std::bind(static_cast<void(VmScriptPlugin::*)(MIDI::MidiUnit&, DWORD)>(&VmScriptPlugin::cb_out_call_), this, _1, _2);
			PluginCb::cnf_cb_ = std::bind(static_cast<void(VmScriptPlugin::*)(std::shared_ptr<JSON::MMTConfig>&)>(&VmScriptPlugin::set_config_cb_), this, _1);
			
			vmscripts_->event_list_add(
				std::bind(static_cast<void(VmScriptPlugin::*)(scripts_list_t&)>(&VmScriptPlugin::build_export_list_), this, _1),
				id_
			);
			vmscripts_->event_run_add(
				std::bind(static_cast<void(VmScriptPlugin::*)(scripts_run_t&)>(&VmScriptPlugin::build_export_run_), this, _1),
				id_
			);
		}
		VmScriptPlugin::~VmScriptPlugin() {
			dispose_();
		}
		void VmScriptPlugin::dispose_() {
			TRACE_CALL();
			try {
				vmscripts_->reset();
			} catch (...) {}
		}
		void VmScriptPlugin::load_(SCRIPT::VmScriptConfig& sc) {
			is_config_ = !sc.empty();
			is_enable_ = (is_config_ && sc.enable);
			if (is_enable_)
				vmscripts_->set_config(sc);
		}

		#pragma region private call cb
		void VmScriptPlugin::set_config_cb_(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALL();
			try {
				if (!is_started_) {
					(void)load(mmt);
					#if defined (_DEBUG_CONFIG)
					::OutputDebugStringW(mmt->vmscript.dump().c_str());
					#endif
					return;
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}
		void VmScriptPlugin::cb_out_call_(MIDI::MidiUnit& m, DWORD t) {
			try {
				awaiter lock(std::ref(call_command_));
				if (!lock.lock_if(!started_.load(std::memory_order_acquire))) return;

				if (((m.type  != MIDI::MidiUnitType::BTN) &&
					 (m.type  != MIDI::MidiUnitType::BTNTOGGLE)) ||
					(m.target != MIDI::Mackie::VMSCRIPT)) return;

				vmscripts_->call_script(m.longtarget);

			} catch (...) {}
		}
		void VmScriptPlugin::build_export_list_(SCRIPT::scripts_list_t& list) {
			try {
				awaiter lock(std::ref(build_list_));
				if (!lock.lock_if()) return;

				auto f = std::async(std::launch::async, [&]() -> bool {

					lock.lock_wait(build_run_);

					if (!export_list_.empty())
						export_list_.erase(
							std::remove_if(
								export_list_.begin(),
								export_list_.end(),
								[](std::pair<uint16_t, std::wstring>& a) -> bool { return a.first == 0U; }
							)
						);

					auto& conf = common_config::Get().GetConfig();
					conf->vmscript.clear();
					for (auto& a : list) {
						auto w = a.namew();
						export_list_.push_back(
							std::make_pair(0, w)
						);
						conf->vmscript.add(w);
					}
					for (auto& u : conf->units) {
						if (u.target == MIDI::Mackie::Target::VMSCRIPT) {
							u.apps.clear();
							if (size_t idx = vmscripts_->script_index_selector(u.longtarget); idx < list.size())
								u.apps.push_back(list[idx].namew());
						}
					}
					return !export_list_.empty();
				});
				try {
					auto b = f.get();
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScriptPlugin::build_export_run_(SCRIPT::scripts_run_t& list) {
			try {
				awaiter lock(std::ref(build_run_));
				if (!lock.lock_if()) return;

				auto f = std::async(std::launch::async, [&]() -> bool {

					lock.lock_wait(build_list_);

					export_list_.erase(
						std::remove_if(
							export_list_.begin(),
							export_list_.end(),
							[](std::pair<uint16_t, std::wstring>& a) -> bool { return (a.first == 1U) || (a.first == 2U); }
						)
					);

					for (auto& a : list)
						export_list_.push_back(
							std::make_pair(a->isrun() ? uint16_t(1U) : uint16_t(2U), a->namew())
						);

					return !export_list_.empty();
				});
				try {
					auto b = f.get();
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region public actions
		bool VmScriptPlugin::load(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->empty()));
			try {
				load_(mmt->vmscript);
				if (is_enable_)
					vmscripts_->set_config(mmt);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return true;
		}
		bool VmScriptPlugin::load(std::wstring s) {
			TRACE_CALLX(s);
			try {
				if (s.empty()) return true;

				worker_background::Get().to_async(std::async(std::launch::async, [=](std::wstring p) {
					try {
						Common::SCRIPT::VmScriptConfig sc{};
						JSON::json_config jc{};

						is_enable_ = jc.ReadFile(p,
							[&](Tiny::TinyJson& root, std::wstring& path) -> bool {
								jc.ReadVmScriptConfig(root, sc);
								return true;
							}
						);
						if (is_enable_) load_(sc);
						if (is_config_ && is_enable_) {
							auto& list = vmscripts_->get_list();
							if (!list.empty()) {
								sc.clear();
								for (auto& a : list)
									sc.add(Utils::to_string(a.name));
							}
							common_config::Get().GetConfig()->vmscript.copy(sc);
						}
					} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
				}, s));

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return true;
		}

		bool VmScriptPlugin::start(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->vmscript.empty()));
			try {
				load_(mmt->vmscript);
				if (!is_config_ || !is_enable_) return false;

				vmscripts_->set_config(mmt);
				vmscripts_->start();
				is_started_ = vmscripts_->isrun();
				started_.store(is_started_, std::memory_order_release);

				if (!is_started_) {
					PluginCb::ToLogRef(log_string().to_log_format(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_SCRIPT_NOTFOUND),
						mmt->vmscript.script_directory));

					return false;
				}
				auto& list = vmscripts_->get_list();
				
				PluginCb::ToLogRef(log_string().to_log_format(__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_SCRIPT_START),
					list.size()));

				if (export_list_.empty())
					build_export_list_(list);

				return is_started_;

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
		bool VmScriptPlugin::stop() {
			TRACE_CALL();
			try {
				if (is_started_) {
					PluginCb::ToLogRef(log_string().to_log_string(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_SCRIPT_STOP)));

					vmscripts_->stop();
				}
				export_list_.clear();
				started_.store(false, std::memory_order_release);
				is_config_ = is_enable_ = is_started_ = false;

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return true;
		}
		void VmScriptPlugin::release() {
			TRACE_CALL();
			try {
				started_.store(false, std::memory_order_release);
				dispose_();
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}

		IO::PluginUi& VmScriptPlugin::GetPluginUi() {
			return std::ref(*plugin_ui_.GetPluginUi());
		}
		#pragma endregion

		std::vector<std::pair<uint16_t, std::wstring>>& VmScriptPlugin::GetDeviceList() {
			TRACE_CALL();
			return std::ref(export_list_);
		}
		std::any VmScriptPlugin::GetDeviceData() {
			std::any a = (int)42;
			return a;
		}
	}
}