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
	if (vmdevices_ptr.size() > 0) {
		for (auto& ptr : vmdevices_ptr) {
			if ((ptr != nullptr) && (ptr)) {
				ptr.get()->Stop();
				ptr.reset();
			}
		}
		vmdevices_ptr.clear();
	}
	isEnable = false;
}
void MidiControllerProxy::Stop() {
	Dispose();
}
bool MidiControllerProxy::Start() {

	if (!config_ptr || config_ptr.get()->name.empty() || config_ptr.get()->proxy == 0U) return false;
	Dispose();

	uint32_t count = config_ptr.get()->proxy;
	for (uint32_t i = 0U; i < count; i++) {
		try {
			std::string devn = Utils::BuildOutDeviceName(config_ptr.get()->name, "-Proxy-Out-" + std::to_string(i + 1));
			std::shared_ptr<VirtualMidi> vmidi = std::make_unique<VirtualMidi>(__status, devn);
			if (!vmidi.get()->Start()) {
				LogCallback(LogString() << LogTag << "[" << devn.c_str() << "] " << LogString::LogNotConnect << "!");
				continue;
			}
			isEnable = vmidi.get()->IsEnable();
			if (isEnable)
				vmdevices_ptr.push_back(vmidi);
		}
		catch (const std::exception& ex) {
			LogCallback(LogString() << LogTag << ex.what());
		}
		catch (...) {}
	}
	isEnable = (vmdevices_ptr.size() > 0U);
	return isEnable;
}

void MidiControllerProxy::SetProxyCount(uint32_t i) {
	config_ptr.get()->proxy = i;
}
uint32_t MidiControllerProxy::GetProxyCount() {
	if (vmdevices_ptr.size() > 0)
		return static_cast<uint32_t>(vmdevices_ptr.size());
	return config_ptr.get()->proxy;
}

bool MidiControllerProxy::IsEnable() {
	return isEnable;
}
std::string MidiControllerProxy::DeviceName(uint32_t i) {
	do {
		if (vmdevices_ptr.size() == 0)
			break;
		auto vmidi = vmdevices_ptr.at(i);
		if (vmidi)
			return vmidi.get()->DeviceName();
	} while (0);
	return "";
}
void MidiControllerProxy::SendToPort(DWORD d) {
	if (MidiControllerProxy::ctrl == nullptr) return;
	if (ctrl->vmdevices_ptr.size() > 0) {
		Mackie::MIDIDATA m{}; m.send = d;

		for (auto& ptr : ctrl->vmdevices_ptr) {
			if (ptr) {
				(void)ptr.get()->SendToPort(m);
			}
		}
	}
}

MidiControllerProxy* MidiControllerProxy::ctrl = nullptr;
