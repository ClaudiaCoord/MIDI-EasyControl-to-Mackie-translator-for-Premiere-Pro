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
            MidiUnitValue(const MidiUnitValue&) = default;
            ~MidiUnitValue() = default;

            const bool empty() const;
            std::wstring dump() const;
            void copy(const MidiUnitValue&);
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
            std::vector<std::wstring> apps;

            MixerUnit();
            MixerUnit(const MidiUnit&);
            MixerUnit(const MixerUnit&) = default;
            ~MixerUnit() = default;

            void ToNull(bool = false);
            bool EqualsOR(const MixerUnit&);
            bool EqualsAND(const MixerUnit&);
            std::wstring dump() const;
            void copy(const MidiUnit&);
            void copy(const MixerUnit&);
        };

        class FLAG_EXPORT MidiUnit {
        public:
            uint8_t key;
            uint8_t scene;
            MidiUnitType type;
            Mackie::Target target;
            Mackie::Target longtarget;
            MidiUnitValue value;
            std::vector<std::wstring> apps;

            MidiUnit();
            MidiUnit(const MixerUnit&);
            MidiUnit(const MidiUnit&) = default;
            ~MidiUnit() = default;

            const bool empty() const;
            const uint32_t GetMixerId() const;
            MixerUnit GetMixerUnit() const;
            std::wstring dump() const;
            void copy(const MidiUnit&);
            void copy(const MixerUnit&);
        };

        class FLAG_EXPORT MidiUnitRef {
        private:
            MidiUnit& m_;
            IO::PluginClassTypes type_{ IO::PluginClassTypes::ClassNone };
            bool isbegin_{ false };

        public:

            MidiUnitRef();
            MidiUnitRef(const MidiUnitRef&) = default;
            ~MidiUnitRef() = default;

            MidiUnit& get();
            void set(MidiUnit&, IO::PluginClassTypes);
            void begin();
            const IO::PluginClassTypes type() const;
            const bool isbegin() const;
            const bool isvalid() const;

            friend void copy_data(MidiUnitRef&, MidiUnit&);
            friend void copy_data(MidiUnitRef&, MixerUnit&);
        };

        class FLAG_EXPORT MidiConfig {
        private:
            void copysettings_(const MidiConfig&);
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

            void copy(const MidiConfig&);
            const bool empty() const;
            std::wstring dump() const;
        };

        class FLAG_EXPORT MMKeyConfig {
        public:
            bool enable{ false };

            void copy(const MMKeyConfig&);
            const bool empty() const;
            std::wstring dump() const;
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
