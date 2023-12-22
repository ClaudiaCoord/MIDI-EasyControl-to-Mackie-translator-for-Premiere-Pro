/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MMTPLUGINxMIDIDEV - Midi output controller

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace MIDI {

        using namespace std::string_view_literals;

        class MidiControllerOut : public MidiControllerBase {
        private:
            static constexpr std::wstring_view LogTag = L"MIDI Output "sv;
            bool issystemport_{ false };
            HMIDIOUT midi_out_handle_{ nullptr };
            std::unique_ptr<MIDI::MidiControllerVirtual> vmdev_ptr_{};

            void dispose_();

        public:

            MidiControllerOut(IO::PluginCb&, std::shared_ptr<MIDI::MidiDriver>&, std::wstring);
            ~MidiControllerOut();

            void       Stop() override final;
            const bool Start() override final;
            const bool Start(std::shared_ptr<JSON::MMTConfig>&) override final;
            void SendToPort(MIDI::Mackie::MIDIDATA&, DWORD&) override final;
            void SendToPort(MIDI::MidiUnit&, DWORD&) override final;

            bool IsSystemPort() const;
            void IsSystemPort(bool, std::wstring = L"");
        };
    }
}
