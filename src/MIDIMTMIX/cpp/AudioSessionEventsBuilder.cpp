/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
    namespace MIXER {

        AudioSessionEventsBuilder::AudioSessionEventsBuilder() : ptrse__(nullptr), ptrae__(nullptr), isdeletese__(false), isdeleteae__(false) {}
        void AudioSessionEventsBuilder::Release() {
            if ((ptrse__ != nullptr) && !isdeletese__) {
                delete ptrse__;
                ptrse__ = nullptr;
            }
            if ((ptrae__ != nullptr) && !isdeleteae__) {
                delete ptrae__;
                ptrae__ = nullptr;
            }
        }

        /* IAudioSessionEvents event */

        AudioSessionEvents* AudioSessionEventsBuilder::GetSe() {
            return ptrse__;
        }
        AudioSessionEvents* AudioSessionEventsBuilder::GetSe(AudioSessionList* aslist, std::function<const GUID()> fnguid) {
            if (isdeletese__) return nullptr;
            if (ptrse__ == nullptr)
                ptrse__ = new AudioSessionEvents(aslist, [&]() { isdeletese__ = true; }, fnguid);
            return ptrse__;
        }
        bool AudioSessionEventsBuilder::IsValidSe() {
            return (!isdeletese__) && (ptrse__ != nullptr);
        }

        /* IAudioEndpointVolumeCallback event */

        AudioEndPointEvents* AudioSessionEventsBuilder::GetAe() {
            return ptrae__;
        }
        AudioEndPointEvents* AudioSessionEventsBuilder::GetAe(AudioSessionList* aslist, std::function<const GUID()> fnguid) {
            if (isdeleteae__) return nullptr;
            if (ptrae__ == nullptr)
                ptrae__ = new AudioEndPointEvents(aslist, [&]() { isdeleteae__ = true; }, fnguid);
            return ptrae__;
        }
        bool AudioSessionEventsBuilder::IsValidAe() {
            return (!isdeleteae__) && (ptrae__ != nullptr);
        }
    }
}