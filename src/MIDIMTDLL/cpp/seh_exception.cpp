/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include <eh.h>
#include "global.h"

namespace Common {

    seh_exception::~seh_exception() {}
    seh_exception::seh_exception() : err__(UINT_MAX-1) {}
    seh_exception::seh_exception(seh_exception& e) : err__(e.err__) {}
    seh_exception::seh_exception(uint32_t n) : err__(n) {}
    const uint32_t seh_exception::error() const {
        return err__;
    }

    FLAG_EXPORT void seh_exception_catch(uint32_t u, _EXCEPTION_POINTERS*) {
        throw seh_exception(u);
    }
}
