/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	COMMON::MIDI

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
    namespace MIDI {

        class FLAG_EXPORT MidiUnitValue {
        public:
            bool lvalue{ false };
            uint8_t value{ 0U };
            std::chrono::steady_clock::time_point time{};
            Mackie::ClickType type{ Mackie::ClickType::ClickUnknown };

            MidiUnitValue();
            MidiUnitValue(uint8_t v, uint32_t t);
            const bool empty() const;
            std::wstring dump();
            void copy(MidiUnitValue&);
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
            MixerUnit(MidiUnit&);
            void ToNull(bool = false);
            bool EqualsOR(MixerUnit&);
            bool EqualsAND(MixerUnit&);
            std::wstring dump();
            void copy(MidiUnit&);
            void copy(MixerUnit&);
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
            MidiUnit(MixerUnit&);
            ~MidiUnit() = default;
            const bool empty() const;
            const uint32_t GetMixerId();
            MixerUnit GetMixerUnit();
            std::wstring dump();
            void copy(MidiUnit&);
            void copy(MixerUnit&);
        };

        class FLAG_EXPORT MidiUnitRef {
        private:
            static MidiUnit midiunitdefault_;
            MidiUnit& m_;
            IO::PluginClassTypes type_{ IO::PluginClassTypes::ClassNone };
            bool isbegin_{ false };

        public:
            MidiUnitRef();
            MidiUnit& get();
            void set(MidiUnit&, IO::PluginClassTypes);
            void begin();
            const IO::PluginClassTypes type();
            const bool isbegin();
            const bool isvalid();

            friend void copy_data(MidiUnitRef&, MidiUnit&);
            friend void copy_data(MidiUnitRef&, MixerUnit&);
        };

        class FLAG_EXPORT MidiConfig {
        private:
            void copysettings_(MidiConfig&);
        public:
            bool enable{ false };
            bool out_system_port{ false };
            bool jog_scene_filter{ true };
            uint32_t out_count{};
            uint32_t proxy_count{};
            uint32_t btn_interval{};
            uint32_t btn_long_interval{};
            std::vector<std::wstring> midi_in_devices{};
            std::vector<std::wstring> midi_out_devices{};

            std::chrono::milliseconds get_interval() const;
            std::chrono::milliseconds get_long_interval() const;

            const bool empty() const;
            void Copy(MidiConfig&);
            std::wstring dump();
        };

        class FLAG_EXPORT MMKeyConfig {
        public:
            bool enable{ false };

            const bool empty() const;
            void Copy(MMKeyConfig&);
            std::wstring dump();
        };

        class FLAG_EXPORT MidiSetter {
        public:
            static bool ValidTarget(MidiUnit& u);
            static bool ÑhatterButton(MidiUnit& u, Mackie::MIDIDATA& m, const std::chrono::milliseconds& btninterval);
            static bool ÑhatterButton(MidiUnit& u, Mackie::MIDIDATA& m, const std::chrono::steady_clock::time_point t, const std::chrono::milliseconds& btninterval);
            static void SetButton(MidiUnit& u);
            static bool SetVolume(MidiUnit& u, uint8_t val, bool = true);
            static bool SetVolume(MidiUnit& u, const std::chrono::steady_clock::time_point t, uint8_t val, bool = true);
        };
    }
}

typedef std::function<const bool(Common::MIDI::Mackie::MIDIDATA&, DWORD&)> callMidiOut1Cb;
typedef std::function<const bool(Common::MIDI::MidiUnit&, DWORD&)>         callMidiOut2Cb;
typedef std::function<void(DWORD, DWORD)>                                  callMidiInCb;
