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

        class bridge_out_event : public PluginCb {
        private:
            std::shared_ptr<locker_awaiter> lock__;
            std::shared_ptr<JSON::MMTConfig>& cnf_;
            common_event<PluginCb*> event_{};

            void set_config_cb_(std::shared_ptr<JSON::MMTConfig>&);

        public:

            std::atomic<bool> remote_once{ false };

            bridge_out_event();
            ~bridge_out_event();

            PluginCb& GetCb();

            #pragma region PluginCbSet Interface
            void SetCbOut(PluginCb*) override;
            void RemoveCbOut(uint32_t) override;
            #pragma endregion
            
            void CbInCall(MIDI::Mackie::MIDIDATA, DWORD);

            void clear();
            
        };
    }
}

