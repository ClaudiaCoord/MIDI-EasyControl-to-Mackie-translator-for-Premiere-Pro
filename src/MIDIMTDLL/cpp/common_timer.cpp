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

namespace Common {

    void common_timer::SetTimeout(int delay_, std::function<void()> f) {
        active_.store(true);
        delay = delay_;
        std::jthread t([=]() {
            if (!active_.load(std::memory_order_acquire)) return;
            std::this_thread::sleep_for(std::chrono::seconds(delay));
            if (!active_.load(std::memory_order_acquire)) return;
            f();
            });
        t.detach();
    }

    void common_timer::SetInterval(int interval_, std::function<void()> f) {
        active_.store(true);
        delay = interval_;
        std::jthread t([=]() {
            while (active_.load(std::memory_order_acquire)) {
                std::this_thread::sleep_for(std::chrono::seconds(delay));
                if (!active_.load(std::memory_order_acquire)) return;
                f();
            }
            });
        t.detach();
    }

    void common_timer::Stop() {
        active_.store(false, std::memory_order_release);
    }
    const bool common_timer::IsActive() const {
        return active_.load(std::memory_order_acquire);
    }
}
