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

        AudioSessionEventsBuilder::AudioSessionEventsBuilder() : ptr_se__(nullptr), ptr_ae__(nullptr), isdelete_se__(false), isdelete_ae__(false) {}
        void AudioSessionEventsBuilder::Release() {
            #if defined(__NO_SAFE_REF_DELETE)
            if ((ptr_se__ != nullptr) && !isdelete_se__) {
                delete ptr_se__;
                ptr_se__ = nullptr;
            }
            if ((ptr_ae__ != nullptr) && !isdelete_ae__) {
                delete ptr_ae__;
                ptr_ae__ = nullptr;
            }
            #endif
        }

        /* IAudioSessionEvents event */

        AudioSessionEvents* AudioSessionEventsBuilder::GetSe() {
            return ptr_se__;
        }
        AudioSessionEvents* AudioSessionEventsBuilder::GetSe(AudioSessionList* aslist, std::function<const GUID()> fnguid) {
            if (isdelete_se__) return nullptr;
            if (ptr_se__ == nullptr)
                ptr_se__ = new AudioSessionEvents(aslist, [&]() { isdelete_se__ = true; }, fnguid);
            return ptr_se__;
        }
        bool AudioSessionEventsBuilder::IsValidSe() {
            return (!isdelete_se__) && (ptr_se__ != nullptr);
        }

        /* IAudioEndpointVolumeCallback event */

        AudioEndPointEvents* AudioSessionEventsBuilder::GetAe() {
            return ptr_ae__;
        }
        AudioEndPointEvents* AudioSessionEventsBuilder::GetAe(AudioSessionList* aslist, std::function<const GUID()> fnguid) {
            if (isdelete_ae__) return nullptr;
            if (ptr_ae__ == nullptr)
                ptr_ae__ = new AudioEndPointEvents(aslist, [&]() { isdelete_ae__ = true; }, fnguid);
            return ptr_ae__;
        }
        bool AudioSessionEventsBuilder::IsValidAe() {
            return (!isdelete_ae__) && (ptr_ae__ != nullptr);
        }
    }
}