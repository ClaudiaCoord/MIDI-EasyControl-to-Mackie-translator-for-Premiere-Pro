/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MMTPLUGINx* DLL (SmartHomeKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace PLUGINS {

        using namespace std::string_view_literals;

        class SmartHomeSetupDialog : public IO::PluginUi {
        private:
            MQTT::BrokerConfig<std::wstring> config_{};
            IO::PluginCb& cb_;

            void dispose_();
            void init_();
            
            void changeOnEnable_();
            void changeOnSaveConfig_();
            void changeOnLogLevel_();
            void changeOnSsl_();
            void changeOnSelfSign_();
            void changeOnIpAddress_();
            void changeOnPort_();
            void changeOnLogin_();
            void changeOnPass_();
            void changeOnPsk_();
            void changeOnCa_();
            void changeOnPrefix_();

            void buildLogLevelComboBox_(const int32_t);

        public:

            SmartHomeSetupDialog(IO::PluginCb&);
            SmartHomeSetupDialog(const SmartHomeSetupDialog&) = default;
            ~SmartHomeSetupDialog();

            IO::PluginUi* GetPluginUi();

            LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM) override final;
        };
    }
}