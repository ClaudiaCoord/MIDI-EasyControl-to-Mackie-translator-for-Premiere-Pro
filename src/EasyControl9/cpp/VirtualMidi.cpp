/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#include "..\pch.h"

VirtualMidi::VirtualMidi(StatusDelegate & fstatus, std::string & devname) : __status(fstatus) {
	VirtualMidi::ctrl = this;
	deviceName = std::wstring(devname.begin(), devname.end());
}
VirtualMidi::~VirtualMidi() {
	try {
		Dispose();
		VirtualMidi::ctrl = nullptr;
	}
	catch (...) {}
}
void VirtualMidi::Dispose() {
	if (__midi_port != nullptr) {
		virtualMIDIClosePort(__midi_port);
		__midi_port = nullptr;
		LogCallback(LogString() << LogTag << LogString::LogTagClose);
	}
	isEnable = false;
}
bool VirtualMidi::Start() {

	isEnable = false;
	(void)GetLastError();

	if (deviceName.empty())
		throw new std::runtime_error(LogString() << LogTag << LogString::LogNotDefined);

	LPCWSTR v = virtualMIDIGetDriverVersion(NULL, NULL, NULL, NULL);
	if (v != nullptr) {
		char version_buff[32]{};
		errno_t error = wcstombs_s(
			NULL, version_buff, 32,
			v, wcslen(v));
		if (error == 0)
			LogCallback(LogString() << LogTag << "driver version [" << version_buff << "]");
	}

	__midi_port = virtualMIDICreatePortEx2(deviceName.c_str(), NULL, 0, TE_VM_DEFAULT_BUFFER_SIZE, TE_VM_FLAGS_PARSE_TX | TE_VM_FLAGS_INSTANTIATE_BOTH);
	DWORD err = GetLastError();

	std::string s{};
	switch (err)
	{
	case ERR_NOT_ERRORS: {
		LogCallback(LogString() << LogTag << "[" << std::string(deviceName.begin(), deviceName.end()).c_str() << "]: OK");
		isEnable = (__midi_port != nullptr);
		return isEnable;
	}
	case ERR_PATH_NOT_FOUND: {
		s = std::string(LogString::LogVMError1);
		break;
	}
	case ERR_INVALID_HANDLE: {
		s = std::string(LogString::LogVMError2);
		break;
	}
	case ERR_TOO_MANY_CMDS: {
		s = std::string(LogString::LogVMError3);
		break;
	}
	case ERR_TOO_MANY_SESS: {
		s = std::string(LogString::LogVMError4);
		break;
	}
	case ERR_INVALID_NAME: {
		s = std::string(LogString::LogVMError5);
		break;
	}
	case ERR_MOD_NOT_FOUND: {
		s = std::string(LogString::LogVMError6);
		break;
	}
	case ERR_BAD_ARGUMENTS: {
		s = std::string(LogString::LogVMError7);
		break;
	}
	case ERR_ALREADY_EXISTS: {
		s = std::string(LogString::LogVMError8);
		break;
	}
	case ERR_OLD_WIN_VERSION: {
		s = std::string(LogString::LogVMError9);
		break;
	}
	case ERR_REVISION_MISMATCH: {
		s = std::string(LogString::LogVMError10);
		break;
	}
	case ERR_ALIAS_EXISTS: {
		s = std::string(LogString::LogVMError11);
		break;
	}
	default:
		break;
	}
	LogCallback(LogString() << LogTag << "[" << std::string(deviceName.begin(), deviceName.end()).c_str() << "]: (" << err << ") " << s.c_str());
	return false;
}
void VirtualMidi::Stop() { Dispose(); }

std::string VirtualMidi::DeviceName() {
	return std::string(deviceName.begin(), deviceName.end());
}
bool VirtualMidi::SendToPort(Mackie::MIDIDATA& m) {
	if (__midi_port == nullptr) return false;
	if (!virtualMIDISendData(__midi_port, m.data, 3)) {
		LogCallback(LogString() << LogTag << "[" << std::string(deviceName.begin(), deviceName.end()).c_str() << ": " <<
			LogString::LogBadValues << " - " << GetLastError());
		return false;
	}
	return true;
}
bool VirtualMidi::IsEnable() {
	return isEnable;
}
void VirtualMidi::SetCallbackStatus(f_Fn_status f) {
	__status += f;
}
void VirtualMidi::LogCallback(std::string s) {
	if (!s.empty()) __status(s);
}

VirtualMidi* VirtualMidi::ctrl = nullptr;
