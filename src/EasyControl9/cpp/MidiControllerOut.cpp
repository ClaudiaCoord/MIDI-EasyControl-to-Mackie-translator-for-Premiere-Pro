/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#include "..\pch.h"

void MidiControllerOut::Initializer() {
	__num_devices = midiOutGetNumDevs();
	if (__num_devices == 0)
		throw new std::runtime_error(LogString() << LogTag << LogString::LogNotConnect);
	isManualPort = config_ptr.get()->manualport;
}
MidiControllerOut::MidiControllerOut(StatusDelegate& fstatus, std::shared_ptr<MidiDevice> cnf) : __status(fstatus) {
	MidiControllerOut::ctrl = this;
	config_ptr = cnf;
	Initializer();
}
MidiControllerOut::~MidiControllerOut() {
	try {
		Dispose();
		MidiControllerOut::ctrl = nullptr;
	}
	catch (...) {}
}
void MidiControllerOut::Dispose(bool b) {
	if (__midi_out_handle != nullptr) {
		try { midiOutReset(__midi_out_handle); }
		catch (...) {}
		try { midiOutClose(__midi_out_handle); }
		catch (...) {}
		__midi_out_handle = nullptr;
	}
	if (b && (vmdevice_ptr != nullptr) && (vmdevice_ptr)) {
		vmdevice_ptr.get()->Stop();
		vmdevice_ptr.reset();
		isConnect = false;
	}
	isEnable = false;
}
bool MidiControllerOut::IsEnable() {
	return isEnable;
}
bool MidiControllerOut::IsConnect() {
	return isConnect;
}
bool MidiControllerOut::IsManualPort() {
	return isManualPort;
}
void MidiControllerOut::SetManualPort(bool b) {
	isManualPort = b;
}
std::string MidiControllerOut::DeviceName() {
	return deviceName;
}
void MidiControllerOut::LogCallback(std::string s) {
	if (!s.empty()) __status(s);
}

void MidiControllerOut::Stop() {
	Dispose(false);
}
bool MidiControllerOut::Start() {

	if (!config_ptr || config_ptr.get()->name.empty()) return false;
	if (deviceName.empty()) {
		deviceName = Utils::BuildOutDeviceName(config_ptr.get()->name, "-Mackie-Out");
	}

	isManualPort = config_ptr.get()->manualport;

	if (!isManualPort) {

		if (vmdevice_ptr == nullptr)
			vmdevice_ptr = std::make_unique<VirtualMidi>(__status, deviceName);
		else if (!vmdevice_ptr)
			vmdevice_ptr.reset(new VirtualMidi(__status, deviceName));
		else {
			isEnable = isConnect = vmdevice_ptr.get()->IsEnable();
			return isEnable;
		}

		if (!vmdevice_ptr.get()->Start()) {
			LogCallback(LogString() << LogTag << "[" << deviceName.c_str() << "] " << LogString::LogNotConnect << "!");
			return false;
		}
		isEnable = isConnect = vmdevice_ptr.get()->IsEnable();
		return isEnable;
	}

	Dispose(false);

	__device_id = UINT_MAX;
	__num_devices = midiOutGetNumDevs();
	for (uint32_t dev_id = 0; dev_id < __num_devices; ++dev_id) {

		MIDIOUTCAPS midi_out_caps{};
		MMRESULT res = midiOutGetDevCaps(
			dev_id,
			&midi_out_caps,
			sizeof(midi_out_caps)
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
			midi_out_caps.szPname, sizeof(midi_out_caps.szPname));
		if (error != 0) continue;

		LogCallback(LogString() << LogString::LogFound << LogTag << "[" << device_name_buff << "]");

		if (deviceName.compare(device_name_buff) == 0) {
			__device_id = dev_id;
			break;
		}
	}

	isEnable = (__device_id < UINT_MAX);
	if (!isEnable) {
		LogCallback(LogString() << LogTag << "[" << deviceName.c_str() << "] " << LogString::LogNotConnect << "!");
		return isEnable;
	}

	MMRESULT res = midiOutOpen(&__midi_out_handle, __device_id, (DWORD_PTR)MidiOutProc, 0, CALLBACK_FUNCTION);
	isEnable = (res == MMSYSERR_NOERROR);
	if (!isEnable) {
		LogCallback(
			LogString() << LogString::LogNotOpen << LogTag << "["
			<< Utils::ErrorMessage(res) << " | " << res << "] [" << __device_id << "]");
		return isEnable;
	}
	LogCallback(LogString() << LogString::LogOpen << LogTag << "index: [" << __device_id << "]");
	return isEnable;
}

bool MidiControllerOut::SendToPort(MidiUnit* unit) {
#if __DEBUG
	std::cout << "*SendToPort 1: " << unit->Print() << std::endl << std::endl;
#endif

	Mackie::MIDIDATA m{};
	if (!Mackie::SelectorTarget(unit, m))
		return false;

#if __DEBUG
	std::cout << "*SendToPort 2: " << m.Print() << std::endl << std::endl;
#endif

	if (isManualPort)
		return SendToPort(m.send);
	return vmdevice_ptr.get()->SendToPort(m);
}
bool MidiControllerOut::SendToPort(DWORD d) {
	if (!isManualPort) return false;
	MMRESULT res = midiOutShortMsg(__midi_out_handle, d);
	if (res != MMSYSERR_NOERROR) {
		LogCallback(LogString() << LogTag << LogString::LogBadValues << ": [" << Utils::ErrorMessage(res) << " | " << res << "] [" << d << "]");
		return false;
	}
	return true;
}

// CALLBACK //

void CALLBACK MidiControllerOut::MidiOutProc(HMIDIOUT __midi_out_handle, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
#if __DEBUG
	std::cout << "*MIDI-OUT-CB: " << wMsg << ", " << dwParam1 << ", " << dwParam2 << std::endl;
#endif

	if (ctrl == nullptr) return;

	switch (wMsg)
	{
	case MOM_OPEN: {
		MidiControllerOut::ctrl->LogCallback(LogString() << ctrl->LogTag << LogString::LogTagOpen);
		MidiControllerOut::ctrl->isConnect = true;
		break;
	}
	case MOM_CLOSE: {
		MidiControllerOut::ctrl->LogCallback(LogString() << ctrl->LogTag << LogString::LogTagClose);
		MidiControllerOut::ctrl->isConnect = false;
		break;
	}
	case MOM_DONE: {
		MidiControllerOut::ctrl->LogCallback(LogString() << ctrl->LogTag << LogString::LogTagDone);
		break;
	}
	default:
		break;
	}
}

MidiControllerOut* MidiControllerOut::ctrl = nullptr;
