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

    class FLAG_EXPORT common_timer {
        std::atomic<bool> active__ { true };

    public:
        const bool IsActive() const;
        void SetTimeout(int delay, std::function<void()> f);
        void SetInterval(int interval, std::function<void()> f);
        void Stop();
    };
}