/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "framework.h"
#include "MIDIMT.h"

DialogMonitor::DialogMonitor() {
	DialogMonitor::ctrl = this;
}
DialogMonitor::~DialogMonitor() {
	DialogMonitor::ctrl = nullptr;
}

void DialogMonitor::Dispose() {
	try {
		__hwndDlg = nullptr;
		TMidiSetInMonitor(false);
		TMidiSetCallbackInMonitor(nullptr);
	}
	catch (...) {}
}

void DialogMonitor::InitDialog(HWND hwndDlg) {
	__hwndDlg = hwndDlg;
	try {
		if (!IsTMidiStarted()) {
			EnableWindow(GetDlgItem(__hwndDlg, IDC_MONITOR_STOP), false);
			EnableWindow(GetDlgItem(__hwndDlg, IDC_MONITOR_START), false);
			return;
		}
		EnableWindow(GetDlgItem(__hwndDlg, IDC_MONITOR_STOP), false);
		EnableWindow(GetDlgItem(__hwndDlg, IDC_MONITOR_START), true);
	}
	catch (const std::exception& ex) {
		DialogMonitor::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
}
void DialogMonitor::Start() {
	HWND hwnd = __hwndDlg;
	if (hwnd == nullptr) return;

	try {
		if (!IsTMidiStarted()) {
			EnableWindow(GetDlgItem(hwnd, IDC_MONITOR_STOP), false);
			EnableWindow(GetDlgItem(hwnd, IDC_MONITOR_START), false);
			return;
		}
		EnableWindow(GetDlgItem(hwnd, IDC_MONITOR_STOP), true);
		EnableWindow(GetDlgItem(hwnd, IDC_MONITOR_START), false);
		TMidiSetCallbackInMonitor(static_cast<f_Fn_monitor>(DialogMonitor::MonitorCb));
		TMidiSetInMonitor(true);
	}
	catch (const std::exception& ex) {
		DialogMonitor::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
}
void DialogMonitor::Stop() {
	try {
		if (IsTMidiInMonitor()) {
			TMidiSetInMonitor(false);
			TMidiSetCallbackInMonitor(nullptr);
		}
		HWND hwnd = __hwndDlg;
		if (hwnd != nullptr) {
			EnableWindow(GetDlgItem(hwnd, IDC_MONITOR_STOP), false);
			EnableWindow(GetDlgItem(hwnd, IDC_MONITOR_START), true);
		}
	}
	catch (const std::exception& ex) {
		DialogMonitor::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
}
void DialogMonitor::Clear() {
	try {
		HWND hwnd = __hwndDlg;
		if (hwnd == nullptr) return;
		SetDlgItemText(hwnd, IDC_MONITOR, L"");
		__log_string.str(L"");
	}
	catch (const std::exception& ex) {
		DialogMonitor::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
}
void DialogMonitor::EndDialog() {
	Dispose();
}

void DialogMonitor::MonitorCb(uint8_t scene, uint8_t id, uint8_t v, uint32_t t) {
	if (DialogMonitor::ctrl == nullptr) return;
	HWND hwnd = DialogMonitor::ctrl->__hwndDlg;
	if (hwnd != nullptr) {
		DialogMonitor::ctrl->__log_string << L"Scene: " << (uint32_t)scene << L", Key id: " << (uint32_t)id << L", Value: " << (uint32_t)v << L", Time offset: " << t << L"\r\n";
		SetDlgItemText(hwnd, IDC_MONITOR, DialogMonitor::ctrl->__log_string.str().c_str());
	}
}
void DialogMonitor::ErrorCb(const std::string s) {
	if (DialogMonitor::ctrl == nullptr) return;
	HWND hwnd = DialogMonitor::ctrl->__hwndDlg;
	if (hwnd != nullptr) {
		DialogMonitor::ctrl->__log_string << std::wstring(s.begin(), s.end()) << L"\r\n";
		SetDlgItemText(hwnd, IDC_MONITOR, DialogMonitor::ctrl->__log_string.str().c_str());
	}
}
DialogMonitor * DialogMonitor::ctrl = nullptr;
