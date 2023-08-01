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

        class FLAG_EXPORT MidiUnitValue {
        public:
            bool lvalue;
            uint8_t value;
            uint32_t time;
            Mackie::ClickType type;

            MidiUnitValue();
            MidiUnitValue(uint8_t v, uint32_t t);
            void Copy(MidiUnitValue&);
        };

        class FLAG_EXPORT MixerUnit {
        public:
            uint32_t id;
            uint8_t key;
            uint8_t scene;
            Mackie::Target target;
            Mackie::Target longtarget;
            MidiUnitType type;
            MidiUnitValue value;
            std::vector<std::wstring> appvolume;

            MixerUnit();
            void Copy(MidiUnit&);
            void Copy(MixerUnit&);
            void ToNull(bool = false);
            bool EqualsOR(MixerUnit&);
            bool EqualsAND(MixerUnit&);
            std::wstring Dump();
        };

        class FLAG_EXPORT MidiUnit {
        public:
            uint8_t key;
            uint8_t scene;
            MidiUnitType type;
            Mackie::Target target;
            Mackie::Target longtarget;
            MidiUnitValue value;
            std::vector<std::wstring> appvolume;

            MidiUnit();
            void Copy(MidiUnit&);
            void Copy(MixerUnit&);
            const bool IsEmpty() const;
            const uint32_t GetMixerId();
            MixerUnit GetMixerUnit();
            std::wstring Dump();
        };

        class FLAG_EXPORT MidiUnitRef {
        private:
            static MidiUnit midiunitdefault__;
            MidiUnit& m__;
            ClassTypes type__;
            bool isbegin__;

        public:
            MidiUnitRef();
            MidiUnit& get();
            void set(MidiUnit&, ClassTypes);
            void begin();
            const ClassTypes type();
            const bool isbegin();
            const bool isvalid();
        };

#       pragma warning( push )
#       pragma warning( disable : 4251 )
        class FLAG_EXPORT MidiDevice {
        private:
            void copysettings__(MidiDevice*);
        public:
            std::wstring name;
            std::wstring config;
            bool autostart;
            bool manualport;
            bool jogscenefilter;
            uint32_t proxy;
            uint32_t btninterval;
            uint32_t btnlonginterval;
            std::vector<MidiUnit> units;
            MQTT::BrokerConfig<std::wstring> mqttconf;

            MidiDevice* get();

            MidiDevice();
            ~MidiDevice();
            bool IsEmpty();
            void Init();
            void Add(MidiUnit);
            void Clear();
            std::wstring Dump();

            template <class T1>
            void CopySettings(T1& ptr) {
                copysettings__(ptr.get());
            }
        };
#       pragma warning( pop )

        class FLAG_EXPORT MidiSetter {
        public:
            static bool ValidTarget(MidiUnit& u);
            static bool ÑhatterButton(MidiUnit& u, Mackie::MIDIDATA& m, DWORD& t, const uint32_t& btninterval);
            static void SetButton(MidiUnit& u);
            static bool SetVolume(MidiUnit& u, DWORD& t, uint8_t val);
        };
    }
}

typedef std::function<const bool(Common::MIDI::Mackie::MIDIDATA&, DWORD&)> callMidiOut1Cb;
typedef std::function<const bool(Common::MIDI::MidiUnit&, DWORD&)>         callMidiOut2Cb;
typedef std::function<void(DWORD, DWORD)>                                  callMidiInCb;
