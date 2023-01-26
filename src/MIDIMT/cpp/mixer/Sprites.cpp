/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"

namespace Common {
	namespace MIDIMT {

		Sprites::Sprites() {

		}
		Sprites::~Sprites() {
			dispose();
		}
		void Sprites::dispose() {
			sprites_enabled.clear();
			sprites_disabled.clear();
		}
		void Sprites::clear() {
			dispose();
		}
		void Sprites::load(uint32_t ide, uint32_t idd) {
			sprites_enabled.load(ide);
			if (!sprites_enabled || (sprites_enabled.size() != 31))
				throw runtime_werror(L"wrong count in sprites enabled");
			sprites_disabled.load(idd);
			if (!sprites_disabled || (sprites_disabled.size() != 31))
				throw runtime_werror(L"wrong count in sprites disabled");
		}
	}
}
