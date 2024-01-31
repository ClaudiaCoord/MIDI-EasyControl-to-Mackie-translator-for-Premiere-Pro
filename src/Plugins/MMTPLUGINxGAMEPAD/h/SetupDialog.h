/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Joystick support.
	+ GamePad support.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (GamePad/Joistick Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace PLUGINS {

        using namespace std::string_view_literals;

        class GamePadSetupDialog : public IO::PluginUi {
        private:
            hwnd_ptr<empty_deleter> mhwnd_{};
            GAMEPAD::JoystickConfig config_{};
            GAMEPAD::JoisticksBase& jb_;
            IO::PluginCb& cb_;

            void dispose_();
            void init_();

            void changeOnEnable_();
            void changeOnAsButton_();
            void changeOnHorizontalMode_();
            void changeOnRole_(uint16_t, bool = true);
            void changeOnType_();
            void changeOnScene_();
            void changeOnStep_();
            void changeOnSlider_();
            void changeOnSaveConfig_();

            void buildTypesComboBox_(int32_t);
            void buildListView_();

        public:

            GamePadSetupDialog(IO::PluginCb&, GAMEPAD::JoisticksBase&, HWND);
            GamePadSetupDialog(const GamePadSetupDialog&) = default;
            ~GamePadSetupDialog();

            IO::PluginUi* GetPluginUi();

            LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM) override final;
        };
    }
}