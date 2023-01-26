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

		class CbEvent : public MIDI::MidiInstance {
		private:
			log_string lsl__;
			log_string lsm__;
			std::queue<std::wstring> base_log__;
			std::queue<std::pair<DWORD, Common::MIDI::Mackie::MIDIDATA>> base_monitor__;

			void LogCb(const std::wstring&);
			const bool MonitorCb(Common::MIDI::Mackie::MIDIDATA&, DWORD&);

		public:

			bool IsLogOneLine = false;
			bool IsMonitorOneLine = false;
			std::function<void()> LogNotify;
			std::function<void()> MonitorNotify;
			callMidiOut1Cb MonitorData;

			CbEvent();

			void Clear();
			void AddToLog(std::wstring);
			void LogLoop(HWND, UINT id);
			void MonitorLoop(HWND, UINT id);
		};
	}
}