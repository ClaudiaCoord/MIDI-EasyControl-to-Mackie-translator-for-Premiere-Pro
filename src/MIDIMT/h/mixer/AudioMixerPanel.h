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

		class AudioMixerPanel;

		class PanelData {
		private:
			bool mcapture__;
			std::atomic<bool> isenable__;
			int32_t id__ = 0;
			HWND hwnd__ = nullptr;
			SUBCLASSPROC proc__ = nullptr;

			void dispose_();
			void refresh_();

		public:
			uint32_t style;
			int32_t x, y, w, h;

			PanelData();
			~PanelData();
			void Close();
			void Set(uint32_t, int32_t, int32_t, int32_t, int32_t);
			const bool Init(HINSTANCE, HWND, int32_t, int32_t, SUBCLASSPROC, void*);
			HWND GetHWND();
			[[maybe_unused]] int32_t GetId();

			const bool GetEnabled();
			void SetEnabled(bool);
			void SetCapture(bool = true);
			RECT GetSize();
			void SetSize(RECT& r);

			template <typename T1>
			void SetData(T1 val) {
				if (isenable__ && (hwnd__ != nullptr)) {
					if constexpr (std::is_same_v<HBITMAP, T1>)
						(void) ::SendMessageW(hwnd__, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)val);
					else if constexpr (std::is_same_v<HICON, T1>)
						(void) ::SendMessageW(hwnd__, STM_SETICON, (WPARAM)val, (LPARAM)0);
					else if constexpr (std::is_same_v<std::wstring, T1>)
						(void) ::SendMessageW(hwnd__, WM_SETTEXT, (WPARAM)0, (LPARAM)val.c_str());
					else if constexpr (std::is_same_v<wchar_t[], T1>)
						(void) ::SendMessageW(hwnd__, WM_SETTEXT, (WPARAM)0, (LPARAM)val);
				}
			}
		};

		class AudioMixerPanel
		{
		private:
			PanelData ctrl__[3]{};
			Sprites& sprites__;
			std::atomic<int32_t> knob_idx__{0};
			std::atomic<bool> isdesposed__{false};
			MIXER::AudioSessionItemChange Item{};

			void dispose_();
			void setknobctrl_(PanelData&, bool, bool);
			void setctrl_(bool, MIXER::OnChangeType);
			void updateaudiosessionvalue_(bool = false);
			const bool setaudiosessionitem_(MIXER::AudioSessionItemChange&);

		public:
			enum ITEMID : int {
				PANEL_ID = 0,
				KNOB_ID,
				TEXT_ID,
				ICON_ID = KNOB_ID
			};

			ui_theme& Theme;
			
			AudioMixerPanel(HINSTANCE, Sprites&);
			~AudioMixerPanel();

			HWND GetHWND(ITEMID);
			[[maybe_unused]] int32_t GetId(ITEMID);

			RECT GetSize(ITEMID = ITEMID::PANEL_ID);
			void SetSize(RECT&, ITEMID = ITEMID::PANEL_ID);

			HBITMAP GetCurrentSprite();
			MIXER::AudioSessionItemChange& GetAudioItem();
			void SetBtnMute();
			void SetKnobValue(int32_t);
			void SetCapture(ITEMID, bool = true);
			void UpdateCtrlData(MIXER::AudioSessionItemChange&);
			void UpdateCtrlValue(MIXER::AudioSessionItemChange&);

			[[maybe_unused]] bool IsValid();
			void Close();
			AudioMixerPanel* Open(HINSTANCE, HWND, MIXER::AudioSessionItemChange, ui_theme&, std::atomic<int32_t>&, std::atomic<int32_t>&);
			static LRESULT CALLBACK EventPANEL_ID(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
			static LRESULT CALLBACK EventKNOB_ID(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
			static LRESULT CALLBACK EventTEXT_ID(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
		};
	}
}