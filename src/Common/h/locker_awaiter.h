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
		volatile LONG64 cnt = 0;
		#elif defined(PX32__)
		volatile LONG cnt = 0;
		#endif
		std::atomic<bool> lock__ = false;
		std::atomic<bool> onlyone__ = false;
		std::atomic<bool> canceled__ = false;

		void wait_lock__();
		void wait__();

	public:

		locker_awaiter() noexcept;

		const bool IsLock() const;
		void Wait() noexcept;
		void EndWait() noexcept;
		void Lock() noexcept;
		bool LockOnlyOne() noexcept;
		const bool IsOnlyOne() noexcept;
		void UnLock() noexcept;
		const bool IsCanceled() noexcept;
		void reset();

		/* Busy mode */

		void Begin();
		void End();
	};
}
