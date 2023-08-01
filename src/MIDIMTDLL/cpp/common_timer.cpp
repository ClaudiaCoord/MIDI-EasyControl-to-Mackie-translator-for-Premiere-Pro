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

    void common_timer::SetTimeout(int delay, std::function<void()> f) {
        active__ = true;
        std::thread t([=]() {
            if (!active__.load()) return;
            std::this_thread::sleep_for(std::chrono::seconds(delay));
            if (!active__.load()) return;
            f();
            });
        t.detach();
    }

    void common_timer::SetInterval(int interval, std::function<void()> f) {
        active__ = true;
        std::thread t([=]() {
            while (active__.load()) {
                std::this_thread::sleep_for(std::chrono::seconds(interval));
                if (!active__.load()) return;
                f();
            }
            });
        t.detach();
    }

    void common_timer::Stop() {
        active__ = false;
    }
    const bool common_timer::IsActive() const {
        return active__;
    }
}