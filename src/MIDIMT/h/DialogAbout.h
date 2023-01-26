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

		class DialogAbout
		{
			handle_ptr<HWND> hwnd__;

		public:

			DialogAbout() = default;
			~DialogAbout() = default;

			void InitDialog(HWND);
			void EndDialog();
		};

	}
}