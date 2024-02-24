/*
    MIDI-MT is a high level application driver for USB MIDI control surface.
    (c) CC 2023-2024, MIT

    MMTPLUGINx* DLL (VM script plugin)

    See README.md for more details.
    NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#pragma once

namespace Common {
    namespace PLUGINS {

        using namespace std::string_view_literals;

        class VmScriptSetupDialog : public IO::PluginUi {
        private:
            std::atomic<bool> build_list_{ false };
            hwnd_ptr<empty_deleter> mhwnd_{};
            SCRIPT::VmScriptConfig config_{};
            std::vector<std::wstring> script_list_{};
            std::shared_ptr<SCRIPT::VmScripts>& vmscripts_;
            IO::PluginCb& cb_;
            const uint32_t id_;

            void dispose_();
            void init_();
            void build_script_list_(SCRIPT::scripts_list_t&);

            void changeOnEnable_();
            void changeOnWatchDirectory_();
            void changeOnSelectDirectory_();
            void changeOnScriptList_();
            void changeOnSaveConfig_();

        public:

            VmScriptSetupDialog(IO::PluginCb&, std::shared_ptr<SCRIPT::VmScripts>&, HWND);
            VmScriptSetupDialog(const VmScriptSetupDialog&) = default;
            ~VmScriptSetupDialog();

            IO::PluginUi* GetPluginUi();

            LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM) override final;
        };
    }
}