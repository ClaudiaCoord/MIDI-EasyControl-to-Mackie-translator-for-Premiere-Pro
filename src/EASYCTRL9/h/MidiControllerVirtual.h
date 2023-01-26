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

        class FLAG_EXPORT MidiControllerVirtual : public MidiControllerBase
        {
        private:
            const wchar_t* LogTag = L"MIDI virtual device ";
            LPVM_MIDI_PORT midi_port__;

        public:

            MidiControllerVirtual() = delete;
            MidiControllerVirtual(std::wstring& name);
            ~MidiControllerVirtual();
            virtual const bool Start() override;
            virtual void Stop() override;
            virtual void Dispose() override;
            virtual const bool SendToPort(Mackie::MIDIDATA& m, DWORD&) override;
        };
    }
}