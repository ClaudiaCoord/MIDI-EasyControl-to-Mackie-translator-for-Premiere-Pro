/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIDIMT {

		class Sprites
		{
		private:
			void dispose();
		public:
			Sprite sprites_enabled;
			Sprite sprites_disabled;

			Sprites();
			~Sprites();
			void load(uint32_t, uint32_t);
			void clear();
		};
	}
}
