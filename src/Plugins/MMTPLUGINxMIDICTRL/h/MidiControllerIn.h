/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MMTPLUGINxMIDIDEV - Midi input controller

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace MIDI {

        using namespace std::string_view_literals;

        class MidiControllerIn : public MidiControllerBase {
        private:
            static constexpr std::wstring_view LogTag = L"MIDI Input "sv;
            HMIDIIN midi_in_handle_{ nullptr };

            void dispose_();

        public:

            MidiControllerIn(IO::PluginCb&, std::shared_ptr<MIDI::MidiDriver>&, std::wstring);
            ~MidiControllerIn();

            void       Stop() override final;
            const bool Start() override final;
            const bool Start(std::shared_ptr<JSON::MMTConfig>&) override final;
            void SendToPort(MIDI::Mackie::MIDIDATA&, DWORD&) override final;
            void SendToPort(MIDI::MidiUnit&, DWORD&) override final;
        };
    }
}