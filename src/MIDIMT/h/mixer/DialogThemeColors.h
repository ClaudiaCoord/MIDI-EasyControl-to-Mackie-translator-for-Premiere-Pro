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

		class DialogThemeColors : public IO::PluginUi {
		private:
			hwnd_ptr<empty_deleter>   hwndp_{};
			hwnd_ptr<empty_deleter>   ctrls_[3]{};
			handle_ptr<HBRUSH, default_hgdiobj_deleter<HBRUSH>> brushs_[3]{};
			std::atomic<bool>  ischanged_ {false};
			static COLORREF customcolors_[16];

			void dispose_();
			void init_();
			void change_brush_(uint16_t, COLORREF);
			void change_theme_(ui_theme&);
			void change_color_select_(uint16_t);
			void change_theme_select_(uint16_t);
			LRESULT event_colors_draw_(LPARAM);

		public:

			DialogThemeColors() = default;
			~DialogThemeColors();

			IO::PluginUi* GetUi();

			const bool IsRun() const;
			const bool IsRunOnce() const;
			void SetFocus();

			const bool IsChanged() const;
			void SetHWNDParent(HWND);

			HWND BuildDialog(HWND) override final;
			LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM) override final;
		};
	}
}
