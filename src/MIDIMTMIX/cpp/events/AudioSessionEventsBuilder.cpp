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

        AudioSessionEventsBuilder::AudioSessionEventsBuilder() : ptr_se_(nullptr), ptr_ae_(nullptr), isdelete_se_(false), isdelete_ae_(false) {}
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
            return ptr_se_;
        }
        AudioSessionEvents* AudioSessionEventsBuilder::GetSe(AudioSessionList* aslist, std::function<const GUID()> fnguid) {
            if (isdelete_se_) return nullptr;
            if (ptr_se_ == nullptr)
                ptr_se_ = new AudioSessionEvents(aslist, [&]() { isdelete_se_ = true; }, fnguid);
            return ptr_se_;
        }
        bool AudioSessionEventsBuilder::IsValidSe() {
            return (!isdelete_se_) && (ptr_se_ != nullptr);
        }

        /* IAudioEndpointVolumeCallback event */

        AudioEndPointEvents* AudioSessionEventsBuilder::GetAe() {
            return ptr_ae_;
        }
        AudioEndPointEvents* AudioSessionEventsBuilder::GetAe(AudioSessionList* aslist, std::function<const GUID()> fnguid) {
            if (isdelete_ae_) return nullptr;
            if (ptr_ae_ == nullptr)
                ptr_ae_ = new AudioEndPointEvents(aslist, [&]() { isdelete_ae_ = true; }, fnguid);
            return ptr_ae_;
        }
        bool AudioSessionEventsBuilder::IsValidAe() {
            return (!isdelete_ae_) && (ptr_ae_ != nullptr);
        }
    }
}