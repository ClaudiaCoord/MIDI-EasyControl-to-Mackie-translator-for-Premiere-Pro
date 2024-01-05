/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace IO {

        class bridge_sys : public PluginCb {
        public:
            bridge_sys();
            ~bridge_sys();

			void SetCbConfig(PluginCb*) override final;
			void RemoveCbConfig(uint32_t) override final;

			void SetCbOutLog(PluginCb*) override final;
			void RemoveCbOutLog(uint32_t) override final;

			PluginCb& GetCb();
        };
    }
}

