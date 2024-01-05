/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {

    class FLAG_EXPORT seh_exception
    {
    private:
        uint32_t err__;
    public:
        seh_exception();
        ~seh_exception();
        seh_exception(seh_exception&);
        seh_exception(uint32_t);
        const uint32_t error() const;
    };
    FLAG_EXPORT void seh_exception_catch(uint32_t, _EXCEPTION_POINTERS*);
}
