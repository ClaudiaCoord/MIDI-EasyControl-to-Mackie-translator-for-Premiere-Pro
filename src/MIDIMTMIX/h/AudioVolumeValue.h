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

        class FLAG_EXPORT AudioVolumeValue
        {
        private:
            bool mute;
            float vo, vn;

        public:
            AudioVolumeValue();
            AudioVolumeValue(float v);
            bool  GetMute();
            float GetVolume();
            float GetOldVolume();
            float IncrementVolume(bool b);
            void  Set(float v, bool m);
            void  SetVolume(float v);
            void  SetVolume(uint8_t v);
            void  SetMute(bool m);
            void  Copy(AudioVolumeValue& v);
        };
    }
}
