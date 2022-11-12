/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

enum MidiUnitType : uint8_t {
    FADER,
    SLIDER,
    KNOB,
    BTN,
    BTNTOGGLE,
    KNOBINVERT,
    UNITNONE = 255
};

enum MidiUnitScene : uint8_t {
    SC1 = 176,
    SC2 = 177,
    SC3 = 178,
    SC4 = 179,
    SC1KNOB = 192,
    SC2KNOB = 193,
    SC3KNOB = 194,
    SC4KNOB = 211,
    SC4BTN  = 243,
    SCNONE = 255
};

struct FLAG_EXPORT MidiUnitValue {
    bool lvalue;
    uint8_t value;
    uint32_t time;
    Mackie::ClickType type;
public:
    MidiUnitValue();
    MidiUnitValue(uint8_t v, uint32_t t);
};
struct FLAG_EXPORT MidiUnit {
    uint8_t scene;
    uint8_t id;
    MidiUnitType type;
    Mackie::Target target;
    Mackie::Target longtarget;
    MidiUnitValue value;
public:
    MidiUnit();
    std::string Print();
};
#pragma warning( push )
#pragma warning( disable : 4251 )
struct FLAG_EXPORT MidiDevice {
    std::string name;
    std::string config;
    bool autostart;
    bool manualport;
    uint32_t proxy;
    uint32_t btninterval;
    uint32_t btnlonginterval;
    MidiUnit *units;
private:
    size_t count;
public:
    MidiDevice();
    ~MidiDevice();
    size_t Count();
    void Init(size_t c);
    void Clear();
};
#pragma warning( pop )
