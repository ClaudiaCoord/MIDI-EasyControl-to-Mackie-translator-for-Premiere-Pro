/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#include "..\pch.h"

void MidiBridge::Initializer0(f_Fn_status fstatus, std::string& fp) {
	if (fp.empty())
		throw new std::runtime_error(LogString() << JsonConfig::LogTag << LogString::LogNotDefined);

	__filepath = CheckFilePath(fp);
	config_ptr = std::make_shared<MidiDevice>();

	if (fstatus != nullptr)
		__status += fstatus;
	if (Load(__filepath))
		Initializer1(isAutoStart);
}
void MidiBridge::Initializer1(bool isstart) {

	if ((ctrl_in_ptr == nullptr) || (!ctrl_in_ptr) || (ctrl_out_ptr == nullptr) || (!ctrl_out_ptr)) {
		Dispose(false);
		Initializer2();
	} else {
		if (ctrl_in_ptr.get()->IsEnable())
			ctrl_in_ptr.get()->Stop();
		if (ctrl_out_ptr.get()->IsEnable())
			ctrl_out_ptr.get()->Stop();
	}

	if (!isstart)
		return;

	LogCallback(LogString() << MidiBridge::LogTag << LogString::LogAutoStart);

	isProxy = config_ptr.get()->proxy;

	if ((ctrl_in_ptr != nullptr) && (ctrl_in_ptr))
		ctrl_in_ptr.get()->Start();
	if ((ctrl_out_ptr != nullptr) && (ctrl_out_ptr))
		ctrl_out_ptr.get()->Start();
	if (isProxy) {
		if ((ctrl_proxy_ptr != nullptr) && (ctrl_proxy_ptr)) {
			ctrl_proxy_ptr.get()->Start();
			ctrl_in_ptr.get()->SetCallbackProxy(MidiControllerProxy::SendToPort);
		}
	}
}

void MidiBridge::Initializer2() {
	if (ctrl_in_ptr == nullptr)
		ctrl_in_ptr = std::make_unique<MidiControllerIn>(&MidiBridgeProc, __status, config_ptr);
	else
		ctrl_in_ptr.reset(new MidiControllerIn(&MidiBridgeProc, __status, config_ptr));

	if (ctrl_out_ptr == nullptr)
		ctrl_out_ptr = std::make_unique<MidiControllerOut>(__status, config_ptr);
	else
		ctrl_out_ptr.reset(new MidiControllerOut(__status, config_ptr));

	if (ctrl_proxy_ptr == nullptr)
		ctrl_proxy_ptr = std::make_unique<MidiControllerProxy>(__status, config_ptr);
	else
		ctrl_proxy_ptr.reset(new MidiControllerProxy(__status, config_ptr));

	isEnable = true;
}
void MidiBridge::LogCallback(std::string s) {
	if (!s.empty()) __status(s);
}
std::string MidiBridge::CheckFilePath(std::string& s) {
	std::string fp = s;
	if (fp.empty())
		throw new std::runtime_error(LogString() << JsonConfig::LogTag << LogString::LogNotFound);

	if (!fp.ends_with(".cnf"))
		fp.append(".cnf");
	return fp;
}

MidiBridge::MidiBridge(f_Fn_status fstatus, std::string fp) {
	MidiBridge::ctrl = this;
	Initializer0(fstatus, fp);
}
MidiBridge::~MidiBridge() { Dispose(); }

void MidiBridge::Dispose(bool isfull) {
	if ((ctrl_in_ptr != nullptr) && (ctrl_in_ptr)) {
		ctrl_in_ptr.get()->Dispose();
		ctrl_in_ptr.reset(nullptr);
	}
	if ((ctrl_out_ptr != nullptr) && (ctrl_out_ptr)) {
		ctrl_out_ptr.get()->Dispose();
		ctrl_out_ptr.reset(nullptr);
	}
	if ((ctrl_proxy_ptr != nullptr) && (ctrl_proxy_ptr)) {
		ctrl_proxy_ptr.get()->Dispose();
		ctrl_proxy_ptr.reset(nullptr);
	}
	if (isfull && config_ptr) {
		config_ptr.reset();
		isConfig = false;
	}
	isEnable = false;
}

bool MidiBridge::Start() {
	return Start(__filepath);
}
bool MidiBridge::Start(std::string& fp) {
	if (isEnable) {
		LogCallback(LogString() << MidiBridge::LogTag << LogString::LogAlreadyRun << BOOLTOLOG(isEnable) << BOOLTOLOG(isConfig));
		return false;
	}

	bool b = false;

	if (!fp.empty()) {
		if (Load(fp)) {
			Initializer1(true);
			b = true;
		}
	} else if (isConfig) {
		Initializer1(true);
		b = true;
	}
	if (b)
		LogCallback(LogString() << MidiBridge::LogTag << LogString::LogStart);
	else
		LogCallback(LogString() << MidiBridge::LogTag << LogString::LogNotRun << BOOLTOLOG(isEnable) << BOOLTOLOG(isConfig));

	return b && isEnable && isConfig;
}
bool MidiBridge::Stop() {
	if (isEnable) {
		LogCallback(LogString() << MidiBridge::LogTag << LogString::LogStop);
		Dispose(false);
		return true;
	}
	LogCallback(LogString() << MidiBridge::LogTag << LogString::LogNotRun << BOOLTOLOG(isEnable) << BOOLTOLOG(isConfig));
	return false;
}
bool MidiBridge::Load() {
	return Load(__filepath);
}
bool MidiBridge::Load(std::string& fp) {

	JsonConfig cnf;
	std::string filepath = CheckFilePath(fp);
	if (filepath.empty())
		return false;

	config_ptr.get()->Clear();
	isConfig = cnf.Read(*config_ptr.get(), filepath);
	if (!isConfig) {
		LogCallback(LogString() << JsonConfig::LogTag << JsonConfig::LogLoadError);
		return isConfig;
	}
	isConfig = !config_ptr.get()->name.empty();
	if (!isConfig) {
		LogCallback(LogString() << JsonConfig::LogTag << JsonConfig::LogNameEmpty);
		return isConfig;
	}
	else
		LogCallback(LogString() << JsonConfig::LogTag << JsonConfig::LogLoad << ": (" << config_ptr.get()->name.c_str() << ")");

	if (config_ptr.get()->config.empty()) {
		config_ptr.get()->config = filepath;
	}
	else if (!config_ptr.get()->config.compare(filepath)) {
		filepath = CheckFilePath(config_ptr.get()->config);
		return Load(filepath);
	}
	isAutoStart = config_ptr.get()->autostart;

#if _DEBUG
	cnf.Print(config_ptr.get());
#endif
	return isConfig;
}
bool MidiBridge::Save() {
	return Save(__filepath);
}
bool MidiBridge::Save(std::string& fp) {

	if (!isConfig) return false;

	JsonConfig cnf;
	std::string filepath = CheckFilePath(fp);
	if (filepath.empty())
		return false;

	bool b = cnf.Write(config_ptr.get(), filepath);
	if (!b)
		LogCallback(LogString() << JsonConfig::LogTag << JsonConfig::LogSaveError);
	else
		LogCallback(LogString() << JsonConfig::LogTag << JsonConfig::LogSave);
	return b;
}

void MidiBridge::SetCallbackStatus(f_Fn_status f) {
	if (f != nullptr) __status += f;
}
void MidiBridge::RemoveCallbackStatus(f_Fn_status f) {
	if (f != nullptr) __status -= f;
}
void MidiBridge::SetButtonOnInterval(uint32_t msec) {
	config_ptr.get()->btninterval = msec;
}
void MidiBridge::SetButtonOnLongInterval(uint32_t msec) {
	config_ptr.get()->btnlonginterval = msec;
}
void MidiBridge::SetCallbackProxy(f_Fn_proxy f) {
	isProxy = f != nullptr;
	if ((ctrl_in_ptr != nullptr) && (ctrl_in_ptr))
		ctrl_in_ptr.get()->SetCallbackProxy(f);
}
void MidiBridge::SetProxy(bool b) {
	isProxy = b;
}
void MidiBridge::SetAutoStart(bool b) {
	isAutoStart = b;
}
void MidiBridge::SetOutManualPort(bool b) {
	if ((ctrl_out_ptr != nullptr) && (ctrl_out_ptr))
		ctrl_out_ptr.get()->SetManualPort(b);
}
void MidiBridge::SetInMonitor(bool b) {
	if ((ctrl_in_ptr != nullptr) && (ctrl_in_ptr))
		ctrl_in_ptr.get()->SetMonitor(b);
}
void MidiBridge::SetCallbackInMonitor(f_Fn_monitor f) {
	if ((ctrl_in_ptr != nullptr) && (ctrl_in_ptr))
		ctrl_in_ptr.get()->SetCallbackMonitor(f);
}

bool MidiBridge::IsAutoStart() {
	return isAutoStart;
}
bool MidiBridge::IsProxy() {
	return isProxy;
}
bool MidiBridge::IsEnable() {
	return isEnable;
}
bool MidiBridge::IsConfig() {
	return isConfig;
}
bool MidiBridge::IsStarted() {
	if ((ctrl_in_ptr != nullptr) && (ctrl_in_ptr) && (ctrl_out_ptr != nullptr) && (ctrl_out_ptr)) {
		do {
			if (!isEnable || !isConfig) break;
			if (!ctrl_in_ptr.get()->IsEnable()) break;
			if (!ctrl_in_ptr.get()->IsConnect()) break;
			if (!ctrl_out_ptr.get()->IsEnable()) break;
			if (!ctrl_out_ptr.get()->IsConnect()) break;
			return true;
		} while (0);
	}
	return false;
}
bool MidiBridge::IsInEnable() {
	if ((ctrl_in_ptr != nullptr) && (ctrl_in_ptr))
		return ctrl_in_ptr.get()->IsEnable();
	return false;
}
bool MidiBridge::IsOutEnable() {
	if ((ctrl_out_ptr != nullptr) && (ctrl_out_ptr))
		return ctrl_out_ptr.get()->IsEnable();
	return false;
}
bool MidiBridge::IsInConnect() {
	if ((ctrl_in_ptr != nullptr) && (ctrl_in_ptr))
		return ctrl_in_ptr.get()->IsConnect();
	return false;
}
bool MidiBridge::IsOutConnect() {
	if ((ctrl_out_ptr != nullptr) && (ctrl_out_ptr))
		return ctrl_out_ptr.get()->IsConnect();
	return false;
}
bool MidiBridge::IsInMonitor() {
	if ((ctrl_in_ptr != nullptr) && (ctrl_in_ptr))
		return ctrl_in_ptr.get()->IsMonitor();
	return false;
}
bool MidiBridge::IsOutManualPort() {
	if ((ctrl_out_ptr != nullptr) && (ctrl_out_ptr))
		return ctrl_out_ptr.get()->IsManualPort();
	return false;
}

std::string MidiBridge::InDeviceName() {
	return config_ptr.get()->name;
}
std::string MidiBridge::OutDeviceName() {
	if ((ctrl_out_ptr != nullptr) && (ctrl_out_ptr))
		return ctrl_out_ptr.get()->DeviceName();
	return "";
}
std::vector<std::string> MidiBridge::InDeviceList() {
	if ((ctrl_in_ptr != nullptr) && (ctrl_in_ptr))
		return ctrl_in_ptr.get()->DeviceList;
	return std::vector<std::string>();
}
std::shared_ptr<MidiDevice> MidiBridge::GetConfig() {
	return config_ptr;
}

bool MidiBridge::SendToPort(MidiUnit* unit) {
	if ((ctrl_out_ptr != nullptr) && (ctrl_out_ptr))
		return ctrl_out_ptr.get()->SendToPort(unit);
	return false;
}
bool MidiBridge::SendToPort(DWORD d) {
	if ((ctrl_out_ptr != nullptr) && (ctrl_out_ptr))
		return ctrl_out_ptr.get()->SendToPort(d);
	return false;
}

// CALLBACK //

void MidiBridge::MidiBridgeProc(MidiUnit* unit) {
	MidiUnit* u = unit;
#if __DEBUG
	std::cout << "*MIDIBRIDGE-IN-CB: " << (int)u->id << ", " << (int)u->type << ", " << (int)u->value.value << "/" << (int)u->value.lvalue << ", " << u->value.longpress << std::endl;
#endif
	if ((ctrl != nullptr) && (ctrl->ctrl_out_ptr)) {
		MidiControllerOut *mco = ctrl->ctrl_out_ptr.get();
		if (mco->IsConnect()) mco->SendToPort(u);
	}
}

MidiBridge* MidiBridge::ctrl = nullptr;

