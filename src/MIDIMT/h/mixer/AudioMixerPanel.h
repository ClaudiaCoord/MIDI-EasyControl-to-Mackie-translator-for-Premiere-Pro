/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIDIMT {

		class AudioMixerPanel {
		private:
			UI::Panel ctrl__[3]{};
			UI::Sprites& sprites__;
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
			void valueknob_(UI::Panel&, bool, bool);

		public:
			enum ITEMID : int {
				PANEL_ID = 0,
				KNOB_ID,
				TEXT_ID,
				ICON_ID = KNOB_ID,
				BALLOON_ID = TEXT_ID
			};

			ui_theme& Theme;
			
			AudioMixerPanel(UI::Sprites&);
			~AudioMixerPanel();

			HWND GetHWND(ITEMID);

			UI::ToolTipData GetBalloonData();
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