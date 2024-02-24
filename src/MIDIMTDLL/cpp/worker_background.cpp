/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include <queue>

namespace Common {

	using namespace std::placeholders;

	#pragma region worker background local
	class wb_local {
	public:
		static std::atomic<bool> isregistred_;
		static std::atomic<uint32_t> workerpackid_;
		static std::queue<std::future<void>> asyncreq_;
		static std::shared_ptr<locker_awaiter> lock_;
		static worker_background worker_background_;
	};

	std::atomic<bool> wb_local::isregistred_{ false };
	std::atomic<uint32_t> wb_local::workerpackid_{ 0U };
	std::queue<std::future<void>> wb_local::asyncreq_{};
	std::shared_ptr<locker_awaiter> wb_local::lock_ = std::make_shared<Common::locker_awaiter>();
	worker_background wb_local::worker_background_{};

	#pragma endregion

	#if defined(__SANITIZE_ADDRESS__)
	__declspec(no_sanitize_address)
	#endif
	static void repack_() {
		try {
			while (!wb_local::asyncreq_.empty()) {
				try {
					std::future<void>& f = wb_local::asyncreq_.front();
					if (f.valid()) {
						if (f.wait_for(std::chrono::milliseconds(200)) != std::future_status::ready) {
							try { (void)f.get(); } catch (...) {
								#if defined(_DEBUG)
								Utils::get_exception(std::current_exception(), __FUNCTIONW__);
								#endif
							}
						}
					}
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				wb_local::asyncreq_.pop();
			}
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}

	worker_background::worker_background() {
		t_.delay = 5;
		wb_local::workerpackid_ = add(std::bind(&worker_background::pack_, this));
	}
	worker_background::~worker_background() {
		dispose_();
	}
	worker_background& worker_background::Get() {
		return std::ref(wb_local::worker_background_);
	}

	void	 worker_background::dispose_() {
		try {
			if (t_.IsActive()) {
				t_.Stop();
				std::this_thread::sleep_for(std::chrono::milliseconds(150));
			}
			if (!list_.empty())
				list_.clear();
		} catch (...) {}
	}
	void     worker_background::worker_() {
		uint32_t id = UINT32_MAX;
		try {
			for (auto& a : list_) {
				id = a.first;
				a.second();
			}
		}
		catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}
	void     worker_background::pack_() {
		try {
			locker_auto locker(wb_local::lock_, locker_auto::LockType::TypeLockOnlyOne);
			if (!locker.Begin() || wb_local::asyncreq_.empty()) return;

			repack_();
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}

	void     worker_background::start(uint32_t interval) {
		try {
			if (t_.IsActive())
				t_.Stop();
			interval = (interval == 0U) ? t_.delay.load() : interval;
			t_.SetInterval(interval, std::bind(&worker_background::worker_, this));
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}
	void     worker_background::stop() {
		t_.Stop();
	}
	void     worker_background::clear() {
		try {
			dispose_();
			wb_local::workerpackid_ = add(std::bind(&worker_background::pack_, this));
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}
	uint32_t worker_background::set_interval(uint32_t sec) {
		uint32_t old = t_.delay.load();
		t_.delay = sec;
		return old;
	}

	uint32_t worker_background::add(callWorkerCb cb) {
		try {
			uint32_t id = Utils::random_hash(&cb);
			if (Utils::random_isvalid(id)) {
				list_.push_front(pairWorkerCb(id, cb));
				return id;
			}
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
		return 0U;
	}
	uint32_t worker_background::remove(uint32_t id) {
		try {
			list_.remove_if([=](pairWorkerCb& p) { return p.first == id; });
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
		return 0U;
	}
	void     worker_background::to_async(callFutureCb cb) {
		try {
			wb_local::asyncreq_.push(std::move(cb));
		}
		catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}
}
