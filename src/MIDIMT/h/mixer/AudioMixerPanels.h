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
			POINT p_{};
		public:
			POINT& Build(HWND, LPARAM);
			RECT   Build();
			void   Save();
			void   Save(RECT);
			void   Set(RECT);
		};

		class audiomixerpanel_deleter {
		public:
			void operator() (AudioMixerPanel* p) {
				try {
					if (p) {
						p->Close();
						delete p;
					}
				} catch (...) {}
			}
		};

		class AudioMixerPanels {
		private:
			std::vector<std::shared_ptr<AudioMixerPanel>> panels_{};
			UI::Sprites sprites_;
			UI::Panel ctrl_[3];
			BuildPoint bp_;
			UI::ImageStateButton<HICON> icon_;
			MIXER::callOnChange_t event_cb_ = [](MIXER::AudioSessionItemChange) {};
			std::atomic<int32_t> startid_{ 50000 };
			std::atomic<int32_t> position_{ 0 };
			std::atomic<bool> isopen_{ false };
			std::atomic<bool> isballoon_{ false };
			std::atomic<bool> isduplicateappremoved_{ false };
			uint32_t event_id_{ 0U };
			TTTOOLINFOW ti_{};

			bool	add_(MIXER::AudioSessionItemChange);
			void	close_();
			void	dispose_();
			void	build_menu_();
			void	init_ui_theme_();
			void	init_ui_ele_(const ui_themes::ThemePlace place);
			POINT	menu_position_(const ui_themes::ThemePlace);
			RECT	panel_position_(const ui_themes::ThemePlace place);
			int32_t panel_begin_size_(bool);
			int32_t panel_begin_size_(common_config&, RECT&);
			void	event_mixer_cb_(TransportItem*);
			void	event_mixer_cb_(MIXER::AudioSessionItemChange);
			void	set_balloon_(UI::ToolTipData);
			void	set_balloon_(HWND);
			void	notify_warning_(std::wstring, std::wstring);

		public:
			AudioMixerPanels();
			~AudioMixerPanels();
			
			ui_theme Theme{};

			bool Open();
			void Close();
			void CallMenu();
			void Show();
			bool IsOpen() const;

			UI::Panel& GetPanel();
			BuildPoint& GetBuildPoint();
			
			void SwitchUiTheme(ui_themes::ThemeId);
			void SwitchUiPlace(ui_themes::ThemePlace);
			void SwitchVisableStatus(bool);
			void SwitchAnimation();
			void SwitchShowAppPath();
			void SwitchPeakMeter();
			void SwitchMidiBind();
			static LRESULT CALLBACK EventPANEL_ID(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
			static LRESULT CALLBACK EventICON_ID(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
		};
	}
}
