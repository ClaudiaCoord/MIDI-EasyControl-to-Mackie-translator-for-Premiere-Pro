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

		/*
			local panel and other elements size, RECTANGLE:
			{ left = 0 (pad x), right = 0 (pad y), top = 84 (width), bottom = 94 (height) }
		*/
		static inline int32_t PLACESIZE(const bool isvertical, RECT& r) {
			return (isvertical ? (r.bottom + r.right) : (r.top + r.left));
		}

		class ToolTipData {
		public:
			HWND		 hwnd;
			RECT		 rect{};
			std::wstring title;

			ToolTipData();
			ToolTipData(HWND, RECT&, const std::wstring&);

			const bool   IsValid();
		};

		class PanelData {
		private:
			std::atomic<bool> anime__;
			std::atomic<bool> enable__;
			std::atomic<bool> mcapture__;
			handle_ptr<HWND, default_hwnd_deleter<HWND>> hwnd__;
			handle_ptr<SUBCLASSPROC> proc__;
			int32_t id__ = 0;

			void dispose_();
			void refresh_();

		public:
			uint32_t style;
			int32_t x, y, w, h;

			PanelData();
			~PanelData();
			void Close(int32_t = -1);
			void Show(const bool);
			void Show(int32_t = 0);
			void Set(uint32_t, int32_t, int32_t, int32_t, int32_t);
			const bool Init(HINSTANCE, int32_t); /* ToolTip init */
			const bool Init(HINSTANCE, HWND, int32_t, int32_t, SUBCLASSPROC, bool, void*);
			HWND GetHWND();
			
			const bool GetEnabled();
			void SetEnabled(bool);
			void SetCapture(bool = true);
			void SetAnimation(bool);
			RECT GetSize();
			void SetSize(RECT& r);
			bool SetPosition(bool, bool);
			
			std::wstring to_string();

			template <typename T1>
			void SetData(T1 val) {
				if (enable__ && (hwnd__ != nullptr)) {
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
			std::atomic<int32_t> knob_idx__{ 0 };
			std::atomic<bool> disposed__{ false };
			std::atomic<bool> isduplicateappremoved__{ false };
			std::atomic<bool> showmidikeybind__{ false };
			std::atomic<bool> showpeaklevel__{ false };
			std::atomic<bool> statevisable__{ true };
			MIXER::AudioSessionItemChange Item{};

			void dispose_(int32_t = -1);
			void datacb_(std::wstring&, std::wstring&, std::wstring&, std::wstring&, GUID&, uint32_t);
			void valuecb_(bool, MIXER::OnChangeType, uint8_t, float, bool);
			void valuecb_mute_(bool, bool, bool);
			void valuecb_volume_(bool, bool, uint8_t);
			void valueknob_(PanelData&, bool, bool);

		public:
			enum ITEMID : int {
				PANEL_ID = 0,
				KNOB_ID,
				TEXT_ID,
				ICON_ID = KNOB_ID,
				BALLOON_ID = TEXT_ID
			};

			ui_theme& Theme;
			
			AudioMixerPanel(HINSTANCE, Sprites&);
			~AudioMixerPanel();

			HWND GetHWND(ITEMID);

			ToolTipData GetBalloonData();
			RECT GetSize(ITEMID = ITEMID::PANEL_ID);
			void SetSize(RECT&, ITEMID = ITEMID::PANEL_ID);
			void SetVisableStatus(bool);
			const bool SetPositionUp(bool, bool);

			HBITMAP GetCurrentSprite();
			MIXER::AudioSessionItemChange& GetAudioItem();
			void SetBtnMute();
			void SetKnobValue(int32_t);
			void SetCapture(ITEMID, bool = true);
			void ShowAnimation(bool);
			void ShowPeakMeter(bool);
			void ShowMidiKeyBind(bool);

			bool InitItem(MIXER::AudioSessionItemChange);
			void UpdateItem(MIXER::AudioSessionItemChange&);

			const std::size_t GetAppId();
			const bool IsAppValid();
			const bool IsMaster();

			void Close();
			AudioMixerPanel* Open(HINSTANCE, HWND, MIXER::AudioSessionItemChange, ui_theme&, std::atomic<int32_t>&, std::atomic<int32_t>&, bool, bool, bool);

			static LRESULT CALLBACK EventPANEL_ID(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
			static LRESULT CALLBACK EventKNOB_ID(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
			static LRESULT CALLBACK EventTEXT_ID(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
		};
	}
}