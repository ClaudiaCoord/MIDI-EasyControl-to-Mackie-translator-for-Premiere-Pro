/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	COMMON::MIDI

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace MIDI {

        class FLAG_EXPORT MidiInstance
        {
        protected:
            uint32_t       id__;
            ClassTypes     type__;
            logFnType      log__;
            callMidiOut1Cb out1__;
            callMidiOut2Cb out2__;
            callMidiInCb   in__;
        public:

            MidiInstance();
            const uint32_t GetId() const;
            ClassTypes GetType();
            callMidiOut1Cb GetCbOut1();
            callMidiOut2Cb GetCbOut2();
            logFnType      GetCbLog();

            void InCallbackSet(callMidiInCb f);
            void InCallbackRemove();

            template<class T1>
            MidiInstance(T1& clz) { Set(clz); }

            template<class T1>
            void Set(T1& clz) {
                type__ = clz.GetType();
                out1__ = clz.GetCbOut1();
                out2__ = clz.GetCbOut2();
                id__   = clz.GetId();
            }
            template<class T1 = ClassTypes>
            void Set(T1 t, callMidiOut1Cb cb1, callMidiOut2Cb cb2) {
                type__ = t;
                out1__ = cb1;
                out2__ = cb2;
                #pragma warning( push )
                #pragma warning( disable : 4302 4311 )
                id__ = reinterpret_cast<uint32_t>(this);
                #pragma warning( pop )
            }
            template<class T1 = ClassTypes>
            void Set(T1 t, callMidiOut1Cb cb1, callMidiOut2Cb cb2, logFnType cb3) {
                type__ = t;
                out1__ = cb1;
                out2__ = cb2;
                log__ = cb3;
                #pragma warning( push )
                #pragma warning( disable : 4302 4311 )
                id__ = reinterpret_cast<uint32_t>(this);
                #pragma warning( pop )
            }
        };
    }
}

