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
    class FLAG_EXPORT locker_auto
    {
    public:
        enum LockType : int {
            TypeLock = 0,
            TypeLockWait,
            TypeLockOnlyOne
        };
    private:
        std::shared_ptr<locker_awaiter> aw;
        LockType type;
        std::atomic<bool> isbegin = false;
    public:

        locker_auto() = delete;
        locker_auto(std::shared_ptr<locker_awaiter>& a, LockType t = LockType::TypeLockWait);
        ~locker_auto();
        const bool IsCanceled() const;
        const bool IsOnlyOne() const;
        const bool Begin();
    };
}