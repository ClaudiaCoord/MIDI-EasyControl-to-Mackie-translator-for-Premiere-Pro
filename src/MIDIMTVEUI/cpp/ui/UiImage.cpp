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

		/* ICONDATA */

		template <typename T>
		ICONDATA<T>::ICONDATA() : icon(nullptr), iconId(0U), ctrlId(0U) {}
		template <typename T>
		ICONDATA<T>::ICONDATA(T h) : icon(h), iconId(0U), ctrlId(0U) {}
		template <typename T>
		ICONDATA<T>::ICONDATA(T h, uint16_t i, uint16_t c) : icon(h), iconId(i), ctrlId(c) {}
		template <typename T>
		T ICONDATA<T>::Get() { return icon; }
		template <typename T>
		uint16_t ICONDATA<T>::GetIconId() { return iconId; }
		template <typename T>
		uint16_t ICONDATA<T>::GetCtrlId() { return ctrlId; }
		template <typename T>
		void ICONDATA<T>::Reset() {
			T h = icon;
			icon = nullptr;
			if (h != nullptr) {
				if constexpr (std::is_same_v<HBITMAP, T>) ::DeleteObject(h);
				else if constexpr (std::is_same_v<HICON, T>) ::DestroyIcon(h);
			}
		}

		template class FLAG_EXPORT ICONDATA<HICON>;
		template class FLAG_EXPORT ICONDATA<HBITMAP>;

		/* GuiImageStateButton */

		template <typename T>
		ImageStateButton<T>::ImageStateButton() : isimagebox_(false) {
		}
		template <typename T>
		ImageStateButton<T>::~ImageStateButton(){
			dispose_();
		}
		template <typename T>
		void ImageStateButton<T>::dispose_() {
			try {
				std::vector<ICONDATA<T>*> icos = icos_;
				icos_.clear();

				for (auto& a : icos)
					if (a != nullptr) {
						a->Reset(); delete a;
					}
				hwnd_.reset();
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		template <typename T>
		void ImageStateButton<T>::Init(HWND hwnd, uint16_t id) {
			try {
				HWND hwctrl = ::GetDlgItem(hwnd, id);
				hwnd_.reset(hwctrl);
				SetStatus();
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		template <typename T>
		void ImageStateButton<T>::Init(uint16_t ids[], size_t size, std::function<HICON(uint16_t)> f, bool is_imagebox) {
			try {
				isimagebox_ = is_imagebox;
				for (size_t i = 0; i < size; i++) {
					uint16_t n = ids[i];
					HICON h = f(n);
					if (h == nullptr) continue;

					if constexpr (std::is_same_v<HICON, T>) {
						icos_.push_back(new ICONDATA<T>(h, n, 0U));
					}
					else if constexpr (std::is_same_v<HBITMAP, T>) {
						ICONINFOEX ii{};
						ii.cbSize = sizeof(ii);
						if (::GetIconInfoExW(h, &ii) && (ii.hbmColor != nullptr)) {
							T img = (T) ::CopyImage(ii.hbmColor, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
							::DestroyIcon(h);
							icos_.push_back(new ICONDATA<T>(img, n, 0U));
						}
					}
				}
			} catch (...) {
				Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		template <typename T>
		void ImageStateButton<T>::Reset() {
			hwnd_.reset();
		}
		template <typename T>
		void ImageStateButton<T>::Release() {
			dispose_();
		}
		template <typename T>
		void ImageStateButton<T>::SetEnable(bool val) {
			if (!hwnd_) return;
			::EnableWindow(hwnd_.get(), val);
		}
		template <typename T>
		void ImageStateButton<T>::SetStatus(uint16_t idx) {
			if (!hwnd_ || (icos_.size() <= idx)) return;
			(void) ::SendMessageW(hwnd_.get(), (isimagebox_ ? STM_SETIMAGE : BM_SETIMAGE), IMAGE_ICON, (LPARAM)icos_[idx]->Get());
		}
		template <typename T>
		void ImageStateButton<T>::SetAnimateStatus(uint16_t idx) {
			if (!hwnd_ || (icos_.size() <= idx)) return;
			(void) ::AnimateWindow(
				hwnd_.get(),
				400,
				(AW_HIDE | AW_BLEND)
			);
			(void) ::SendMessageW(hwnd_.get(), (isimagebox_ ? STM_SETIMAGE : BM_SETIMAGE), IMAGE_ICON, (LPARAM)icos_[idx]->Get());
			(void) ::AnimateWindow(
				hwnd_.get(),
				600,
				(AW_ACTIVATE | AW_CENTER)
			);
		}
		template <typename T>
		T ImageStateButton<T>::GetIcon(uint16_t idx) {
			if (icos_.size() <= idx) return nullptr;
			return icos_[idx]->Get();
		}
		template <typename T>
		bool  ImageStateButton<T>::IsEmpty() const {
			return icos_.empty();
		}
		template <typename T>
		size_t ImageStateButton<T>::Count() const {
			return icos_.size();
		}

		template class FLAG_EXPORT ImageStateButton<HICON>;
		template class FLAG_EXPORT ImageStateButton<HBITMAP>;

		/* BitmapInfo */

		#if defined (EXT_DRAW_BITMAP)
		BitmapInfo::BitmapInfo(BITMAP& bmp) {
			set(bmp);
		}
		BitmapInfo::BitmapInfo(HBITMAP hbmp) {
			BITMAP bmp{};
			if (::GetObjectW(hbmp, sizeof(bmp), &bmp) > 0) set(bmp);
		}
		void BitmapInfo::set(BITMAP& bmp) {
			bmi__.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi__.bmiHeader.biWidth = bmp.bmWidth;
			bmi__.bmiHeader.biHeight = bmp.bmHeight;
			bmi__.bmiHeader.biPlanes = bmp.bmPlanes;
			bmi__.bmiHeader.biBitCount = bmp.bmBitsPixel;
			bmi__.bmiHeader.biCompression = BI_RGB;
		}
		BITMAPINFO* BitmapInfo::get() {
			return &bmi__;
		}
		int BitmapInfo::width() {
			return bmi__.bmiHeader.biWidth;
		}
		int BitmapInfo::height() {
			return bmi__.bmiHeader.biHeight;
		}
		#endif
	}
}
