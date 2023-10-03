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

        class MidiControllerProxy : public MidiControllerBase {
        private:
            const wchar_t* LogTag = L"MIDI proxy port ";
            std::vector<std::shared_ptr<MidiControllerVirtual>> vmdev_ptr__{};

        public:

            MidiControllerProxy(std::shared_ptr<MidiDriver>);
            ~MidiControllerProxy();

            virtual const bool Start(std::shared_ptr<MidiDevice>& cnf) override;
            virtual void Stop() override;
            virtual void Dispose() override;
            virtual const bool SendToPort(Mackie::MIDIDATA& m, DWORD&) override;

            const uint32_t GetProxyCount();
        };
    }
}