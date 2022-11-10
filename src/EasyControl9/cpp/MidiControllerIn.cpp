/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#include "..\pch.h"

void MidiControllerIn::Initializer() {
	Initializer(nullptr);
}
void MidiControllerIn::Initializer(f_Fn_event faction) {
	__num_devices = midiInGetNumDevs();
	if (__num_devices == 0)
		throw new std::runtime_error(LogString() << LogTag << LogString::LogNotConnect);
	if (faction != nullptr)
		event = faction;
}
void MidiControllerIn::LogCallback(std::string s) {
	if (!s.empty()) __status(s);
}
MidiControllerIn::MidiControllerIn(f_Fn_event faction, StatusDelegate & fstatus, std::shared_ptr<MidiDevice> cnf) : __status(fstatus) {
	MidiControllerIn::ctrl = this;
	config_ptr = cnf;
	Initializer(faction);
}
MidiControllerIn::~MidiControllerIn() {
	try {
		Dispose();
		MidiControllerIn::ctrl = nullptr;
	}
	catch (...) {}
}
void MidiControllerIn::Dispose() {
	if (__midi_in_handle != nullptr) {
		try { midiInStop(__midi_in_handle); }
		catch (...) {}
		try { midiInReset(__midi_in_handle); }
		catch (...) {}
		try { midiInClose(__midi_in_handle); }
		catch (...) {}
		__midi_in_handle = nullptr;
	}
	isEnable = false;
}
std::vector<std::string> MidiControllerIn::GetDeviceList() {

	DeviceList.clear();
	__num_devices = midiInGetNumDevs();
	for (size_t dev_id = 0; dev_id < __num_devices; ++dev_id) {

		MIDIINCAPS midi_in_caps{};
		MMRESULT res = midiInGetDevCaps(
			dev_id,
			&midi_in_caps,
			sizeof(midi_in_caps)
		);
		if (res != MMSYSERR_NOERROR) {
			LogCallback(
				LogString() << LogTag << "["
				<< Utils::ErrorMessage(res) << " | " << res << "] [" << dev_id << "]");
			continue;
		}

		char device_name_buff[32]{};
		errno_t error = wcstombs_s(
			NULL, device_name_buff, 32,
			midi_in_caps.szPname, sizeof(midi_in_caps.szPname));
		if (error != 0) continue;

		DeviceList.push_back(device_name_buff);
	}
	return DeviceList;
}
void MidiControllerIn::Stop() {
	Dispose();
}
bool MidiControllerIn::Start() {

	if (!config_ptr || config_ptr.get()->name.empty()) return false;

	Dispose();
	GetDeviceList();

	__device_id = UINT_MAX;
	__num_devices = midiInGetNumDevs();
	for (uint32_t i = 0; i < __num_devices; ++i) {
		if (DeviceName().compare(DeviceList.at(i)) == 0) {
			__device_id = i;
			break;
		}
	}

	isEnable = (__device_id < UINT_MAX);
	if (!isEnable) {
		LogCallback(LogString() << LogTag << "[" << DeviceName().c_str() << "] " << LogString::LogNotConnect << "!");
		return isEnable;
	}
	LogCallback(LogString() << LogString::LogFound << LogTag << "[" << DeviceList.at(__device_id).c_str() << "]");

	MMRESULT res = midiInOpen(&__midi_in_handle, static_cast<UINT>(__device_id), (DWORD_PTR)MidiInProc, 0, CALLBACK_FUNCTION);
	isEnable = (res == MMSYSERR_NOERROR);
	if (!isEnable) {
		LogCallback(
			LogString() << LogString::LogNotOpen << LogTag << "["
			<< Utils::ErrorMessage(res) << " | " << res << "] [" << __device_id << "]");
		return isEnable;
	}
	midiInStart(__midi_in_handle);
	LogCallback(LogString() << LogString::LogOpen << LogTag << "index: [" << __device_id << "]");
	return isEnable;
}

void MidiControllerIn::MonitorCallbackEvent(uint8_t scene, uint8_t id, uint8_t v, uint32_t t) {
	if (monitor != nullptr) monitor(scene, id, v, t);
}

void MidiControllerIn::RunCallbackEvent(uint8_t scene, uint8_t id, uint8_t v, uint32_t t) {
	if (isMonitor) MonitorCallbackEvent(scene, id, v, t);
	if (!config_ptr) return;

#if __DEBUG
	std::cout << "*MIDI-IN-PARSE: " << (int)scene << ", " << (int)id << ", " << (int)v << ", " << t << std::endl;
#endif

	for (size_t i = 0; i < config_ptr.get()->Count(); i++) {
		MidiUnit* unit = &config_ptr.get()->units[i];
		if (unit->id == id) {
			if ((unit->scene != 255) && (unit->scene != scene))
				break;
			if ((unit->type == UNITNONE) || (unit->target == Mackie::Target::NOTARGET))
				break;

			if (unit->type == BTNTOGGLE) {
				if (t - unit->value.time < config_ptr.get()->btninterval) {
					unit->value.time = t;
					break;
				}
				unit->value.lvalue = (v > 0);
				unit->value.type = Mackie::ClickType::ClickTrigger;
			}
			else if (unit->type == BTN) {
				if (v > 0) {
					unit->value.time = t;
					break;
				} else {
					uint32_t ctime = (t - unit->value.time);
					if (ctime < config_ptr.get()->btninterval) {
						unit->value.time = t;
						break;
					} else if (ctime >= config_ptr.get()->btnlonginterval) {
						if (unit->longtarget == Mackie::Target::NOTARGET)
							unit->value.type = Mackie::ClickType::ClickOnce;
						else
							unit->value.type = Mackie::ClickType::ClickLong;
#						if __DEBUG
						std::cout << "*LONGPERESS-IN: " << (int)ctime << ", " << (int)unit->value.time << ", " << (int)t << ", " << (int)unit->value.type << std::endl;
#						endif
					} else {
						unit->value.type = Mackie::ClickType::ClickOnce;
					}
					unit->value.lvalue = true;
				}
			}
			else if ((unit->type == SLIDER) || (unit->type == FADER)) {
				if (unit->value.value == v) break;
				switch (unit->target)
				{
				case Mackie::Target::AP1:
				case Mackie::Target::AP2:
				case Mackie::Target::AP3:
				case Mackie::Target::AP4:
				case Mackie::Target::AP5:
				case Mackie::Target::AP6:
				case Mackie::Target::AP7:
				case Mackie::Target::AP8:
				case Mackie::Target::XP9: unit->value.lvalue = (unit->value.value > v); break;
				default: break;
				}
			}
			else if (unit->type == KNOB) {
#				if __DEBUG
				std::cout << "*KNOB-IN " << (int)unit->value.value << ", " << (int)v << ", " << t << std::endl;
#				endif
				if (unit->value.value == v) break;
				unit->value.lvalue = (v > unit->value.value);
			}
			unit->value.value = v;
			unit->value.time = t;
			event(unit);
			break;
		}
	}
}

bool MidiControllerIn::IsEnable() {
	return isEnable;
}
bool MidiControllerIn::IsConnect() {
	return isConnect;
}
bool MidiControllerIn::IsMonitor() {
	return isMonitor;
}
void MidiControllerIn::SetMonitor(bool b) {
	isMonitor = b;
}

std::string MidiControllerIn::DeviceName() {
	return config_ptr.get()->name;
}
void MidiControllerIn::SetCallbackEvent(f_Fn_event f) {
	event = f;
}
void MidiControllerIn::SetCallbackProxy(f_Fn_proxy f) {
	proxy = f;
}
void MidiControllerIn::SetCallbackMonitor(f_Fn_monitor f) {
	monitor = f;
}


// CALLBACK //

void CALLBACK MidiControllerIn::MidiInProc(HMIDIIN midi_in_handle, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
#if __DEBUG
	std::cout << "*MIDI-IN-CB: " << wMsg << ", " << dwParam1 << ", " << dwParam2 << std::endl;
#endif

	if (ctrl == nullptr) return;

	switch (wMsg)
	{
	case MIM_OPEN: {
		MidiControllerIn::ctrl->LogCallback(LogString() << ctrl->LogTag << LogString::LogTagOpen);
		MidiControllerIn::ctrl->isConnect = true;
		break;
	}
	case MIM_CLOSE: {
		MidiControllerIn::ctrl->LogCallback(LogString() << ctrl->LogTag << LogString::LogTagClose);
		break;
	}
	case MIM_DATA: {
		uint8_t scene = (dwParam1 & 0x000000ff);
		switch (scene) {
		case SC1KNOB: // Main Rotary (scene 1)
		case SC2KNOB: // Main Rotary (scene 2)
		case SC3KNOB: // Main Rotary (scene 3)
		case SC4KNOB: // Main Rotary (scene 4)
		{
			switch (scene) {
			case SC1KNOB: { scene = SC1; break; }
			case SC2KNOB: { scene = SC2; break; }
			case SC3KNOB: { scene = SC3; break; }
			case SC4KNOB: { scene = SC4; break; }
			default: break;
			}
			MidiControllerIn::ctrl->RunCallbackEvent(
				scene,
				static_cast<uint8_t>(10),
				static_cast<uint8_t>((dwParam1 & 0x0000ff00) >> 8),
				static_cast<uint32_t>(dwParam2));
			break;
		}
		default: {
			if (scene == SC4BTN) {
				scene = SC4; // Main Rotary button left / right (scene 4)
			}
#pragma warning( push )
#pragma warning( disable : 4244 )
			MidiControllerIn::ctrl->RunCallbackEvent(
				scene, 
				static_cast<uint8_t>((dwParam1 & 0x0000ff00) >> 8),
				static_cast<uint8_t>((dwParam1 & 0x00ff0000) >> 16),
				static_cast<uint32_t>(dwParam2));
#pragma warning( pop )
			break;
		}
		}

		if (MidiControllerIn::ctrl->proxy != nullptr)
			MidiControllerIn::ctrl->proxy(dwParam1);
		break;
	}
	case MIM_ERROR:
	case MIM_MOREDATA:
	case MIM_LONGDATA:
	case MIM_LONGERROR:
	default:
		break;
	}
}

MidiControllerIn* MidiControllerIn::ctrl = nullptr;
