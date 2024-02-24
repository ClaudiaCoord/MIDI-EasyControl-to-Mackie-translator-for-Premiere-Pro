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

	class FLAG_EXPORT common_static {
	public:
		static common_config commonconfig_empty;
		static common_error_code common_error_code_empty;
		static LIGHT::SerialPortConfig serialportconfig_empty;
		static LIGHT::ArtnetConfig artnetconfig_empty;
		static MIDI::MidiUnit unit_empty;
		static ui_theme theme_empty;
	};
}

