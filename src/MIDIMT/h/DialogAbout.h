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

		class DialogAbout : public IO::PluginUi {
		private:

			void dispose_();
			void init_();

		public:

			DialogAbout() = default;
			~DialogAbout() = default;

			IO::PluginUi* GetUi();

			const bool IsRunOnce();
			void SetFocus();

			HWND BuildDialog(HWND) override final;
			LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM) override final;
		};
	}
}