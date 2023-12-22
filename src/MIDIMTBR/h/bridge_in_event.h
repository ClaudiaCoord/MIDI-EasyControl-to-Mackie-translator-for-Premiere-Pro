/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace IO {

        class bridge_in_event : public PluginCb {
        private:
            worker_background& wb_;
            std::atomic<bool> jogscenefilter_;
            std::atomic<uint64_t> tmid_{ 1 };
            std::shared_ptr<bridge_out_event>& outevent_;

            DWORD get_timeid_();
            void set_config_cb_(std::shared_ptr<JSON::MMTConfig>&);

            void cbincall1_(DWORD, DWORD);
            void cbincall2_(MIDI::Mackie::MIDIDATA, DWORD);
            void cbin_(DWORD&, DWORD&);

        public:

            bridge_in_event() = delete;
            bridge_in_event(std::shared_ptr<bridge_out_event>&);
            ~bridge_in_event();

            PluginCb& GetCb();

            const bool IsJogSceneFilter();
            void       UseJogSceneFilter(bool);
        };
    }
}

