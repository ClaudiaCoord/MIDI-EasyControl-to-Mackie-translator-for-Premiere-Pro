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
        switch (type) {
            case locker_auto::LockType::TypeLock:
            case locker_auto::LockType::TypeLockWait: {
                Begin();
                break;
            }
            case locker_auto::LockType::TypeLockOnlyOne:
            default: break;
        }
    }
    locker_auto::~locker_auto() {
        if (isbegin) {
            switch (type) {
                case locker_auto::LockType::TypeLockOnlyOne:
                case locker_auto::LockType::TypeLock: {
                    aw->UnLock();
                    break;
                }
                case locker_auto::LockType::TypeLockWait: {
                    aw->EndWait();
                    break;
                }
                default: break;
            }
        }
    }

    const bool locker_auto::IsCanceled() const {
        return aw->IsCanceled();
    }
    const bool locker_auto::IsOnlyOne() const {
        switch (type) {
            case locker_auto::LockType::TypeLockOnlyOne:
                return aw->IsOnlyOne();
            case locker_auto::LockType::TypeLock:
            case locker_auto::LockType::TypeLockWait:
            default: return false;
        }
    }
    const bool locker_auto::Begin() {
        if (isbegin) return true;

        bool b = true;
        switch (type) {
            case locker_auto::LockType::TypeLockOnlyOne: {
                b = aw->LockOnlyOne();
                break;
            }
            case locker_auto::LockType::TypeLock: {
                aw->Lock();
                break;
            }
            case locker_auto::LockType::TypeLockWait: {
                aw->Wait();
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

}