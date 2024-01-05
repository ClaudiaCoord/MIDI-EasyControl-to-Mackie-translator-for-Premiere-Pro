/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace MIXER {

        class AudioSessionEventsBuilder
        {
        private:
            Common::MIXER::AudioSessionEvents*  ptr_se_;
            Common::MIXER::AudioEndPointEvents* ptr_ae_;
            std::atomic<bool> isdelete_se_;
            std::atomic<bool> isdelete_ae_;

        public:

            AudioSessionEventsBuilder();
            AudioSessionEvents*  GetSe();
            AudioSessionEvents*  GetSe(AudioSessionList*, std::function<const GUID()>);
            AudioEndPointEvents* GetAe();
            AudioEndPointEvents* GetAe(AudioSessionList*, std::function<const GUID()>);
            bool IsValidSe();
            bool IsValidAe();
            [[deprecated("Ahtung!")]] void Release();
        };
    }
}
