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

		class TransportItem {
		public:
			MIXER::AudioSessionItemChange item;
			TransportItem(MIXER::AudioSessionItemChange);
		};
		class TransportDeleter {
		public:
			TransportItem* tr;
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
			PanelData ctrl__[2];
			BuildPoint bp__;
			GuiImageStateButton<HICON> icon__;
			MIXER::callOnChange event_cb__ = [](MIXER::AudioSessionItemChange) {};
			std::atomic<int32_t> startid__ = 50000;
			std::atomic<int32_t> position__ = 0;
			std::atomic<bool> isopen__ = false;
			uint32_t event_id__ = 0U;
			std::unique_ptr<DialogThemeColors> dlgt;
			TrayNotify* trayn;

			bool  add(MIXER::AudioSessionItemChange);
			void  dispose();
			void  buildmenu();
			void  inituitheme();
			void  inituiele();
			POINT menuposition();
			RECT  panelposition();
			void  eventmixercb(TransportItem*);
			void  eventmixercb(MIXER::AudioSessionItemChange);

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
			static LRESULT CALLBACK EventPANEL_ID(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
			static LRESULT CALLBACK EventICON_ID(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
			static INT_PTR CALLBACK CustomThemeDialog(HWND, UINT, WPARAM, LPARAM);
		};
	}
}
