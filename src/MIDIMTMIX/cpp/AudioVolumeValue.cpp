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

        AudioVolumeValue::AudioVolumeValue() : mute(false), vo(1.0f), vn(0.0f) {}
        AudioVolumeValue::AudioVolumeValue(float v) : mute(false), vo(v), vn(0.0f) {}

        bool  AudioVolumeValue::GetMute() { return mute; }
        void  AudioVolumeValue::SetMute(bool m) { mute = m; }

        float AudioVolumeValue::GetVolume() { return vn; }
        float AudioVolumeValue::GetOldVolume() { return vo; }
        float AudioVolumeValue::IncrementVolume(bool b) {
            vo = vn;
            vn = (b ? (vn + 0.1f) : (vn - 0.1f));
            vn = ((vn > 1.0f) ? 1.0f : (((vn < 0.0f) || std::isnan(vn)) ? 0.0f : vn));
            return vn;
        }
        void  AudioVolumeValue::SetVolume(float v) {
            vo = vn;
            vn = ((v > 1.0f) ? 1.0f : ((v < 0.0f) ? 0.0f : v));
        }
        void  AudioVolumeValue::SetVolume(uint8_t v) {
            uint8_t vu = (v > 100) ? 100 : v;
            float vf = static_cast<float>(vu * 0.01);
            SetVolume(vf);
        }
        void  AudioVolumeValue::Copy(AudioVolumeValue& v) {
            vo = v.vo;
            vn = v.vn;
            mute = v.mute;
        }
        void  AudioVolumeValue::Set(float v, bool m) {
            SetVolume(v);
            SetMute(m);
        }
    }
}
