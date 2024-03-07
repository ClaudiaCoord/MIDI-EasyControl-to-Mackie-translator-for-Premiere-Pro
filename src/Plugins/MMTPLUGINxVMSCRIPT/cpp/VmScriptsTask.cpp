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
	namespace SCRIPT {

		using namespace std::placeholders;

		class NamesVmScriptsTask {
		public:
			static constexpr std::wstring_view extension = L".mmts"sv;
			#if defined (_DEBUG)
			static constexpr auto TRACE_HEADER = L"* VmScriptsTask -> ";
			#endif
		};

		VmScriptsTask::VmScriptsTask(std::function<void(const std::wstring&)> pcb, std::function<void(const PoolActions)> ccb)
			: print_cb_(pcb), command_cb_(ccb), id_(Utils::random_hash(this)) {
			watcher_ = std::make_unique<common_filewatch>();
			watcher_->event_add(
				std::bind(static_cast<void(VmScriptsTask::*)(const filewatch_event&)>(&VmScriptsTask::directory_event_), this, _1),
				id_
			);
		}
		VmScriptsTask::~VmScriptsTask() {
			TRACE_CALL();
			dispose_();
		}
		void VmScriptsTask::dispose_() {
			TRACE_CALL();
			reset();
		}

		void VmScriptsTask::reset() {
			TRACE_CALLD(NamesVmScriptsTask::TRACE_HEADER + L"reset reset.watch..");
			watcher_->stop();
			TRACE_CALLD(NamesVmScriptsTask::TRACE_HEADER + L"reset event.clear..");
			event_clear();
			TRACE_CALLD(NamesVmScriptsTask::TRACE_HEADER + L"reset run.pack.erase..");
			run_pack_erase();
			TRACE_CALLD(NamesVmScriptsTask::TRACE_HEADER + L"reset reset end");
		}

		#pragma region running scripts pack...
		QueueEnded VmScriptsTask::run_pack(const uint32_t& hash) {
			QueueEnded state{ QueueEnded::None };
			try {
				awaiter lock(std::ref(script_pack_));
				if (!lock.lock_if(script_scan_.load(std::memory_order_acquire))) return state;

				for (size_t i = 0; i < running_.size(); i++) {
					auto& a = running_[i];

					#if defined (_DEBUG_THREAD)
					log_string ls{};
					ls << TRACE_HEADER << L"\trun pack (" << i << L"/" << hash << L") -> isend:" << a->isend() << L", isrun:" << a->isrun() << L", name:[" << a->namew().c_str() << L"], hash:" << a->hash() << L"/" << hash << L", size:" << running_.size() << L"\n";
					TRACE_CALLX(ls.c_str());
					#endif

					if (a && (a->isend() || !a->isrun())) {
						state = (!hash) ? state : ((a->hash() == hash) ? QueueEnded::Ended : state);
						run_pack_clear_(a);
						i = (i > 0) ? (i - 1) : 0;
					}
					else if (a && hash && (a->hash() == hash)) {
						state = a->isend() ? QueueEnded::Ended : QueueEnded::Running;
						run_pack_clear_(a);
						i = (i > 0) ? (i - 1) : 0;
					}
				}

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				state = QueueEnded::Error;
			}
			return state;
		}
		void VmScriptsTask::run_pack_clear_(std::unique_ptr<VmScript>& a) {
			try {

				try { a->stop(); } catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}

				if (!running_.empty()) {
					auto i = std::remove_if(
					   running_.begin(),
					   running_.end(),
						[&](std::unique_ptr<VmScript>& aa) -> bool {
							return a->hash() == aa->hash();
						}
					);
					if (i != running_.end())
						running_.erase(i);
				}

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScriptsTask::run_pack_erase() {
			TRACE_CALL();
			try {

				if (running_.empty()) return;

				awaiter lock1(std::ref(script_pack_));
				awaiter lock2(std::ref(script_scan_));
				lock1.lock_wait(script_pack_, script_scan_);
				lock1.lock();
				lock2.lock();

				for (auto& a : running_)
					if (a && a->isrun())
						try { a->stop(); } catch (...) {}

				running_.clear();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region directory manage...
		void VmScriptsTask::directory_scan(const std::wstring& s) {
			TRACE_CALL();
			try {
				awaiter lock(std::ref(script_scan_));
				if (!lock.lock_if(!running_.empty())) return;
				lock.lock_wait(script_pack_);

				auto f = std::async(std::launch::async, [=]() -> bool {
					try {
						std::filesystem::path p(s);
						if (std::filesystem::exists(p)) {

							FILETIME ft{};
							::GetSystemTimeAsFileTime(&ft);
							uint64_t time = std::bit_cast<std::uint64_t>(ft);
							scripts_list_t list{};

							for (const auto& f : std::filesystem::directory_iterator(p)) {
								if (f.path().extension().wstring()._Equal(NamesVmScriptsTask::extension.data())) {
									try {
										std::ifstream fs(f.path().string(), std::ios::in | std::ios::binary | std::ios::ate);
										if (!fs.is_open()) continue;

										fs.imbue(std::locale(""));
										std::streampos size = fs.tellg();
										if (size == 0U) {
											fs.close();
											continue;
										}

										#pragma warning( push )
										#pragma warning( disable : 4244 )
										std::string txt(size, 0);
										#pragma warning( pop )

										fs.seekg(0, std::ios::beg);
										fs.read(txt.data(), size);
										fs.close();

										try {

											auto name = f.path().stem().string();
											uint32_t hash = static_cast<uint32_t>(std::hash<std::string>{}(name));

											list.push_back(
												ScriptEntry(name, std::move(txt), hash, time)
											);

										} catch (...) {
											Utils::get_exception(std::current_exception(), __FUNCTIONW__);
										}

									} catch (...) {
										Utils::get_exception(std::current_exception(), __FUNCTIONW__);
									}
								}
							}
							scripts_.assign(list.begin(), list.end());
						}
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
					return !scripts_.empty();
				});

				try {
					if (f.get()) {
						print_cb_(log_string().to_log_format(__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_SCRIPT_LOADED),
							scripts_.size()));

						event_list_send();
					}
					else
						print_cb_(log_string().to_log_format(__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_SCRIPT_NOTFOUND),
							s));

				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScriptsTask::directory_event_(const filewatch_event& ev) {
			TRACE_CALL();
			try {
				TRACE_CALLD(
					(log_string() << NamesVmScriptsTask::TRACE_HEADER
						<< L"watch event -> file: " << ev.name
						<< L", action: (" << ev.action << L") " << filewatch_event::ActionsString(ev.action)
						<< L", time: " << ev.time << L" / " << Utils::NANOSECONDS_to_string(ev.time) << L"\n").c_str()
				);
				switch (ev.action) {
					using enum filewatch_event::Actions;
					case FileRename:
					case FileChange:
					case FileCreate:
					case FileDeleted: {
						if (!ev.name.ends_with(NamesVmScriptsTask::extension.data())) return;
						break;
					}
					default: return;
				}

				std::filesystem::path p(ev.name);
				const std::string name = p.stem().string();

				auto entry = std::find_if(
					scripts_.begin(),
					scripts_.end(),
					[name](const ScriptEntry& a) -> bool {
						return a.name._Equal(name);
					}
				);
				if (entry != scripts_.end()) {
					ScriptEntry& e = reinterpret_cast<ScriptEntry&>(*entry);
					TRACE_CALLD(
						(log_string() << NamesVmScriptsTask::TRACE_HEADER
							<< L"watch event -> entry: " << e.name
							<< L", hash: " << e.hash
							<< L", time: " << Utils::NANOSECONDS_to_string(e.time) << L" / " << e.time << L"\n").c_str()
					);
					if ((ev.time - 100000) <= e.time) return;
					e.time = ev.time;
				}

				print_cb_(log_string().to_log_format(__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_SCRIPT_MODIFY),
					ev.path().filename().wstring()));

				command_cb_(PoolActions::ActionsDirectoryScan);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region watcher
		const bool VmScriptsTask::watcher_start(const std::wstring& s) {
			return watcher_->start(s);
		}
		void VmScriptsTask::watcher_stop() {
			watcher_->stop();
		}
		const bool VmScriptsTask::is_watcher_run() const {
			return watcher_->isrun();
		}
		#pragma endregion

		#pragma region events
		void VmScriptsTask::event_clear() {
			event_scripts_run_.clear();
			event_scripts_list_.clear();
		}
		void VmScriptsTask::event_list_rescan(const std::wstring& s) {
			TRACE_CALL();
			std::jthread(
				[=](std::stop_token) {
					directory_scan(s);
				}
			).detach();
		}
		void VmScriptsTask::event_list_send() {
			try {
				event_scripts_list_.send(std::ref(scripts_));
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScriptsTask::event_list_add(scripts_list_event_t f, const uint32_t id) {
			event_scripts_list_.add(f, id);
			try { if (!scripts_.empty()) f(std::ref(scripts_)); } catch (...) {}
		}
		void VmScriptsTask::event_list_remove(const uint32_t id) {
			event_scripts_list_.remove(id);
		}

		void VmScriptsTask::event_run_queue(std::unique_ptr<VmScript> vms) {
			try {
				running_.push_back(std::move(vms));
				event_run_send();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			
		}
		void VmScriptsTask::event_run_send() {
			try {
				event_scripts_run_.send(std::ref(running_));
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScriptsTask::event_run_add(scripts_run_event_t f, const uint32_t id) {
			event_scripts_run_.add(f, id);
			try { if (!running_.empty()) f(std::ref(running_)); } catch (...) {}
		}
		void VmScriptsTask::event_run_remove(const uint32_t id) {
			event_scripts_run_.remove(id);
		}
		#pragma endregion

		std::wstring VmScriptsTask::get_script_extension() {
			return NamesVmScriptsTask::extension.data();
		}
		scripts_list_t& VmScriptsTask::get_list() {
			return std::ref(scripts_);
		}
		scripts_run_t& VmScriptsTask::get_run() {
			return std::ref(running_);
		}
		ScriptEntry& VmScriptsTask::get_script(const size_t i) {
			return scripts_[i];
		}

		const size_t VmScriptsTask::list_script_size() const {
			return scripts_.size();
		}
		const bool VmScriptsTask::is_list_run_empty() const {
			return running_.empty();
		}
		const bool VmScriptsTask::is_lock() const {
			return script_scan_.load(std::memory_order_acquire) || script_pack_.load(std::memory_order_acquire);
		}
	}
}