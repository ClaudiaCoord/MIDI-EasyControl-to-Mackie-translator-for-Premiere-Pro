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

		class TransportDeleter;
		class TransportItem {
		public:
			MIXER::AudioSessionItemChange item;
			TransportItem(MIXER::AudioSessionItemChange);

			TransportDeleter GetDeleter();
		};
		class TransportDeleter {
		public:
			TransportItem* ti;
			TransportDeleter(TransportItem*);
			~TransportDeleter();
		};

		class BuildPoint {
		private:
			POINT p__{};
		public:
			POINT& Build(HWND, LPARAM);
			RECT   Build();
			void   Save();
			void   Save(RECT);
			void   Set(RECT);
		};

		class AudioMixerPanels
		{
		private:
			std::vector<AudioMixerPanel*> panels__;
			Sprites sprites__;
			PanelData ctrl__[3];
			BuildPoint bp__;
			GuiImageStateButton<HICON> icon__;
			MIXER::callOnChange event_cb__ = [](MIXER::AudioSessionItemChange) {};
			std::atomic<int32_t> startid__ = 50000;
			std::atomic<int32_t> position__ = 0;
			std::atomic<bool> isopen__{ false };
			std::atomic<bool> isballoon__{ false };
			std::atomic<bool> isduplicateappremoved__{ false };
			std::unique_ptr<DialogThemeColors> dlgt;
			TrayNotify* trayn;
			uint32_t event_id__ = 0U;
			TTTOOLINFOW ti__{};

			bool	add_(MIXER::AudioSessionItemChange);
			void	dispose_();
			void	buildmenu_();
			void	inituitheme_();
			void	inituiele_(const ui_themes::ThemePlace place);
			POINT	menuposition_(const ui_themes::ThemePlace);
			RECT	panelposition_(const ui_themes::ThemePlace place);
			int32_t panelbeginsize_(bool);
			int32_t panelbeginsize_(common_config&, RECT&);
			void	eventmixercb_(TransportItem*);
			void	eventmixercb_(MIXER::AudioSessionItemChange);
			void	setballoon_(ToolTipData);
			void	setballoon_(HWND);

		public:
			AudioMixerPanels(TrayNotify*);
			~AudioMixerPanels();
			
			ui_theme Theme{};

			bool Open();
			void Close();
			void CallMenu();
			void Show();
			bool IsOpen() const;

			PanelData& GetPanel();
			BuildPoint& GetBuildPoint();
			DialogThemeColors* GetDialogThemeColors();
			
			void SwitchUiTheme(ui_themes::ThemeId);
			void SwitchUiPlace(ui_themes::ThemePlace);
			void SwitchVisableStatus(bool);
			void SwitchAnimation();
			void SwitchShowAppPath();
			void SwitchPeakMeter();
			void SwitchMidiBind();
			static LRESULT CALLBACK EventPANEL_ID(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
			static LRESULT CALLBACK EventICON_ID(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
			static INT_PTR CALLBACK CustomThemeDialog(HWND, UINT, WPARAM, LPARAM);
		};
	}
}
