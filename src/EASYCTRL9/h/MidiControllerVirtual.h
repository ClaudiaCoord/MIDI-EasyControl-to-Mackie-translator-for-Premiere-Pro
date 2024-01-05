/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	EASYCTRL9

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace MIDI {

        using namespace std::string_view_literals;

        class FLAG_EXPORT MidiControllerVirtual : public MidiControllerBase {
        private:
            static constexpr std::wstring_view LogTag = L"MIDI virtual device "sv;
            LPVM_MIDI_PORT midi_port_;
            void dispose_();

        public:

            MidiControllerVirtual() = delete;
            MidiControllerVirtual(IO::PluginCb&, std::shared_ptr<MidiDriver>&, std::wstring);
            ~MidiControllerVirtual();

            void       Stop() override final;
            const bool Start() override final;
            const bool Start(std::shared_ptr<JSON::MMTConfig>&) override final;
            void SendToPort(MIDI::Mackie::MIDIDATA& m, DWORD&) override final;
            void SendToPort(MIDI::MidiUnit&, DWORD&) override final;
        };
    }
}