/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	EASYCTRL9

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIDI {

		typedef struct _VM_MIDI_PORT VM_MIDI_PORT, * LPVM_MIDI_PORT;
		typedef void (CALLBACK *LPVM_MIDI_DATA_CB)(LPVM_MIDI_PORT, LPBYTE, DWORD, DWORD_PTR);

		typedef LPVM_MIDI_PORT (CALLBACK *vMIDICreatePortEx2)(LPCWSTR, LPVM_MIDI_DATA_CB, DWORD_PTR, DWORD, DWORD);
		typedef LPVM_MIDI_PORT (CALLBACK *vMIDICreatePortEx3)(LPCWSTR, LPVM_MIDI_DATA_CB, DWORD_PTR, DWORD, DWORD, GUID*, GUID*);
		typedef void (CALLBACK *vMIDIClosePort)(LPVM_MIDI_PORT);
		typedef BOOL (CALLBACK *vMIDISendData)(LPVM_MIDI_PORT, LPBYTE, DWORD);
		typedef BOOL (CALLBACK *vMIDIGetData)(LPVM_MIDI_PORT, LPBYTE, PDWORD);
		typedef BOOL (CALLBACK *vMIDIGetProcesses)(LPVM_MIDI_PORT, ULONG64*, PDWORD);
		typedef BOOL (CALLBACK *vMIDIShutdown)(LPVM_MIDI_PORT);
		typedef LPCWSTR (CALLBACK *vMIDIGetVersion)(PWORD, PWORD, PWORD, PWORD);
		typedef LPCWSTR (CALLBACK *vMIDIGetDriverVersion)(PWORD, PWORD, PWORD, PWORD);
		typedef DWORD (CALLBACK *vMIDILogging)(DWORD);

		class MidiDriver {
		private:
			HMODULE hdrv__{ nullptr };

			void dispose_();
			void clear_();
			bool init_();

			vMIDICreatePortEx2 vCreatePortEx2_{ nullptr };
			vMIDIGetDriverVersion vGetDriverVer_{ nullptr };
			vMIDISendData vSendData_{ nullptr };
			vMIDIClosePort vClosePort_{ nullptr };

		public:

			enum STAT : int {
				ERR_NOT_ERRORS = 0,
				ERR_PATH_NOT_FOUND = 3,
				ERR_INVALID_HANDLE = 6,
				ERR_TOO_MANY_CMDS = 56,
				ERR_TOO_MANY_SESS = 69,
				ERR_INVALID_NAME = 123,
				ERR_MOD_NOT_FOUND = 126,
				ERR_BAD_ARGUMENTS = 160,
				ERR_ALREADY_EXISTS = 183,
				ERR_OLD_WIN_VERSION = 1150,
				ERR_REVISION_MISMATCH = 1306,
				ERR_ALIAS_EXISTS = 1379,
			};

			vMIDICreatePortEx3 vCreatePortEx3{ nullptr };
			vMIDIGetData vGetData{ nullptr };
			vMIDIGetProcesses vGetProcesses{ nullptr };
			vMIDIShutdown vShutdown{ nullptr };
			vMIDIGetVersion vGetVersion{ nullptr };
			vMIDILogging vLogging{ nullptr };

			MidiDriver();
			~MidiDriver();

			const bool Check();
			const bool IsEmpty() const;
			const bool CheckMMRESULT(std::function<MMRESULT()> f, std::wstring tag);

			/* virtual MIDI API */
			LPVM_MIDI_PORT vCreatePortEx2(LPCWSTR, LPVM_MIDI_DATA_CB, DWORD_PTR);
			void vClosePort(LPVM_MIDI_PORT);
			bool vSendData(LPVM_MIDI_PORT, LPBYTE, DWORD);
			LPCWSTR vGetDriverVersion();

			/* standart MIDI API */
			UINT     InGetNumDevs();
			UINT     OutGetNumDevs();
			MMRESULT InOpen(LPHMIDIIN, UINT, DWORD_PTR, DWORD_PTR);
			MMRESULT OutOpen(LPHMIDIOUT, UINT, DWORD_PTR, DWORD_PTR);

			MMRESULT InStart(HMIDIIN);
			MMRESULT InStop(HMIDIIN);
			MMRESULT InReset(HMIDIIN);
			MMRESULT InClose(HMIDIIN);

			MMRESULT OutReset(HMIDIOUT);
			MMRESULT OutClose(HMIDIOUT);

			MMRESULT InGetDevCaps(UINT_PTR, LPMIDIINCAPSW, UINT);
			MMRESULT OutGetDevCaps(UINT_PTR, LPMIDIOUTCAPSW, UINT);
			MMRESULT OutShortMsg(HMIDIOUT, DWORD);
		};
	}
}

