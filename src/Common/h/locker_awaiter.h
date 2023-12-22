/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {

	class FLAG_EXPORT locker_awaiter
	{
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
