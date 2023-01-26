/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

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
            Common::MIXER::AudioSessionEvents*  ptrse__;
            Common::MIXER::AudioEndPointEvents* ptrae__;
            std::atomic<bool> isdeletese__;
            std::atomic<bool> isdeleteae__;

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
