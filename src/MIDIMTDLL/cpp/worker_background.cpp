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

	static std::atomic<bool> isregistred__ = false;
	static std::atomic<uint32_t> workerpackid__ = 0U;
	static std::queue<std::future<void>> asyncreq__;
	static std::shared_ptr<locker_awaiter> lock__ = std::make_shared<Common::locker_awaiter>();
	static worker_background worker_background__;

	#if defined(__SANITIZE_ADDRESS__)
	__declspec(no_sanitize_address)
	#endif
	static void repack_() {
		try {
			while (!asyncreq__.empty()) {
				try {
					std::future<void>& f = asyncreq__.front();
					if (f.valid()) {
						if (f.wait_for(std::chrono::milliseconds(200)) != std::future_status::ready) {
							try { (void)f.get(); } catch (...) {}
						}
					}
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				asyncreq__.pop();
			}
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}

	worker_background::worker_background() {
		t__.delay = 5;
		workerpackid__ = add(std::bind(&worker_background::pack_, this));
	}
	worker_background::~worker_background() {
		dispose_();
	}
	worker_background& worker_background::Get() {
		return std::ref(worker_background__);
	}

	void	 worker_background::dispose_() {
		try {
			if (t__.IsActive()) {
				t__.Stop();
				std::this_thread::sleep_for(std::chrono::milliseconds(150));
			}
			if (!list__.empty())
				list__.clear();
		} catch (...) {}
	}
	void     worker_background::worker_() {
		uint32_t id = UINT32_MAX;
		try {
			for (auto& a : list__) {
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
			locker_auto locker(lock__, locker_auto::LockType::TypeLockOnlyOne);
			if (!locker.Begin() || asyncreq__.empty()) return;

			repack_();
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}

	void     worker_background::start(uint32_t interval) {
		try {
			if (t__.IsActive())
				t__.Stop();
			interval = (interval == 0U) ? t__.delay.load() : interval;
			t__.SetInterval(interval, std::bind(&worker_background::worker_, this));
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}
	void     worker_background::stop() {
		t__.Stop();
	}
	void     worker_background::clear() {
		try {
			dispose_();
			workerpackid__ = add(std::bind(&worker_background::pack_, this));
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}
	uint32_t worker_background::set_interval(uint32_t sec) {
		uint32_t old = t__.delay.load();
		t__.delay = sec;
		return old;
	}

	uint32_t worker_background::add(callWorkerCb cb) {
		try {
			uint32_t id = Utils::random_hash(&cb);
			if (Utils::random_isvalid(id)) {
				list__.push_front(pairWorkerCb(id, cb));
				return id;
			}
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
		return 0U;
	}
	uint32_t worker_background::remove(uint32_t id) {
		try {
			list__.remove_if([=](pairWorkerCb& p) { return p.first == id; });
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
		return 0U;
	}
	void     worker_background::to_async(callFutureCb cb) {
		try {
			asyncreq__.push(std::move(cb));
		}
		catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}
}
