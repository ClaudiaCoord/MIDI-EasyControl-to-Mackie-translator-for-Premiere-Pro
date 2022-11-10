/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#include "..\pch.h"

void MidiControllerProxy::LogCallback(std::string s) {
	if (!s.empty()) __status(s);
}
MidiControllerProxy::MidiControllerProxy(StatusDelegate& fstatus, std::shared_ptr<MidiDevice> cnf) : __status(fstatus) {
	MidiControllerProxy::ctrl = this;
	config_ptr = cnf;
	if ((config_ptr == nullptr) || (!config_ptr) || (config_ptr.get()->name.empty()))
		throw new std::runtime_error(LogString() << LogTag << JsonConfig::LogNameEmpty);
}
MidiControllerProxy::~MidiControllerProxy() {
	try {
		Dispose();
		MidiControllerProxy::ctrl = nullptr;
	}
	catch (...) {}
}
void MidiControllerProxy::Dispose() {
	if ((vmdevice_ptr != nullptr) && (vmdevice_ptr)) {
		vmdevice_ptr.get()->Stop();
		vmdevice_ptr.reset();
	}
	isEnable = false;
}
void MidiControllerProxy::Stop() {
	Dispose();
}
bool MidiControllerProxy::Start() {

	if (!config_ptr || config_ptr.get()->name.empty()) return false;
	std::string deviceName = Utils::BuildOutDeviceName(config_ptr.get()->name, "-Proxy-Out");

	if (vmdevice_ptr == nullptr)
		vmdevice_ptr = std::make_unique<VirtualMidi>(__status, deviceName);
	else if (!vmdevice_ptr)
		vmdevice_ptr.reset(new VirtualMidi(__status, deviceName));
	else {
		isEnable = vmdevice_ptr.get()->IsEnable();
		return isEnable;
	}

	if (!vmdevice_ptr.get()->Start()) {
		LogCallback(LogString() << LogTag << "[" << deviceName.c_str() << "] " << LogString::LogNotConnect << "!");
		return false;
	}
	isEnable = vmdevice_ptr.get()->IsEnable();
	return isEnable;
}


bool MidiControllerProxy::IsEnable() {
	return isEnable;
}
std::string MidiControllerProxy::DeviceName() {
	if ((vmdevice_ptr != nullptr) && (vmdevice_ptr))
		return vmdevice_ptr.get()->DeviceName();
	return "";
}
void MidiControllerProxy::SendToPort(DWORD d) {
	if (MidiControllerProxy::ctrl == nullptr) return;
	if ((ctrl->vmdevice_ptr != nullptr) && (ctrl->vmdevice_ptr)) {
		Mackie::MIDIDATA m{}; m.send = d;
		(void) ctrl->vmdevice_ptr.get()->SendToPort(m);
	}
}

MidiControllerProxy* MidiControllerProxy::ctrl = nullptr;
