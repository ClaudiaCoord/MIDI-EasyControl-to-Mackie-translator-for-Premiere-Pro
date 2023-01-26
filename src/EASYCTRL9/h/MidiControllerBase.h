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

        class FLAG_EXPORT MidiControllerBase
        {
        protected:
            bool isenable__;
            bool isconnect__;
            uint32_t id__;

            callMidiInCb eventin__;
            std::wstring active_device__;
            std::vector<std::wstring> device_list__;
            ClassTypes this_type__;

            virtual bool BuildDeviceList(uint32_t&);

            MidiControllerBase();
        public:

            ~MidiControllerBase();
            virtual const bool Start();
            virtual const bool Start(std::shared_ptr<MidiDevice>& cnf);
            virtual void Stop();
            virtual void Dispose();

            std::wstring& DeviceName();
            std::vector<std::wstring>& GetDeviceList();

            const bool IsEmpty();
            const bool IsEnable();
            const bool IsConnected();

            void InCallbackSet(callMidiInCb f);
            void InCallbackRemove();

            uint32_t   GetId();
            ClassTypes GetType();

            // CALLBACK OUT GETTER //
            callMidiOut1Cb GetCbOut1();
            callMidiOut2Cb GetCbOut2();

            // CALLBACK TO PORT //
            virtual const bool SendToPort(Mackie::MIDIDATA& m, DWORD&);
            virtual const bool SendToPort(MidiUnit&, DWORD&);

            // CALLBACK TO INPUT-OUTPUT //
            static void CALLBACK MidiInProc(HMIDIIN, UINT, DWORD_PTR, DWORD, DWORD);
            static void CALLBACK MidiOutProc(HMIDIOUT, UINT, DWORD_PTR, DWORD, DWORD);
            static void MidiProc(UINT, DWORD_PTR, DWORD, DWORD);
        };
    }
}

