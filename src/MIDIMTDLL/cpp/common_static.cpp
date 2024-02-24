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

	using namespace std::string_view_literals;

	ui_theme common_static::theme_empty{};
	MIDI::MidiUnit common_static::unit_empty{};
	LIGHT::ArtnetConfig common_static::artnetconfig_empty{};
	LIGHT::SerialPortConfig common_static::serialportconfig_empty{};

	common_config common_static::commonconfig_empty{};
	common_error_code common_static::common_error_code_empty{};
}
