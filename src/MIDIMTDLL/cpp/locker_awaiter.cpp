/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

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

	void locker_awaiter::wait_lock__() {
		while (lock__ || (cnt != 0)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
	void locker_awaiter::wait__() {
		while (lock__) {
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}
	}

	locker_awaiter::locker_awaiter() noexcept {}

	const bool locker_awaiter::IsLock() const {
		return lock__ || canceled__;
	}
	void locker_awaiter::Wait() noexcept {
		wait__();
		(void)InterlockedIncrement__(&cnt);
	}
	void locker_awaiter::EndWait() noexcept {
		(void)InterlockedDecrement__(&cnt);
	}
	void locker_awaiter::Lock() noexcept {
		wait_lock__();
		lock__ = true;
	}
	bool locker_awaiter::LockOnlyOne() noexcept {
		if (onlyone__) return false;
		wait_lock__();
		lock__ = onlyone__ = true;
		return true;
	}
	const bool locker_awaiter::IsOnlyOne() noexcept {
		return onlyone__;
	}
	void locker_awaiter::UnLock() noexcept {
		lock__ = onlyone__ = false;
	}

	const bool locker_awaiter::IsCanceled() noexcept {
		return canceled__;
	}

	void locker_awaiter::reset() {
		canceled__ = true;
		lock__ = onlyone__ = false;
		(void)InterlockedExchange__(&cnt, 0);
	}

	/* Busy mode */

	void locker_awaiter::Begin() {
		lock__ = true;
	}
	void locker_awaiter::End() {
		lock__ = false;
	}
}