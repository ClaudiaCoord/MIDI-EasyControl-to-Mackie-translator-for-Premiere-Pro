/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MMTPLUGINx* DLL (MidiCtrlPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace PLUGINS {

        using namespace std::string_view_literals;

        class MidiCtrlSetupDialog : public IO::PluginUi {
        private:
            MIDI::MidiConfig config_{};
            IO::PluginCb& cb_;

            void dispose_();
            void init_();
            void setOutPort_(HWND, const uint32_t);

            void changeOnEnable_();
            void changeOnSaveConfig_();
            void changeOnJogfilter_();
            void changeOnSystemPort_();
            void changeOnSliders_();
            void changeOnProxy_();
            void changeOnOut_();
            void changeOnListViewCheck_();

            void buildDeviceListBox_(const bool, bool = false);
            void buildProxyComboBox_(const uint32_t);
            void buildOutComboBox_(const uint32_t);

        public:

            MidiCtrlSetupDialog(const MidiCtrlSetupDialog&) = default;
            MidiCtrlSetupDialog(IO::PluginCb&);
            ~MidiCtrlSetupDialog();

            IO::PluginUi* GetPluginUi();

            LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM) override final;
        };
    }
}