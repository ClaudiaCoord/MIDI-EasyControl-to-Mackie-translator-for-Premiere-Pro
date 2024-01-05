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

        class FLAG_EXPORT MidiControllerBase {
        protected:
            bool isenable_{ false };
            bool isconnect_{ false };

            std::wstring active_device_{};
            std::shared_ptr<MidiDriver>& mdrv_;
            IO::PluginCb& cb_;
            std::shared_mutex mtx_{};

            MidiControllerBase(IO::PluginCb&, std::shared_ptr<MIDI::MidiDriver>&);
            MidiControllerBase(IO::PluginCb&, std::shared_ptr<MIDI::MidiDriver>&, std::wstring);
        public:

            #pragma region VIRTUAL
            virtual ~MidiControllerBase() = default;
            virtual void       Stop() = 0;
            virtual const bool Start() = 0;
            virtual const bool Start(std::shared_ptr<JSON::MMTConfig>& cnf) = 0;

            #pragma region CALLBACK TO PORT
            virtual void SendToPort(MIDI::Mackie::MIDIDATA&, DWORD&) = 0;
            virtual void SendToPort(MIDI::MidiUnit&, DWORD&) = 0;
            #pragma endregion

            #pragma endregion

            std::wstring& GetActiveDevice();

            const bool IsEmpty();
            const bool IsEnable();
            const bool IsConnected();


        protected:
            #pragma region CALLBACK TO INPUT-OUTPUT
            static void CALLBACK MidiInProc_(HMIDIIN, UINT, DWORD_PTR, DWORD, DWORD);
            static void CALLBACK MidiOutProc_(HMIDIOUT, UINT, DWORD_PTR, DWORD, DWORD);
            static void CALLBACK MidiProc_(UINT, DWORD_PTR, DWORD, DWORD);
            #pragma endregion
        };
    }
}

