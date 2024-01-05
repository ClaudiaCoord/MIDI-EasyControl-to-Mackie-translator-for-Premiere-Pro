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

		void Sprite::dispose() {
			try {
				if (icons_.empty()) return;
				std::vector<HBITMAP> icons = icons_;
				icons_.clear();
				for (auto& a : icons)
					if (a != nullptr) ::DeleteObject(a);
			} catch (...) {}
		}
		void Sprite::add(std::vector<uint8_t>& v, int w, int h) {
			try {
				if (v.empty()) return;
				BYTE* bytes = &v[0];
				HBITMAP hbmp = ::CreateBitmap(w, h, 1, 32, bytes);
				if (hbmp != nullptr) icons_.push_back(hbmp);
			} catch (...) {}
		}
		void Sprite::foreach(std::vector<uint8_t>& vi, int w, int h, const uint8_t colors) {
			try {
				std::vector<uint8_t> v;
				int32_t bsize = colors == 6 ? 4 : 3,
					bsplit = ((bsize * w) * w);

				for (int32_t y = 0U, offset, cursor = bsplit; y < h; y++) {
					offset = ((bsize * w) * y);

					for (int32_t x = 0U; x < w; x++) {
						uint32_t pos = (offset + (x * 4));
						v.push_back(vi[(pos + 2)]);
						v.push_back(vi[(pos + 1)]);
						v.push_back(vi[(pos + 0)]);
						v.push_back(vi[(pos + 3)]);
						if ((pos + 4) == cursor) {
							cursor += bsplit;
							add(v, w, w);
							v.clear();
							break;
						}
					}
				}
			} catch (...) {}
		}

		Sprite::Sprite() {
		}
		Sprite::~Sprite() {
			dispose();
		}
		void Sprite::load(uint16_t id, HMODULE h) {
			try {
				HRSRC res = nullptr;
				HGLOBAL glob = nullptr;
				dispose();

				do {
					if ((res = ::FindResourceW(h, MAKEINTRESOURCEW(id), L"PNG")) == nullptr) break;
					if ((glob = ::LoadResource(h, res)) == nullptr) break;

					uint32_t width, height, val = ::SizeofResource(h, res);
					if (val == 0) break;
					uint8_t* ptr = (uint8_t*) ::LockResource(glob);
					if (ptr == nullptr) break;

					std::vector<uint8_t> iout, iin;
					for (uint32_t i = 0U; i < val; i++)
						iin.push_back((uint8_t)ptr[i]);

					if ((val = lodepng::decode(iout, width, height, iin, LCT_RGBA, 8)) != 0U)
						throw make_common_error(Utils::to_string(lodepng_error_text(val)));

					foreach(iout, width, height, LCT_RGBA);

				} while (0);
			} catch (...) {}
		}
		void Sprite::clear() {
			dispose();
		}
		Sprite::operator bool() const {
			return icons_.size() > 0U;
		}
		size_t Sprite::size() const {
			return icons_.size();
		}
		HBITMAP Sprite::get(uint16_t idx) {
			return (icons_.size() > idx) ? icons_[idx] : nullptr;
		}
	}
}


