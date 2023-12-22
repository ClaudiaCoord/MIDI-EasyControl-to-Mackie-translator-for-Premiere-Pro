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

		class DialogEditInfo : public IO::PluginUi {
		private:

			void dispose_();
			void init_();

		public:

			DialogEditInfo() = default;
			~DialogEditInfo() = default;

			IO::PluginUi* GetUi();

			const bool IsRunOnce();
			void SetFocus();

			HWND BuildDialog(HWND) override final;
			LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM) override final;
		};
	}
}
