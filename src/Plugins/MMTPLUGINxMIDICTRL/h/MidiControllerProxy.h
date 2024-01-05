/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MMTPLUGINxMIDIDEV - Virtual Midi proxy controller

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace MIDI {

        class MidiControllerProxy : public MidiControllerBase {
        private:
            static constexpr std::wstring_view LogTag = L"MIDI proxy port "sv;
            std::vector<std::shared_ptr<MIDI::MidiControllerVirtual>> vmdev_ptr_{};
            uint32_t proxy_count_{ 0U };

            void dispose_();

        public:

            MidiControllerProxy(IO::PluginCb&, std::shared_ptr<MIDI::MidiDriver>&, std::wstring);
            ~MidiControllerProxy();

            void SetProxyCount(uint32_t);
            const uint32_t GetProxyCount();
            const uint32_t GetRunningProxyCount();

            void       Stop() override final;
            const bool Start() override final;
            const bool Start(std::shared_ptr<JSON::MMTConfig>&) override final;
            void SendToPort(MIDI::Mackie::MIDIDATA&, DWORD&) override final;
            void SendToPort(MIDI::MidiUnit&, DWORD&) override final;

        };
    }
}