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

		class FLAG_EXPORT Sprite {
		private:
			std::vector<HBITMAP> icons_;

			void foreach(std::vector<uint8_t>&, int, int, const uint8_t);
			void add(std::vector<uint8_t>&, int, int);
			void dispose();

		public:
			Sprite();
			~Sprite();

			operator bool() const;

			size_t size() const;
			HBITMAP get(uint16_t = 0U);
			void clear();
			void load(uint16_t, HMODULE);
		};
	}
}

