/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ UI/Viewer/Editor interface.
	(c) CC 2023-2024, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace UI {

		Sprites::Sprites() {
		}
		Sprites::~Sprites() {
			dispose_();
		}
		void Sprites::dispose_() {
			sprites_enabled.clear();
			sprites_disabled.clear();
		}
		size_t Sprites::size(size_t z) {
			if (z > 0) size_ = z;
			return size_;
		}
		void Sprites::clear() {
			dispose_();
		}
		void Sprites::load(uint32_t ide, uint32_t idd, HMODULE h) {
			if (sprites_enabled.size() != sprites_disabled.size())
				throw make_common_error(L"wrong count in group (enabled/disabled) sprites");
			sprites_enabled.load(ide, h);
			if (!size_) size_ = sprites_enabled.size();
			if (!sprites_enabled || (sprites_enabled.size() != size_))
				throw make_common_error(L"wrong count in sprites enabled");
			sprites_disabled.load(idd, h);
			if (!sprites_disabled || (sprites_disabled.size() != size_))
				throw make_common_error(L"wrong count in sprites disabled");
		}
	}
}

