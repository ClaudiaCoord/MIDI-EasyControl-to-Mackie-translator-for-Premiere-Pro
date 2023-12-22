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

namespace Common {

    locker_auto::locker_auto(std::shared_ptr<locker_awaiter>& a, locker_auto::LockType t) : aw(a), type(t) {
        (void) Begin(t == locker_auto::LockType::TypeLockOnlyOne);
    }
    locker_auto::~locker_auto() {
        End();
    }

    const bool locker_auto::IsCanceled() const {
        return aw->IsCanceled();
    }
    const bool locker_auto::IsOnlyOne() const {
        switch (type) {
            using enum locker_auto::LockType;
            case TypeWaitOnlyOne:
            case TypeLockOnlyOne: return aw->IsOnlyOne();
            case TypeLock:
            case TypeLockWait:
            default: return false;
        }
    }
    const bool locker_auto::Begin(bool noinit) {
        if (isbegin) return true;

        bool b{ true };
        switch (type) {
            using enum locker_auto::LockType;
            case TypeLockOnlyOne: {
                if (noinit) {
                    b = false;
                    break;
                }
                b = (aw->IsOnlyOne() || aw->IsCanceled()) ? false : aw->LockOnlyOne();
                break;
            }
            case TypeLock: {
                aw->Lock();
                break;
            }
            case TypeLockWait: {
                aw->Wait();
                break;
            }
            case TypeWaitOnlyOne: {
                aw->WaitOnlyOne();
                b = true;
                break;
            }
            default: {
                b = false;
                break;
            }
        }
        isbegin = b;
        return b;
    }
    void locker_auto::End() {
        if (isbegin) {
            isbegin = false;
            switch (type) {
                using enum locker_auto::LockType;
                case TypeLock:
                case TypeLockOnlyOne: {
                    aw->UnLock();
                    break;
                }
                case TypeLockWait:
                case TypeWaitOnlyOne: {
                    aw->EndWait();
                    break;
                }
                default: break;
            }
        }
    }
}