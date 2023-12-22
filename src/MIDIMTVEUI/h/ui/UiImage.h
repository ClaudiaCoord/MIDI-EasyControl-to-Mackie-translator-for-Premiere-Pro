/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ UI/Viewer/Editor interface.
	(c) CC 2023, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace UI {

		#pragma warning( push )
		#pragma warning( disable : 4910 )

		template <typename T>
		class FLAG_EXPORT ICONDATA {
			T icon;
			uint16_t iconId, ctrlId;
		public:
			ICONDATA();
			ICONDATA(T);
			ICONDATA(T, uint16_t, uint16_t);
			uint16_t GetIconId();
			uint16_t GetCtrlId();
			void Reset();
			T Get();
		};

		FLAG_EXTERN template class FLAG_IMPORT ICONDATA<HICON>;
		FLAG_EXTERN template class FLAG_IMPORT ICONDATA<HBITMAP>;

		template <typename T>
		class FLAG_EXPORT ImageStateButton {
		private:
			bool isimagebox_{ false };
			hwnd_ptr<empty_deleter> hwnd_{};
			std::vector<ICONDATA<T>*> icos_{};
			void dispose_();
		public:

			ImageStateButton();
			~ImageStateButton();
			size_t Count() const;
			bool  IsEmpty() const;
			void  Init(HWND, uint16_t);
			void  Init(uint16_t[], size_t, std::function<HICON(uint16_t)>, bool = false);
			void  Reset();
			void  Release();
			void  SetEnable(bool = true);
			void  SetStatus(uint16_t = 0);
			void  SetAnimateStatus(uint16_t = 0);
			T     GetIcon(uint16_t = 0);
		};

		FLAG_EXTERN template class FLAG_IMPORT ImageStateButton<HICON>;
		FLAG_EXTERN template class FLAG_IMPORT ImageStateButton<HBITMAP>;

		#pragma warning( pop )

		#if defined (EXT_DRAW_BITMAP)
		class FLAG_EXPORT BitmapInfo {
		private:
			BITMAPINFO bmi__{};
			void set(BITMAP&);
		public:
			BitmapInfo(BITMAP&);
			BitmapInfo(HBITMAP);
			BITMAPINFO* get();
			int width();
			int height();
		};
		#endif
	}
}