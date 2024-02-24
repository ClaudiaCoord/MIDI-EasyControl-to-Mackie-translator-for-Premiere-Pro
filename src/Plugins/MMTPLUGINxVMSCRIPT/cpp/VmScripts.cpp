/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include "../../shared/h/chaiscript/extras/math.hpp"
#include "../../shared/h/chaiscript/extras/string_methods.hpp"
#include "../../shared/h/chaiscript/extras/wstring_methods.hpp"

namespace Common {
	namespace SCRIPT {

		using namespace std::placeholders;

		class NamesVmScripts {
		public:
			static constexpr std::wstring_view empty = L"- empty -"sv;
			static constexpr std::wstring_view directory = L"scripts"sv;
			#if defined (_DEBUG)
			static constexpr auto TRACE_HEADER = L"* VmScripts -> ";
			#endif
		};

		VmScripts::VmScripts(IO::PluginCb& pcb)
			: pcb_(pcb), mmt_(common_config::Get().GetConfig()) {
			ActionConstant::SetUpdateCb(
				std::bind(static_cast<void(VmScripts::*)(const uint8_t, const uint8_t, const uint8_t)>(&VmScripts::call_update), this, _1, _2, _3)
			);
			vmscriptstask_ = std::make_unique<VmScriptsTask>(
				std::bind(static_cast<void(VmScripts::*)(const std::wstring&)>(&VmScripts::puts), this, _1),
				std::bind(static_cast<void(VmScripts::*)(const PoolActions)>(&VmScripts::queue_task_command), this, _1)
			);
		}
		VmScripts::~VmScripts() {
			TRACE_CALL();
			ActionConstant::SetUpdateCb(nullptr);
			dispose_();
		}
		void VmScripts::dispose_() {
			TRACE_CALL();
			try {

				TRACE_CALLD(NamesVmScripts::TRACE_HEADER + L"dispose begin..");

				if (task_.joinable()) {
					TRACE_CALLD(NamesVmScripts::TRACE_HEADER + L"dispose task.joinable begin..");

					script_pack_queue_.push(PoolActions::ActionsStop);
					std::stop_source ssource = task_.get_stop_source();

					if (ssource.stop_possible())
						ssource.request_stop();
					else if (auto h = task_.native_handle(); h)
						#pragma warning( push )
						#pragma warning( disable : 6258 )
						::TerminateThread(h, 1);
						#pragma warning( pop )

					TRACE_CALLD(NamesVmScripts::TRACE_HEADER + L"dispose task.join begin..");
					task_.join();
					TRACE_CALLD(NamesVmScripts::TRACE_HEADER + L"dispose task.join end");

				} else {

					TRACE_CALLD(NamesVmScripts::TRACE_HEADER + L"dispose scripts.task.reset begin..");
					vmscriptstask_->reset();
					TRACE_CALLD(NamesVmScripts::TRACE_HEADER + L"dispose scripts.task.reset end");
				}

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			TRACE_CALLD(NamesVmScripts::TRACE_HEADER + L"dispose all end");
		}

		#pragma region scripts manage...
		size_t VmScripts::script_index_selector(const MIDI::Mackie::Target& t) {
			switch (t) {
				using enum MIDI::Mackie::Target;
				case B11: return 0;
				case B12: return 1;
				case B13: return 2;
				case B14: return 3;
				case B15: return 4;
				case B16: return 5;
				case B17: return 6;
				case B18: return 7;
				case B19: return 8;
				case B21: return 9;
				case B22: return 10;
				case B23: return 11;
				case B24: return 12;
				case B25: return 13;
				case B26: return 14;
				case B27: return 15;
				case B28: return 16;
				case B29: return 17;
				case B31: return 18;
				case B32: return 19;
				case B33: return 20;
				case B34: return 21;
				case B35: return 22;
				case B36: return 23;
				case B37: return 24;
				case B38: return 25;
				case B39: return 26;
				default: return UINT32_MAX;
			}
		}
		std::shared_ptr<chaiscript::ChaiScript> VmScripts::script_init_() {

			try {
				std::shared_ptr<chaiscript::ChaiScript> cs = std::make_shared<chaiscript::ChaiScript>();
				auto bs = ScriptBootstrap::script_module();
				if (bs) {
					script_bootstrap_(bs);
					cs->add(bs);
					ScriptBootstrap::script_enum_unittype(cs);
					ScriptBootstrap::script_enum_clicktype(cs);
					ScriptBootstrap::script_enum_colorgroups(cs);
					ScriptBootstrap::script_enum_mackietarget(cs);
					cs->add(chaiscript::extras::math::bootstrap());
					cs->add(chaiscript::extras::string_methods::bootstrap());
					cs->add(chaiscript::extras::wstring_methods::bootstrap());

					/*
						add values from class VmScript:
							"IsCanceled" = bool;
							"ScriptName" = std::string;
					*/

					return cs;
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return std::shared_ptr<chaiscript::ChaiScript>();
		}
		void VmScripts::script_bootstrap_(std::shared_ptr<chaiscript::Module>& bmod) {
			try {

				/* print */
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const MIDI::MidiUnit&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const MIDI::MidiUnitValue&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const MIDI::Mackie::Target&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const UnitDef&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const RGBWColor&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const chaiscript::Type_Info&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const std::vector<MIDI::MidiUnit>&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const std::string&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const int32_t&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const int16_t&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const int8_t&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const uint32_t&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const uint16_t&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const uint8_t&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const double&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const float&)>(&VmScripts::print), this), "print");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const bool&)>(&VmScripts::print), this), "print");

				/* puts */
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const std::string&)>(&VmScripts::puts), this), "puts");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const std::string_view&)>(&VmScripts::puts), this), "puts");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const std::wstring&)>(&VmScripts::puts), this), "puts");
				bmod->add(chaiscript::fun(static_cast<void(VmScripts::*)(const std::wstring_view&)>(&VmScripts::puts), this), "puts");

				/* callback */
				bmod->add(chaiscript::fun(static_cast<std::vector<MIDI::MidiUnit>&(VmScripts::*)()>(&VmScripts::units_), this), "MidiUnitsList");
				bmod->add(chaiscript::fun([=](const uint8_t s, const uint8_t k, const uint8_t v) -> void { call_update(s, k, v); }), "UpdateControl");
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region scripts call...
		void VmScripts::wait_script_() {
			while (vmscriptstask_->is_lock()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}
		void VmScripts::call_script(const MIDI::Mackie::Target& t) {
			TRACE_CALL();
			try {
				size_t idx = script_index_selector(t);
				if (vmscriptstask_->list_script_size() <= idx) return;

				wait_script_();

				auto& data = vmscriptstask_->get_script(idx);

				QueueEnded state = vmscriptstask_->run_pack(static_cast<uint32_t>(std::hash<std::string>{}(data.name)));
				if (state == QueueEnded::Running) {
					vmscriptstask_->event_run_send();
					return;
				}
				std::unique_ptr<VmScript> vms = std::make_unique<VmScript>(
					data,
					script_init_(),
					std::bind(static_cast<void(VmScripts::*)(const uint32_t)>(&VmScripts::queue_task_command), this, _1),
					std::bind(static_cast<void(VmScripts::*)(const std::wstring&)>(&VmScripts::puts), this, _1)
				);
				if (vms->start())
					vmscriptstask_->event_run_queue(std::move(vms));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScripts::call_update(const uint8_t scene, const uint8_t key, const uint8_t val) {
			try {
				auto f = pcb_.GetCbIn2();
				if (!f) return;

				MIDI::Mackie::MIDIDATA m{};
				m.Set(scene, key, val);

				DWORD id = send_count_.load(std::memory_order_acquire);
				id = (id == (UINT_MAX - 1)) ? 1U : (id + 1);
				send_count_.store(id, std::memory_order_release);
				f(m, id);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region scripts start/stop/reset/rescan
		void VmScripts::start_() {
			TRACE_CALL();
			try {
				if (task_.joinable()) return;

				while (!script_pack_queue_.empty())
					script_pack_queue_.pop();

				task_ = std::jthread(
					[=](std::stop_token stoken) {
						try {
							bool is_directory_scan{ false };
							TRACE_CALLD(NamesVmScripts::TRACE_HEADER + L"master task started..");

							while (!stoken.stop_requested()) {
								if (!script_pack_queue_.empty()) {
									const uint32_t& hash = script_pack_queue_.front();
										 if (hash == PoolActions::ActionsDirectoryScan) is_directory_scan = true;
									else if (hash == PoolActions::ActionsPackErase) vmscriptstask_->run_pack_erase();
									else if (hash == PoolActions::ActionsWatchInit) vmscriptstask_->watcher_start(config_().script_directory);
									else if (hash == PoolActions::ActionsWatchReset) vmscriptstask_->watcher_stop();
									else if (hash == PoolActions::ActionsStop) break;
									else if (!vmscriptstask_->is_list_run_empty()) vmscriptstask_->run_pack(hash);
									script_pack_queue_.pop();
								}
								if (is_directory_scan && vmscriptstask_->is_list_run_empty()) {
									is_directory_scan = false;
									vmscriptstask_->directory_scan(config_().script_directory);
								}
								std::this_thread::sleep_for(std::chrono::milliseconds(250));
								#if defined (_DEBUG_SCRIPT_LOOP)
								TRACE_CALLD(
									(log_string() << NamesVmScripts::TRACE_HEADER
										<< L"master task +++ (stop.req:" << std::boolalpha << stoken.stop_requested() << L")").c_str()
								);
								#endif
							}

							TRACE_CALLD(NamesVmScripts::TRACE_HEADER + L"master task go run.pack.erase..");
							vmscriptstask_->run_pack_erase();

							TRACE_CALLD(NamesVmScripts::TRACE_HEADER + L"master task go reset watch..");
							vmscriptstask_->watcher_stop();

							TRACE_CALLD(NamesVmScripts::TRACE_HEADER + L"master task ended..");
						} catch (...) {
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						}
						TRACE_CALLD(NamesVmScripts::TRACE_HEADER + L"master task end final");
					}
				);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScripts::stop() {
			TRACE_CALL();
			if (task_.joinable()) {
				script_pack_queue_.push(PoolActions::ActionsWatchReset);
				script_pack_queue_.push(PoolActions::ActionsPackErase);
				script_pack_queue_.push(PoolActions::ActionsStop);
			} else vmscriptstask_->reset();
		}
		void VmScripts::start() {
			TRACE_CALL();
			try {
				std::tuple<bool, bool, bool> t = config_check_();
				if (pcb_.empty() || !std::get<0>(t)) {
					if (!std::get<1>(t) && vmscriptstask_->is_watcher_run()) {
						if (task_.joinable())
							queue_task_command(PoolActions::ActionsWatchReset);
						else vmscriptstask_->watcher_stop();
					}
					if (task_.joinable()) {
						queue_task_command(PoolActions::ActionsPackErase);
						queue_task_command(PoolActions::ActionsStop);
					} else vmscriptstask_->run_pack_erase();
					return;
				}
				if (!std::get<2>(t))
					config_().script_directory = get_default_directory_();

				start_();

				queue_task_command(PoolActions::ActionsWatchInit);
				queue_task_command(PoolActions::ActionsDirectoryScan);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScripts::start(const std::wstring& s, const bool b) {
			TRACE_CALL();
			try {
				std::tuple<bool, bool, bool> t = config_check_(s, b);

				if (pcb_.empty() || !std::get<0>(t)) {
					if (std::get<2>(t))
						config_().script_directory = s;
					if (std::get<1>(t))
						config_().script_watch = b;
					if (!b && vmscriptstask_->is_watcher_run()) {
						if (task_.joinable()) queue_task_command(PoolActions::ActionsWatchReset);
						else vmscriptstask_->watcher_stop();
					}
					if (pcb_.empty()) stop();
					return;
				}

				if (std::get<2>(t))
					config_().script_directory = s;

				if (std::get<1>(t))
					config_().script_watch = b;

				if (std::get<1>(t) || std::get<2>(t) || (b && !vmscriptstask_->is_watcher_run()))
					queue_task_command(PoolActions::ActionsWatchInit);

				if (std::get<2>(t))
					queue_task_command(PoolActions::ActionsDirectoryScan);

				if (!task_.joinable()) start_();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScripts::rescan() {
			TRACE_CALL();
			try {
				queue_task_command(PoolActions::ActionsDirectoryScan);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScripts::reset() {
			TRACE_CALL();
			vmscriptstask_->event_clear();
			dispose_();
		}
		#pragma endregion

		#pragma region events
		void VmScripts::event_list() {
			TRACE_CALL();
			if (!isrun()) vmscriptstask_->event_list_rescan(config_().script_directory);
			else queue_task_command(PoolActions::ActionsDirectoryScan);
		}

		void VmScripts::event_list_add(scripts_list_event_t f, const uint32_t id) {
			vmscriptstask_->event_list_add(f, id);
		}
		void VmScripts::event_list_remove(const uint32_t id) {
			vmscriptstask_->event_list_remove(id);
		}

		void VmScripts::event_run_add(scripts_run_event_t f, const uint32_t id) {
			vmscriptstask_->event_run_add(f, id);
		}
		void VmScripts::event_run_remove(const uint32_t id) {
			vmscriptstask_->event_run_remove(id);
		}
		#pragma endregion

		#pragma region get/set configs...
		SCRIPT::VmScriptConfig& VmScripts::config_() {
			return static_config_.empty() ? std::ref(mmt_->vmscript) : std::ref(static_config_);
		}
		std::vector<MIDI::MidiUnit>& VmScripts::units_() {
			return std::ref(mmt_->units);
		}
		std::tuple<bool, bool, bool> VmScripts::config_check_() {
			return std::make_tuple(
				config_().enable,
				config_().script_watch,
				!config_().script_directory.empty()
			);
		}
		std::tuple<bool, bool, bool> VmScripts::config_check_(const std::wstring& s, const bool b) {
			return std::make_tuple(
				config_().enable,
				(config_().script_watch != b),
				(!s.empty() && !config_().script_directory._Equal(s))
			);
		}

		void VmScripts::set_config(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALL();
			try {
				mmt_ = mmt;

				if (mmt_->empty() || !config_().enable) return;
				if (mmt_->vmscript.script_directory.empty())
					mmt_->vmscript.script_directory = get_default_directory_();

				if (config_().script_directory.empty()) return;

				if (isrun()) {
					queue_task_command(PoolActions::ActionsWatchInit);
					queue_task_command(PoolActions::ActionsDirectoryScan);
				}

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScripts::set_config(SCRIPT::VmScriptConfig& cnf) {
			TRACE_CALL();
			static_config_.copy(cnf);
		}
		std::wstring VmScripts::get_default_directory_() {
			try {
				std::filesystem::path p(Utils::app_dir(NamesVmScripts::directory, nullptr));
				if (!std::filesystem::exists(p)) {
					if (std::filesystem::create_directories(p))
						return p.wstring();
				}
				else if (std::filesystem::is_directory(p))
					return p.wstring();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return std::wstring();
		}
		std::wstring VmScripts::get_script_extension() {
			return vmscriptstask_->get_script_extension();
		}
		SCRIPT::VmScriptConfig& VmScripts::get_config() {
			return config_();
		}
		scripts_list_t& VmScripts::get_list() {
			return vmscriptstask_->get_list();
		}
		scripts_run_t& VmScripts::get_run() {
			return vmscriptstask_->get_run();
		}
		#pragma endregion

		const bool VmScripts::isrun() const {
			return task_.joinable();
		}

		void VmScripts::queue_task_command(const uint32_t hash) {
			script_pack_queue_.push(hash);
		}
		void VmScripts::queue_task_command(const PoolActions action) {
			script_pack_queue_.push(static_cast<uint32_t>(action));
		}

		#pragma region template puts/print
		void VmScripts::script_puts_(const std::wstring& s) {
			if (!pcb_.empty())
				pcb_.ToLogRef<std::wstring>(s);
		}

		template <typename T>
		void VmScripts::puts(const T& s) {
			if constexpr ((std::is_same_v<std::wstring, T>) ||
						  (std::is_same_v<std::wstring_view, T>)) {
				script_puts_(std::wstring(s));
			}
			else if constexpr ((std::is_same_v<std::string, T>) ||
							   (std::is_same_v<std::string_view, T>)) {
				script_puts_(Utils::to_string(s));
			}
			else if constexpr (std::is_same_v<log_string, T>) {
				script_puts_(s.str());
			}
		}

		template <typename T>
		void VmScripts::print(const T& a) {
			log_string ls{};
			if constexpr (std::is_same_v<std::string, T>)
				ls << Utils::to_string(a);
			else if constexpr (std::is_same_v<std::wstring, T>)
				ls << a.c_str();
			else if constexpr (std::is_same_v<UnitDef, T>) {
				if (a.empty()) ls << NamesVmScripts::empty.data();
				else ls << L"Unit() ->\n" << ((T&)a).dump();
			}
			else if constexpr (std::is_same_v<RGBWColor, T>) {
				if (a.empty()) ls << NamesVmScripts::empty.data();
				else ls << L"RGBW() ->\n" << ((T&)a).dump();
			}
			else if constexpr (std::is_same_v<MIDI::MidiUnit, T>) {
				if (a.empty()) ls << NamesVmScripts::empty.data();
				else ls << L"MidiUnit() ->\n" << ((T&)a).dump();
			}
			else if constexpr (std::is_same_v<MIDI::MidiUnitValue, T>) {
				if (a.empty()) ls << NamesVmScripts::empty.data();
				else ls << L"UnitValue() ->\n" << ((T&)a).dump();
			}
			else if constexpr (std::is_same_v<chaiscript::Type_Info, T>) {
				ls << a.name() << L", [ "
					<< (a.is_arithmetic() ? L"arithmetic | " : L"")
					<< (a.is_undef() ? L"undef | " : L"")
					<< (a.is_const() ? L"const | " : L"")
					<< (a.is_pointer() ? L"pointer | " : L"")
					<< (a.is_reference() ? L"reference | " : L"")
					<< (a.is_void() ? L"void" : L"")
					<< L" ]";
			}
			else if constexpr (std::is_same_v<std::vector<MIDI::MidiUnit>, T>) {

				if (a.empty()) ls << NamesVmScripts::empty.data();
				else {
					ls << L"MidiUnits() ->\n";
					for (auto& u : a)
						ls << static_cast<uint16_t>(u.scene) << L"/" << static_cast<uint16_t>(u.key) << L"-"
						   << static_cast<uint16_t>(u.longtarget) << L"/" << static_cast<uint16_t>(u.value.value) << L", ";
					ls.seekp(-2, std::ios_base::end);
				}
			}
			else if constexpr (std::is_same_v<MIDI::Mackie::Target, T>)
				ls << L"TargetIndex -> " << static_cast<uint16_t>(a) << L"|" << MIDI::MackieHelper::GetTarget(a);

			else if constexpr (std::is_same_v<MIDI::Mackie::ClickType, T>)
				ls << L"ClickTypeIndex -> " << static_cast<uint16_t>(a) << L"|" << MIDI::MackieHelper::GetClickType(a);

			else if constexpr (std::is_same_v<MIDI::MidiUnitType, T>)
				ls << L"UnitTypeIndex -> " << static_cast<uint16_t>(a) << L"|" << MIDI::MidiHelper::GetType(a);

			else if constexpr (std::is_same_v<SCRIPT::ColorGroup, T>)
				ls << L"ColorIndex -> " << static_cast<uint16_t>(a) << L"|" << ColorConstant::ColorHelper(a);

			else if constexpr (std::is_same_v<bool, T>)
				ls << std::boolalpha << a;

			else if constexpr ((std::is_same_v<int32_t, T>) ||
				(std::is_same_v<int32_t, T>) ||
				(std::is_same_v<int16_t, T>) ||
				(std::is_same_v<uint32_t, T>) ||
				(std::is_same_v<uint16_t, T>) ||
				(std::is_same_v<size_t, T>) ||
				(std::is_same_v<double, T>) ||
				(std::is_same_v<float, T>))
				ls << a;

			else if constexpr ((std::is_same_v<int8_t, T>) ||
				(std::is_same_v<uint8_t, T>))
				ls << static_cast<uint16_t>(a);

			if (!ls.emptyb()) script_puts_(ls.str());
		}

		template void VmScripts::puts<std::string>(const std::string&);
		template void VmScripts::puts<std::string_view>(const std::string_view&);
		template void VmScripts::puts<std::wstring>(const std::wstring&);
		template void VmScripts::puts<std::wstring_view>(const std::wstring_view&);
		template void VmScripts::puts<log_string>(const log_string&);

		template void VmScripts::print<MIDI::MidiUnit>(const MIDI::MidiUnit&);
		template void VmScripts::print<MIDI::MidiUnitValue>(const MIDI::MidiUnitValue&);
		template void VmScripts::print<MIDI::Mackie::Target>(const MIDI::Mackie::Target&);
		template void VmScripts::print<MIDI::Mackie::ClickType>(const MIDI::Mackie::ClickType&);
		template void VmScripts::print<MIDI::MidiUnitType>(const MIDI::MidiUnitType&);
		template void VmScripts::print<SCRIPT::ColorGroup>(const SCRIPT::ColorGroup&);
		template void VmScripts::print<SCRIPT::UnitDef>(const SCRIPT::UnitDef&);
		template void VmScripts::print<SCRIPT::RGBWColor>(const SCRIPT::RGBWColor&);

		template void VmScripts::print<std::string>(const std::string&);
		template void VmScripts::print<std::wstring>(const std::wstring&);
		template void VmScripts::print<chaiscript::Type_Info>(const chaiscript::Type_Info&);
		template void VmScripts::print<std::vector<MIDI::MidiUnit>>(const std::vector<MIDI::MidiUnit>&);

		template void VmScripts::print<int32_t>(const  int32_t&);
		template void VmScripts::print<int16_t>(const  int16_t&);
		template void VmScripts::print<int8_t>(const   int8_t&);
		template void VmScripts::print<uint32_t>(const uint32_t&);
		template void VmScripts::print<uint16_t>(const uint16_t&);
		template void VmScripts::print<uint8_t>(const  uint8_t&);
		template void VmScripts::print<double>(const   double&);
		template void VmScripts::print<float>(const    float&);
		template void VmScripts::print<size_t>(const   size_t&);
		template void VmScripts::print<bool>(const     bool&);
		#pragma endregion
	}
}