/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#include "..\pch.h"

MidiUnitValue::MidiUnitValue() : value(255), time(0), lvalue(false), type(Mackie::ClickType::ClickUnknown) {}
MidiUnitValue::MidiUnitValue(uint8_t v, uint32_t t) { value = v; time = t; lvalue = false; type = Mackie::ClickType::ClickUnknown; }
MidiUnit::MidiUnit() : scene(255), id(255), type(UNITNONE), target(Mackie::Target::NOTARGET), longtarget(Mackie::Target::NOTARGET) {}
MidiDevice::MidiDevice() : name(""), autostart(false), manualport(false), proxy(0), btninterval(100U), btnlonginterval(1500U), count(1) { units = new MidiUnit[1]{}; }
MidiDevice::~MidiDevice() { if (units != nullptr) delete[] units; }
size_t MidiDevice::Count() { return count; }
void MidiDevice::Init(size_t c) {
	if (units != nullptr) delete[] units;
	count = c;
	units = new MidiUnit[count]{};
}
void MidiDevice::Clear() {
	if (count != 1) Init(1);
}
std::string MidiUnit::Print() {
	return LogString() << "\tid:" << static_cast<uint32_t>(id) 
		<< "\n\t\ttarget:" << static_cast<uint32_t>(target) << ", long press target:" << static_cast<uint32_t>(longtarget)
		<< "\n\t\tpress type:" << static_cast<uint32_t>(value.type)
		<< "\n\t\tvalue:" << static_cast<uint32_t>(value.value) << ", bool value:" << value.lvalue
		<< "\n\t\ttime:" << value.time;
}
