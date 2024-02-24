/*
	MIDI-MT is a high level application driver for USB MIDI control surface.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once
#include <queue>

namespace Common {
	namespace SCRIPT {

		using namespace std::string_view_literals;

		enum class QueueEnded : uint16_t {
			None = 0,
			Running,
			Ended,
			Error
		};

		enum PoolActions : uint32_t {
			ActionsDirectoryScan = 1001,
			ActionsPackErase = 1002,
			ActionsWatchInit = 1003,
			ActionsWatchReset = 1004,
			ActionsStop = 1005
		};

		class VmScriptsTask {
		private:
			const uint32_t id_;
			std::atomic<bool> script_scan_{ false };
			std::atomic<bool> script_pack_{ false };
			std::unique_ptr<common_filewatch> watcher_{};
			uint64_t watch_diff_{ 0U };
			scripts_list_t scripts_{};
			scripts_run_t running_{};
			common_event<scripts_list_event_t> event_scripts_list_{};
			common_event<scripts_run_event_t> event_scripts_run_{};
			std::function<void(const std::wstring&)> print_cb_ = [](const std::wstring&) {};
			std::function<void(const PoolActions)> command_cb_ = [](const PoolActions) {};

			void dispose_();
			void directory_event_(const filewatch_event&);
			void run_pack_clear_(std::unique_ptr<VmScript>&);

		public:

			VmScriptsTask(std::function<void(const std::wstring&)>, std::function<void(const PoolActions)>);
			VmScriptsTask(const VmScriptsTask&) = default;
			~VmScriptsTask();

			void reset();

			void run_pack_erase();
			QueueEnded run_pack(const uint32_t&);
			void directory_scan(const std::wstring&);

			const bool watcher_start(const std::wstring&);
			void watcher_stop();

			void event_clear();
			void event_list_send();
			void event_list_rescan(const std::wstring&);
			void event_list_add(scripts_list_event_t, const uint32_t);
			void event_list_remove(const uint32_t);

			void event_run_queue(std::unique_ptr<VmScript>);
			void event_run_send();
			void event_run_add(scripts_run_event_t, const uint32_t);
			void event_run_remove(const uint32_t);

			scripts_list_t& get_list();
			scripts_run_t& get_run();
			ScriptEntry& get_script(const size_t);
			std::wstring get_script_extension();

			const bool is_lock() const;
			const bool is_watcher_run() const;
			const bool is_list_run_empty() const;
			const size_t list_script_size() const;
		};
	}
}