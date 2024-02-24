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

#include <chrono>
#include <thread>
#include <atomic>

#if defined(PX64__)
#	define InterlockedIncrement__(A) InterlockedIncrement64(A)
#	define InterlockedDecrement__(A) InterlockedDecrement64(A)
#	define InterlockedExchange__(A,B) InterlockedExchange64(A,B)
#elif defined(PX32__)
#	define InterlockedIncrement__(A) InterlockedIncrement(A)
#	define InterlockedDecrement__(A) InterlockedDecrement(A)
#	define InterlockedExchange__(A,B) InterlockedExchange(A,B)
#else
#	error "Environment platform unknown: 32 or 64 bit."
#endif

namespace Common {

	#pragma region timeout_waiter
	timeout_waiter::timeout_waiter(const int t)
		: end_(std::chrono::steady_clock::now() + std::chrono::duration<double, std::milli>(t * 1000)) {
	}
	const bool timeout_waiter::check() {
		return (end_ <= std::chrono::steady_clock::now());
	}
	#pragma endregion

	#pragma region awaiter
	awaiter::awaiter(std::atomic<bool>& a) : alock_(a), isunlock_(!a.load(std::memory_order_acquire)) {
	}
	awaiter::~awaiter() {
		if (isunlock_)
			alock_.store(false, std::memory_order_release);
	}

	const bool awaiter::lock_if() {
		return (isunlock_ = awaiter::lock_if<bool>(alock_));
	}
	const bool awaiter::lock_if(const bool b) {
		return (isunlock_ = awaiter::lock_if<bool>(alock_, b));
	}
	void awaiter::lock_wait() {
		awaiter::lock_wait<15, 8, bool>(alock_);
	}
	void awaiter::lock_wait(const std::atomic<bool>& a) {
		awaiter::lock_wait<15, 8, bool>(a);
	}
	void awaiter::lock_wait(const std::atomic<bool>& a, const std::atomic<bool>& b) {
		awaiter::lock_wait<15, 8, bool>(a, b);
	}
	void awaiter::lock() {
		awaiter::lock<bool>(alock_);
	}
	void awaiter::unlock() {
		awaiter::unlock<bool>(alock_);
		isunlock_ = false;
	}
	#pragma endregion

	#pragma region locker_awaiter
	void locker_awaiter::wait_() {
		while (lock_.load(std::memory_order_acquire)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}
	}
	void locker_awaiter::wait_counter_() {
		while (lock_.load(std::memory_order_acquire) || (cnt != 0)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
	void locker_awaiter::wait_onlyone_() {
		while (onlyone_.load(std::memory_order_acquire) && (!canceled_.load(std::memory_order_acquire)) && (cnt != 0)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}
	}

	void locker_awaiter::Wait() {
		wait_();
		(void)InterlockedIncrement__(&cnt);
	}
	void locker_awaiter::WaitOnlyOne() {
		wait_onlyone_();
		(void)InterlockedIncrement__(&cnt);
	}
	void locker_awaiter::EndWait() {
		(void)InterlockedDecrement__(&cnt);
	}
	void locker_awaiter::Lock() {
		wait_counter_();
		lock_.store(true, std::memory_order_release);
	}
	const bool locker_awaiter::IsLock() const {
		return lock_.load(std::memory_order_acquire) || canceled_.load(std::memory_order_acquire);
	}
	const bool locker_awaiter::LockOnlyOne() {
		if (onlyone_.load(std::memory_order_acquire)) return false;
		wait_counter_();
		lock_.store(true, std::memory_order_release);
		onlyone_.store(true, std::memory_order_release);
		return true;
	}
	const bool locker_awaiter::IsOnlyOne() const {
		return onlyone_.load(std::memory_order_acquire);
	}
	const bool locker_awaiter::IsCanceled() const {
		return canceled_.load(std::memory_order_acquire);
	}

	void locker_awaiter::UnLock() {
		lock_.store(false, std::memory_order_release); 
		onlyone_.store(false, std::memory_order_release);
	}

	void locker_awaiter::reset() {
		canceled_.store(true, std::memory_order_release);
		lock_.store(false, std::memory_order_release);
		onlyone_.store(false, std::memory_order_release);
		(void)InterlockedExchange__(&cnt, 0);
	}

	/* Busy mode */

	void locker_awaiter::Begin() {
		lock_.store(true, std::memory_order_release);
	}
	void locker_awaiter::End() {
		lock_.store(false, std::memory_order_release);
	}
	#pragma endregion
}