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

        class FLAG_EXPORT MidiDevices {
        private:
            static MidiDevices ctrlmididevices__;
            static constexpr std::wstring_view LogTag = L"MIDI devices "sv;
            std::shared_ptr<MidiDriver> mdrv_;
            std::vector<std::wstring> device_in_list_{};
            std::vector<std::wstring> device_out_list_{};
            std::vector<std::wstring> device_proxy_list_{};

        public:

            MidiDevices();
            ~MidiDevices();

            static MidiDevices& Get();

            const bool CheckVirtualDriver();
            std::wstring GetVirtualDriverVersion();
            std::shared_ptr<MidiDriver>& GetMidiDriver();
            std::vector<std::wstring>& GetMidiInDeviceList(bool = false);
            std::vector<std::wstring>& GetMidiOutDeviceList(bool = false);
            std::vector<std::wstring>& GetMidiProxyDeviceList();
        };
    }
}

