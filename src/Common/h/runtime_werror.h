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

    class FLAG_EXPORT runtime_werror : public std::runtime_error
    {
    private:
        const std::wstring msg__;
    public:
        runtime_werror(const std::wstring& m);
        const std::wstring error();
    };
}
