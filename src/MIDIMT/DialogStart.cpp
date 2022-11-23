/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "framework.h"
#include "MIDIMT.h"
#include "JsonConfig.h"

#define CHECKBTN(A) (A ? BST_CHECKED : BST_UNCHECKED)
#define CHECKRADIO(A,B,C) (A ? B : C)

static const wchar_t regRoot[] = L"SOFTWARE\\CC\\MIDIMT";
static const wchar_t regRun[] = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
static const wchar_t regLogger[] = L"Logger";
static const wchar_t regAutorun[] = L"Autorun";
static const wchar_t regConfigName[] = L"ConfigName";
static const wchar_t regRunId[] = L"MIDIMT";

const wchar_t * DialogStart::outMackie = L"-Mackie-Out";
const wchar_t * DialogStart::outProxy = L"-Proxy-Out";

DialogStart::DialogStart(HINSTANCE hinst) {
	DialogStart::ctrl = this;
	LoadStringW(hinst, IDS_DLG_MSG1, wmsgs[0], MAX_PATH);
	LoadStringW(hinst, IDS_DLG_MSG2, wmsgs[1], MAX_PATH);

}
DialogStart::~DialogStart() {
	Dispose();
	DialogStart::ctrl = nullptr;
}

void DialogStart::Dispose() {
	try {
		TMidiStop();
		__hwndDlg = nullptr;
		if (__log.is_open())
			__log.close();
	} catch (...) {}
}

void DialogStart::EndDialog() {
	__hwndDlg = nullptr;
}
void DialogStart::InitDialog(HWND hwndDlg) {
	DialogStart::ctrl = this;
	__hwndDlg = hwndDlg;
	if (__hwndDlg == nullptr) return;

	try {
		bool isStarted = false,
			 isConfig = false,
			 isLog = IsOnLog();

		uint32_t proxyCount = 0U;
		std::shared_ptr<MidiDevice> devc = nullptr;

		if (isLog && !__log.is_open())
			__log = std::ofstream("MIDIMT.log");

		isConfig = IsTMidiConfig();
		if (isConfig) {
			try {
				devc = TMidiGetConfig();
				proxyCount = devc.get()->proxy;
			}
			catch (const std::exception& ex) {
				DialogStart::InfoCb(ex.what());
				isConfig = false;
			}
			catch (...) { isConfig = false; }
		}

		CheckDlgButton(__hwndDlg, IDC_WRITELOG_CHECK, CHECKBTN(isLog));
		CheckDlgButton(__hwndDlg, IDC_AUTOBOOT_CHECK, CHECKBTN(IsOnAutoRun()));
		
		SetDlgItemText(__hwndDlg, IDC_MIDI_MACKIE_OUT, wmsgs[0]);
		SetDlgItemText(__hwndDlg, IDC_MIDI_PROXY_OUT, wmsgs[1]);

		if (!OpenTMidiController(static_cast<f_Fn_status>(DialogStart::InfoCb))) {
			CheckRadioButton(__hwndDlg, IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);
			EnableWindow(GetDlgItem(__hwndDlg, IDC_GO_START), false);
			EnableWindow(GetDlgItem(__hwndDlg, IDC_CONFIG_SAVE), false);
			return;
		}

		if (isConfig) {
			const std::string s = TMidiInDeviceName();
			if (!s.empty()) {
				std::wstringstream wss;
				wss << std::wstring(s.begin(), s.end()) << DialogStart::outMackie;
				SetDlgItemText(__hwndDlg, IDC_MIDI_MACKIE_OUT, wss.str().c_str());
				wss.str(L"");
				wss << std::wstring(s.begin(), s.end()) << DialogStart::outProxy;
				if (proxyCount > 0) {
					wss << L"-(";
					for (uint32_t i = 0, z = proxyCount - 1; i < proxyCount; i++) {
						wss << static_cast<uint32_t>(i + 1U);
						if (i < z)
							wss << L",";
					}
					wss << L")";
				}
				SetDlgItemText(__hwndDlg, IDC_MIDI_PROXY_OUT, wss.str().c_str());
				BuildDeviceComboBox(s);
			}
		}

		isStarted = IsTMidiStarted();
		if (isStarted) {
			EnableWindow(GetDlgItem(__hwndDlg, IDC_GO_START), false);
		}

		CheckRadioButton(__hwndDlg, IDC_RADIO1, IDC_RADIO2, CHECKRADIO(isStarted, IDC_RADIO1, IDC_RADIO2));
		CheckDlgButton(__hwndDlg, IDC_CHECK1, CHECKBTN(IsTMidiEnable()));
		CheckDlgButton(__hwndDlg, IDC_CHECK2, CHECKBTN(isConfig));
		CheckDlgButton(__hwndDlg, IDC_CHECK3, CHECKBTN(IsTMidiAutoStart()));

		CheckDlgButton(__hwndDlg, IDC_CHECK4, CHECKBTN(IsTMidiInEnable()));
		CheckDlgButton(__hwndDlg, IDC_CHECK5, CHECKBTN(IsTMidiInConnect()));
		CheckDlgButton(__hwndDlg, IDC_CHECK6, CHECKBTN(IsTMidiInMonitor()));

		CheckDlgButton(__hwndDlg, IDC_CHECK7, CHECKBTN(IsTMidiOutEnable()));
		CheckDlgButton(__hwndDlg, IDC_CHECK8, CHECKBTN(IsTMidiOutConnect()));
		CheckDlgButton(__hwndDlg, IDC_CHECK9, CHECKBTN(IsTMidiOutManualPort()));

		CheckDlgButton(__hwndDlg, IDC_CHECK12, CHECKBTN(IsTMidiProxy()));
		BuildProxyComboBox(proxyCount);

		{
			uint32_t ival = 50,
					 lval = 500;
			if (isConfig) {
				try {
					ival = devc.get()->btninterval;
					lval = devc.get()->btnlonginterval;
				}
				catch (const std::exception& ex) {
					DialogStart::InfoCb(ex.what());
				} catch (...) {}
			}
			SetSliderValues(__hwndDlg, IDC_SLIDER_INT, IDC_SLIDER_VAL1, 10, 1000, ival);
			SetSliderValues(__hwndDlg, IDC_SLIDER_LONGINT, IDC_SLIDER_VAL2, 100, 1500, lval);
		}
	}
	catch (const std::runtime_error& ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (const std::exception& ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (...) {}
}
void DialogStart::AutoStart() {
	try {
		if (!IsOnAutoRun()) return;
		if (!IsTMidiEnable()) {
			if (!OpenTMidiController(static_cast<f_Fn_status>(DialogStart::InfoCb)))
				return;
			if (!IsTMidiConfig() || !IsTMidiAutoStart())
				return;
		}
		if (!IsTMidiStarted())
			TMidiStart();
	}
	catch (const std::exception& ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (...) {}
}
void DialogStart::Start() {
	if (!IsTMidiStarted()) {
		if (TMidiStart()) {
			if (__hwndDlg == nullptr) return;
			try {
				HWND hwcb;
				if ((hwcb = GetDlgItem(__hwndDlg, IDC_PROXY_COMBO)) != nullptr)
					ComboBox_Enable(hwcb, false);
				if ((hwcb = GetDlgItem(__hwndDlg, IDC_CHECK6)) != nullptr)
					Button_Enable(hwcb, false);
				if ((hwcb = GetDlgItem(__hwndDlg, IDC_CHECK9)) != nullptr)
					Button_Enable(hwcb, false);
			}
			catch (const std::runtime_error& ex) {
				DialogStart::InfoCb(std::string(ex.what()));
			}
			catch (const std::exception& ex) {
				DialogStart::InfoCb(std::string(ex.what()));
			}
		}
	}
}
void DialogStart::Stop() {
	Dispose();
}
bool DialogStart::IsStart() {
	return IsTMidiStarted();
}
bool DialogStart::IsAutoStart() {
	return IsTMidiAutoStart();
}
bool DialogStart::IsProxy() {
	return IsTMidiProxy();
}
void DialogStart::ConfigSave() {
	HWND hwnd = __hwndDlg;
	if (hwnd == nullptr) return;
	try {
		std::shared_ptr<MidiDevice> dev;
		bool isconfig = IsTMidiConfig();
		if (!isconfig)
			dev = std::make_shared<MidiDevice>();
		else
			dev = TMidiGetConfig();

		if (dev.get()->config.empty()) {
			dev.get()->config = CONFIG_NAME;
		}
		dev.get()->autostart = (IsDlgButtonChecked(__hwndDlg, IDC_CHECK3) == BST_CHECKED);
		dev.get()->manualport = (IsDlgButtonChecked(__hwndDlg, IDC_CHECK9) == BST_CHECKED);

		HWND hwcb = GetDlgItem(__hwndDlg, IDC_DEVICE_COMBO);
		if (hwcb != nullptr) {
			WCHAR devname[MAX_PATH];
			ComboBox_GetText(hwcb, devname, MAX_PATH);
			const std::wstring w(devname);
			dev.get()->name = std::string(w.begin(), w.end());
		}

		hwcb = GetDlgItem(__hwndDlg, IDC_PROXY_COMBO);
		if (hwcb != nullptr) {
			if (IsDlgButtonChecked(__hwndDlg, IDC_CHECK12) != BST_CHECKED)
				dev.get()->proxy = 0U;
			else {
				WCHAR devname[MAX_PATH];
				ComboBox_GetText(hwcb, devname, MAX_PATH);
				dev.get()->proxy = std::stoul(devname);
			}
		}

		DWORD pos = GetSliderValue(hwnd, IDC_SLIDER_INT);
		if (pos <= 1000)
			dev.get()->btninterval = pos;
		pos = GetSliderValue(hwnd, IDC_SLIDER_LONGINT);
		if (pos <= 1500)
			dev.get()->btnlonginterval = pos;

		if (isconfig)
			TMidiSave();
		else {
			JsonConfig cnf;
			cnf.Write(dev.get(), CONFIG_NAME);
			dev.reset();
		}
	}
	catch (const std::exception& ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (...) {}
}

///

void DialogStart::SetRegistryBool(bool b, const wchar_t* label) {
	try {
		auto access = RegistryAPI::DesiredAccess::Write;
		auto key = RegistryAPI::CurrentUser->Create(regRoot, access);
		key->SetBoolean(label, b);
	}
	catch (const std::exception & ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (...) {}
}
bool DialogStart::GetRegistryBool(const wchar_t* label) {
	try {
		auto key = RegistryAPI::CurrentUser->Open(regRoot);
		return key->GetBoolean(label);
	}
	catch (const std::exception & ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (...) {}
	return false;
}
void DialogStart::SetRegistryRun() {
	try {
		WCHAR path[MAX_PATH];
		if (GetModuleFileName(NULL, path, MAX_PATH) != ERROR_SUCCESS)
			return;

		auto access = RegistryAPI::DesiredAccess::Write;
		auto key = RegistryAPI::CurrentUser->Open(regRun, access);
		key->SetString(regRunId, path);
	}
	catch (const std::exception& ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (...) {}
}
void DialogStart::DeleteRegistryRun() {
	try {
		auto access = RegistryAPI::DesiredAccess::Write;
		auto key = RegistryAPI::CurrentUser->Open(regRun, access);
		key->Delete(regRunId);
	}
	catch (const std::exception& ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (...) {}
}

///

void DialogStart::ChangeOnLog() {
	HWND hwnd = __hwndDlg;
	if (hwnd == nullptr) return;
	SetRegistryBool((IsDlgButtonChecked(__hwndDlg, IDC_WRITELOG_CHECK) == BST_CHECKED), regLogger);
}
bool DialogStart::IsOnLog() {
	return GetRegistryBool(regLogger);
}

void DialogStart::ChangeOnAutoRun() {
	HWND hwnd = __hwndDlg;
	if (hwnd == nullptr) return;
	bool b = (IsDlgButtonChecked(__hwndDlg, IDC_AUTOBOOT_CHECK) == BST_CHECKED);
	SetRegistryBool(b, regAutorun);
	if (b) SetRegistryRun();
	else DeleteRegistryRun();
}
bool DialogStart::IsOnAutoRun() {
	return GetRegistryBool(regAutorun);
}

void DialogStart::ChangeOnSliders() {
	try {
		HWND hwnd = __hwndDlg;
		if (!IsTMidiConfig() || (hwnd == nullptr)) return;

		DWORD pos1, pos2;
		std::shared_ptr<MidiDevice> dev = TMidiGetConfig();
		pos1 = GetSliderValue(hwnd, IDC_SLIDER_INT);
		if (pos1 <= 1000) {
			dev.get()->btninterval = pos1;
			SetSliderInfo(hwnd, IDC_SLIDER_VAL1, pos1);
		} else {
			pos1 = dev.get()->btninterval;
			SetSliderValues(hwnd, IDC_SLIDER_INT, IDC_SLIDER_VAL1, 10, 1000, pos1);
		}

		pos2 = GetSliderValue(hwnd, IDC_SLIDER_LONGINT);
		if (pos2 <= 1500) {
			bool b = (pos2 > pos1);
			pos2 = b ? pos2 : (pos1 + 150);
			dev.get()->btnlonginterval = pos2;
			if (!b)
				SetSliderValues(hwnd, IDC_SLIDER_LONGINT, IDC_SLIDER_VAL2, 100, 1500, pos2);
			else
				SetSliderInfo(hwnd, IDC_SLIDER_VAL2, pos2);
		}
	}
	catch (const std::exception & ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (...) {}
}
void DialogStart::ChangeOnProxy() {
	try {
		HWND hwnd = __hwndDlg;
		if (!IsTMidiConfig() || (hwnd == nullptr)) return;
		std::shared_ptr<MidiDevice> dev = TMidiGetConfig();

		bool b = false;
		HWND hwcb = GetDlgItem(__hwndDlg, IDC_PROXY_COMBO);
		if (hwcb != nullptr) {
			uint32_t n = static_cast<uint32_t>(::SendMessage(hwcb, CB_GETCURSEL, 0, 0));
			b = n > 0U;
			TMidiSetProxyCount(n);
		} else {
			TMidiSetProxyCount(0U);
		}
		CheckDlgButton(__hwndDlg, IDC_CHECK12, CHECKBTN(b));
	}
	catch (const std::exception& ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (...) {}
}
void DialogStart::ChangeOnManualPort() {
	try {
		HWND hwnd = __hwndDlg;
		if (!IsTMidiConfig() || (hwnd == nullptr)) return;

		std::shared_ptr<MidiDevice> dev = TMidiGetConfig();
		dev.get()->manualport = (IsDlgButtonChecked(__hwndDlg, IDC_CHECK9) == BST_CHECKED);
		TMidiSetOutManualPort(dev.get()->manualport);
	}
	catch (const std::exception& ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (...) {}
}
void DialogStart::ChangeOnAutoStart() {
	try {
		HWND hwnd = __hwndDlg;
		if (!IsTMidiConfig() || (hwnd == nullptr)) return;

		std::shared_ptr<MidiDevice> dev = TMidiGetConfig();
		dev.get()->autostart = (IsDlgButtonChecked(__hwndDlg, IDC_CHECK3) == BST_CHECKED);
		TMidiSetAutoStart(dev.get()->autostart);
	}
	catch (const std::exception& ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (...) {}
}

void DialogStart::BuildDeviceComboBox(const std::string s) {
	try {
		HWND hwcb = GetDlgItem(__hwndDlg, IDC_DEVICE_COMBO);
		if (hwcb == nullptr) return;

		std::vector<std::string> v = TMidiDeviceList();
		if (v.empty()) {
			ComboBox_AddString(hwcb, std::wstring(s.begin(), s.end()).c_str());
		} else {
			for (auto& name : v)
				ComboBox_AddString(hwcb, std::wstring(name.begin(), name.end()).c_str());
		}
		ComboBox_SelectString(hwcb, 0, std::wstring(s.begin(), s.end()).c_str());
	}
	catch (const std::runtime_error & ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (const std::exception & ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
}
void DialogStart::BuildProxyComboBox(const uint32_t n) {
	try {
		HWND hwcb = GetDlgItem(__hwndDlg, IDC_PROXY_COMBO);
		if (hwcb == nullptr) return;

		for (uint32_t i = 0, z = (n > 5) ? (n + 1) : 5; i < z; i++)
			ComboBox_AddString(hwcb, std::to_wstring(i).c_str());
		ComboBox_SelectString(hwcb, 0, std::to_wstring(n).c_str());
	}
	catch (const std::runtime_error& ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (const std::exception& ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
}

void DialogStart::SetSliderInfo(HWND hwnd, uint32_t id, uint32_t pos) {
	if (hwnd == nullptr) return;
	SetDlgItemText(hwnd, id, std::wstring(std::to_wstring(pos)).c_str());
}
void DialogStart::SetSliderValues(HWND hwnd, uint32_t id, uint32_t idinfo, uint32_t minv, uint32_t maxv, uint32_t pos) {
	if (hwnd == nullptr) return;

	try {
		HWND hwtr = GetDlgItem(hwnd, id);
		if (hwtr == nullptr) return;
		(void) SendMessage(hwtr, TBM_SETRANGE, static_cast<WPARAM>(FALSE), static_cast<LPARAM>(MAKELONG(minv, maxv)));
		(void) SendMessage(hwtr, TBM_SETPOS, static_cast<WPARAM>(TRUE), static_cast<LPARAM>(pos));
		SetSliderInfo(hwnd, idinfo, pos);
	}
	catch (const std::exception& ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (...) {}
}
uint32_t DialogStart::GetSliderValue(HWND hwnd, uint32_t id) {
	if (hwnd == nullptr) return UINT_MAX;

	try {
		HWND hwcb = GetDlgItem(hwnd, id);
		if (hwcb != nullptr)
			return static_cast<uint32_t>(SendMessage(hwcb, TBM_GETPOS, 0, 0));
	}
	catch (const std::exception& ex) {
		DialogStart::InfoCb(std::string(ex.what()));
	}
	catch (...) {}
	return UINT_MAX;
}

void DialogStart::InfoCb(const std::string s) {
	if (DialogStart::ctrl == nullptr) return;
	if (DialogStart::ctrl->__log.is_open()) {
		DialogStart::ctrl->__log << s << std::endl;
		DialogStart::ctrl->__log.flush();
	}
	HWND hwnd = DialogStart::ctrl->__hwndDlg;
	if (hwnd != nullptr) {
		DialogStart::ctrl->__log_string << std::wstring(s.begin(), s.end()) << L"\r\n";
		SetDlgItemText(hwnd, IDC_LOG, DialogStart::ctrl->__log_string.str().c_str());
	}
}

DialogStart *DialogStart::ctrl = nullptr;
