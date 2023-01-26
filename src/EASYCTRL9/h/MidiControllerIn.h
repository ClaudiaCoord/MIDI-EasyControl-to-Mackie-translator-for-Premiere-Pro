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

        class FLAG_EXPORT MidiControllerIn : public MidiControllerBase {
        private:
            const wchar_t* LogTag = L"MIDI Input ";
            HMIDIIN midi_in_handle__;

            virtual bool BuildDeviceList(uint32_t&) override;

        public:

            MidiControllerIn();
            ~MidiControllerIn();
            static MidiControllerIn& Get() noexcept;

            virtual const bool Start(std::shared_ptr<MidiDevice>& cnf) override;
            virtual void Stop() override;
            virtual void Dispose() override;

            std::vector<std::wstring>& GetReBuildDeviceList();
        };
    }
}