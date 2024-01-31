/*
    MIDI-MT is a high level application driver for USB MIDI control surface.
    (c) CC 2023-2024, MIT

    MMTPLUGINx* DLL (Remote Controls Plugin)

    See README.md for more details.
    NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#pragma once

namespace Common {
    namespace PLUGINS {

        using namespace std::string_view_literals;

        class RemoteSetupDialog : public IO::PluginUi {
        private:
            hwnd_ptr<empty_deleter> mhwnd_{};
            REMOTE::RemoteConfig<std::wstring> config_{};
            IO::PluginCb& cb_;
            UI::ImageStateButton<HICON> btn_copy_{};

            void dispose_();
            void init_();

            void changeOnEnable_();
            void changeOnIpv6_();
            void changeOnFastSocket_();
            void changeOnReuseAddr_();
            void changeOnPort_();
            void changeOnTmc_();
            void changeOnTmi_();
            void changeOnLogLevel_();
            void changeOnNetInterface_();
            void changeOnCopyServerUrl_(bool = false);
            void changeOnSaveConfig_();

            std::vector<std::wstring> buildNetInterfaceList_();
            void buildNetInterfaceComboBox_(const std::wstring&);
            void buildLogLevelComboBox_(uint16_t);
            std::wstring buildServerUrl_(const std::wstring&, uint16_t, bool = true);

        public:

            RemoteSetupDialog(IO::PluginCb&, HWND);
            RemoteSetupDialog(const RemoteSetupDialog&) = default;
            ~RemoteSetupDialog();

            IO::PluginUi* GetPluginUi();

            LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM) override final;
        };
    }
}