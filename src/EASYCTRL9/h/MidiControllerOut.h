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

        class FLAG_EXPORT MidiControllerOut : public MidiControllerBase {
        private:
            const wchar_t* LogTag = L"MIDI Output ";
            bool ismanualport__;
            HMIDIOUT midi_out_handle__;
            std::unique_ptr <MidiControllerVirtual> vmdev_ptr__{};

            virtual bool BuildDeviceList(uint32_t&) override;
            void Dispose(bool);

        public:

            MidiControllerOut();
            ~MidiControllerOut();
            static MidiControllerOut& Get();

            virtual const bool Start(std::shared_ptr<MidiDevice>& cnf) override;
            virtual void Stop() override;
            virtual void Dispose() override;
            virtual const bool SendToPort(MidiUnit&, DWORD&) override;

            bool IsManualPort();
            void IsManualPort(bool b);
        };
    }
}
