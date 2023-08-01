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

		enum class CbHWNDType : int {
			TYPE_CB_LOG = 0,
			TYPE_CB_MON,
			TYPE_CB_NONE
		};

		class CbEventData;
		class CbEventDataDeleter {
		private:
			CbEventData* cbd__ = nullptr;
		public:

			CbEventDataDeleter(CbEventData*);
			~CbEventDataDeleter();
			CbEventData* GetData();
			CbHWNDType GetType();

			template <typename T>
			T Get();
		};

		class CbEventData {
		private:
			std::wstring ws__{};
			std::pair<DWORD, MIDI::Mackie::MIDIDATA> data__;
			CbHWNDType type__ = CbHWNDType::TYPE_CB_NONE;
		public:

			CbEventData(std::wstring);
			CbEventData(MIDI::Mackie::MIDIDATA&, DWORD&);
			CbHWNDType GetType();
			CbEventDataDeleter GetDeleter();

			template <typename T>
			T Get();
		};

		class CbEvent : public MIDI::MidiInstance {
		private:
			int ILOG = -1, IMON = -1;

			void LogCb(const std::wstring&);
			const bool MonitorCb(MIDI::Mackie::MIDIDATA&, DWORD&);

		public:

			std::function<HWND()> HwndCb;

			CbEvent();

			void Init(int, int);
			void Clear();
			void AddToLog(std::wstring);
			static void ToLog(HWND, CbEventData*, bool);
			static void ToMonitor(HWND, CbEventData*, bool);
		};
	}
}