/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ UI/Viewer/Editor interface.
	(c) CC 2023-2024, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#pragma once

namespace Common {
	namespace UI {

		class FLAG_EXPORT Sprites {
		private:
			size_t size_{ 0 };
			void dispose_();
		public:
			Sprite sprites_enabled;
			Sprite sprites_disabled;

			Sprites();
			~Sprites();
			size_t size(size_t);
			void load(uint32_t, uint32_t, HMODULE);
			void clear();
		};
	}
}

