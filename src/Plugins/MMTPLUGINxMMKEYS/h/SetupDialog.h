/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (MultimediaKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace PLUGINS {

        using namespace std::string_view_literals;

        class MMKeysSetupDialog : public IO::PluginUi {
        private:
            hwnd_ptr<empty_deleter> mhwnd_{};
            MIDI::MMKeyConfig config_{};
            IO::PluginCb& cb_;

            void dispose_();
            void init_();

            void changeOnEnable_();
            void changeOnSaveConfig_();

        public:

            MMKeysSetupDialog(IO::PluginCb&, HWND);
            MMKeysSetupDialog(const MMKeysSetupDialog&) = default;
            ~MMKeysSetupDialog();

            IO::PluginUi* GetPluginUi();

            LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM) override final;
        };
    }
}