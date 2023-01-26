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

		class DialogMonitor
		{
			handle_ptr<HWND> hwnd__;
			CbEvent mcb__;

			void Dispose();

		public:

			DialogMonitor();
			~DialogMonitor();
			const bool IsRunOnce();
			void SetFocus();

			void EventLog();
			void EventMonitor();

			void InitDialog(HWND);
			void EndDialog();

			void Start();
			void Stop();
			void Clear();

		};

	}
}