/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {

	class FLAG_EXPORT timeout_waiter {
	private:
		const std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::nano>> end_;
	public:

		timeout_waiter(const int t); /* 't' is seconds */
		~timeout_waiter() = default;

		const bool check();
	};

	class FLAG_EXPORT awaiter {
	private:
		std::atomic<bool>& alock_;
		bool isunlock_;
	public:

		awaiter(std::atomic<bool>&);
		~awaiter();

		const bool lock_if();												/* self */
		const bool lock_if(const bool);										/* self + static input */
		void lock_wait();													/* self */
		void lock_wait(const std::atomic<bool>&);							/* input only */
		void lock_wait(const std::atomic<bool>&, const std::atomic<bool>&); /* input only */
		void lock();														/* self */
		void unlock();														/* self */

		template<typename T1 = bool>
		static inline const bool lock_if(std::atomic<T1>& a) {
			if (a.load(std::memory_order_acquire)) return false;
			a.store(true, std::memory_order_release);
			return true;
		}

		template<typename T1 = bool>
		static inline const bool lock_if(std::atomic<T1>& a, const T1 b) {
			if (a.load(std::memory_order_acquire) || b) return false;
			a.store(true, std::memory_order_release);
			return true;
		}

		template<typename T1 = bool>
		static inline void lock(std::atomic<T1>& a) {
			if (a.load(std::memory_order_acquire)) awaiter::lock_wait<15,10,bool>(a);
			a.store(true, std::memory_order_release);
		}

		template<typename T1 = bool>
		static inline void unlock(std::atomic<T1>& a) {
			if (a.load(std::memory_order_acquire))
				a.store(false, std::memory_order_release);
		}

		template<const int MILLIS = 10, const int TIMEOUT = 10, typename T1 = bool>
		static inline void lock_wait(const std::atomic<T1>& a) {
			timeout_waiter tmw(TIMEOUT);
			while (a.load(std::memory_order_acquire)) {
				std::this_thread::sleep_for(std::chrono::milliseconds(MILLIS));
				if (tmw.check())
					throw make_common_error(common_error_id::err_TIMEOUT_EXPIRED);
			}
		}

		template<const int MILLIS = 10, const int TIMEOUT = 10, typename T1 = bool>
		static inline void lock_wait(const std::atomic<T1>& a1, const std::atomic<T1>& a2) {
			timeout_waiter tmw(TIMEOUT);
			while (a1.load(std::memory_order_acquire) || a2.load(std::memory_order_acquire)) {
				std::this_thread::sleep_for(std::chrono::milliseconds(MILLIS));
				if (tmw.check())
					throw make_common_error(common_error_id::err_TIMEOUT_EXPIRED);
			}
		}
	};

	class FLAG_EXPORT locker_awaiter {
	private:
		#if defined(PX64__)
		volatile LONG64 cnt{ 0 };
		#elif defined(PX32__)
		volatile LONG cnt{ 0 };
		#endif
		std::atomic<bool> lock_{ false };
		std::atomic<bool> onlyone_{ false };
		std::atomic<bool> canceled_{ false };

		void wait_counter_();
		void wait_onlyone_();
		void wait_();

	public:

		locker_awaiter() = default;
		~locker_awaiter() = default;

		void Wait();
		void EndWait();
		void Lock();
		void UnLock();

		const bool IsLock() const;
		const bool IsCanceled() const;
		const bool IsOnlyOne() const;
		const bool LockOnlyOne();
		void WaitOnlyOne();

		void reset();

		/* Busy mode */

		void Begin();
		void End();
	};
}
