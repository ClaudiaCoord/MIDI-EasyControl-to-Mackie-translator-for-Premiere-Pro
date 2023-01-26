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

        class MidiBridge
        {
        private:

            static MidiBridge ctrlmidibridge__;

            const wchar_t* LogTag = L"MIDI bridge ";
            bool isenable__;
            bool isproxy__;

            std::unique_ptr<bridge_in_event> in_event__;
            std::unique_ptr<bridge_out_event> out_event__;

            const bool start_();
            const bool start__();
            void Dispose();

        public:

            MidiBridge();
            FLAG_EXPORT ~MidiBridge();

            FLAG_EXPORT static MidiBridge& Get();

            FLAG_EXPORT const bool IsEnable();
            FLAG_EXPORT std::wstring GetVirtualDriverVersion();

            FLAG_EXPORT std::vector<std::wstring>& GetInputDeviceList();
            FLAG_EXPORT std::vector<std::wstring>& GetOutputDeviceList();
            FLAG_EXPORT std::vector<std::wstring>& GetProxyDeviceList();

            FLAG_EXPORT const bool Start(std::wstring = L"");
            FLAG_EXPORT const bool Stop();

            FLAG_EXPORT void SetCallbackIn(MidiControllerBase& mi);
            FLAG_EXPORT void SetCallbackOut(MidiControllerBase& mcb);
            FLAG_EXPORT void SetCallbackOut(MidiInstance& mi);
            FLAG_EXPORT void RemoveCallbackOut(uint32_t id);
            FLAG_EXPORT void RemoveCallbackOut(MidiInstance& mi);
            FLAG_EXPORT void RemoveCallbackOut(MidiControllerBase& mcb);
            FLAG_EXPORT void RemoveCallbackIn(MidiControllerBase& mcb);
        };
    }
}

