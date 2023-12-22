/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MMTPLUGINx* DLL (LightKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace PLUGINS {

        using namespace std::string_view_literals;

        class LightsSetupDialog : public IO::PluginUi {
        private:
            LIGHT::LightsConfig config_{};
            IO::PluginCb& cb_;

            std::function<LIGHT::SerialPortConfigs& ()> get_serial_devices_;
            std::function<LIGHT::ArtnetConfigs& ()> get_network_interfaces_;

            void dispose_();
            void init_();

            void changeOnSaveConfig_();

            void changeOnDmxPool_();
            void changeOnDmxEnable_();
            void changeOnDmxDevice_();

            void changeOnArtnetEnable_();
            void changeOnArtnetNetwork_();
            void changeOnArtnetPort_();
            void changeOnArtnetUniverse_();

            void clearDmxConfig_();
            void clearArtnetConfig_();

            void showDmxConfig_();
            void showArtnetConfig_();

            void buildDmxDevicesComboBox(const int32_t);
            void buildArtnetInterfacesComboBox(const std::wstring&);

        public:

            LightsSetupDialog(
                IO::PluginCb&,
                std::function<LIGHT::SerialPortConfigs & ()>,
                std::function<LIGHT::ArtnetConfigs & ()>);
            ~LightsSetupDialog();

            IO::PluginUi* GetPluginUi();

            LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM) override final;
        };
    }
}