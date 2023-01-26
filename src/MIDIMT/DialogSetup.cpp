/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2023, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "framework.h"
#include "MIDIMT.h"
#include "JsonConfig.h"

static const wchar_t* blanks[] = { L"", L"  ", L"    " };
static const wchar_t* columns[] = { L"id", L"scene", L"type", L"target", L"long-target" };
static const uint16_t iconsId[] = { IDB_ICON_SETUP3, IDB_ICON_SETUP2, IDB_ICON_SETUP1 };
static const uint16_t msgsId[] = { IDS_SETUP_MSG2, IDS_SETUP_MSG1, IDS_SETUP_MSG3 };
static const uint16_t ids[] = { IDC_SETUP_NUMBER, IDC_SETUP_SCENE, IDC_SETUP_TYPE, IDC_SETUP_TARGET, IDC_SETUP_TARGET_LONG };
static const uint16_t idn[] = { IDC_SETUP_NUMBER, IDC_SETUP_SCENEN, IDC_SETUP_TYPEN, IDC_SETUP_TARGETN, IDC_SETUP_TARGET_LONGN };

DialogSetup::DialogSetup(HINSTANCE hinst) {
	DialogSetup::ctrl = this;
	__hinst = hinst;
	try {
		if (icons.size() > 0) {
			for (ICONDATA* data : icons) {
				if (data == nullptr) continue;
				data->Reset();
				delete data;
			}
			icons.clear();
		}
		for (size_t i = 0; i < std::size(iconsId); i++) {
			HBITMAP hi = (HBITMAP)LoadImage(__hinst, MAKEINTRESOURCE(iconsId[i]), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT | LR_DEFAULTSIZE);
			if (hi != nullptr)
				icons.push_back(new ICONDATA(hi));
		}
	}
	catch (...) {}
}
DialogSetup::~DialogSetup() {
	Dispose();
}
void DialogSetup::Clear() {
	try {
		ConfigDevice = ConfigStatus::None;
		__hwndDlg = nullptr;
		if (__dev != nullptr) {
			__dev.reset();
			__dev = nullptr;
		}
		if (__lv != nullptr) {
			__lv.reset();
			__lv = nullptr;
		}
	}
	catch (...) {}
}
void DialogSetup::Dispose() {
	DialogSetup::ctrl = nullptr;
	Clear();
	try {
		for (ICONDATA* data : icons) {
			if (data == nullptr) continue;
			data->Reset();
			delete data;
		}
		icons.clear();
	} catch (...) {}
}
void DialogSetup::InitListView() {
	try {
		if (__lv != nullptr)
			__lv.reset();
		__lv = std::make_unique<ListEdit>(__hinst);
		__lv.get()->ListViewErrorCb(
			[](std::wstring ws) { DialogSetup::ErrorCb(ws); }
		);
	}
	catch (...) {}
}
void DialogSetup::EndDialog() {
	try {
		if (IsTMidiInMonitor()) {
			TMidiSetInMonitor(false);
			TMidiSetCallbackInMonitor(nullptr);
		}
	}
	catch (const std::exception& ex) {
		DialogSetup::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
	Clear();
}
void DialogSetup::InitDialog(HWND hwndDlg) {
	__hwndDlg = hwndDlg;

	try {
		ConfigDevice = IsTMidiConfig() ? ConfigStatus::LoadDevice : ConfigStatus::None;
		if (ConfigDevice == ConfigStatus::LoadDevice)
			__dev = TMidiGetConfig();
		else {
			__dev = std::make_shared<MidiDevice>();

			try {
				JsonConfig cnf;
				cnf.Read(*__dev.get(), CONFIG_NAME);
				ConfigDevice = ConfigStatus::LoadFile;
			} catch (const std::exception& ex) {
				DialogSetup::ErrorCb(std::string(ex.what()));
			} catch (...) {}
		}

		HWND hwcl;
		if ((hwcl = GetDlgItem(__hwndDlg, IDC_SETUP_LIST)) == nullptr)
			return;

		InitListView();
		__lv.get()->ListViewInit(hwcl);

		if (ConfigDevice != ConfigStatus::None) {
			__lv.get()->ListViewLoad(__dev);
			SetDlgItemText(__hwndDlg, IDC_SETUP_COUNT, std::to_wstring(__lv.get()->ListViewCount()).c_str());
		}

		if (IsTMidiStarted())
			EnableWindow(GetDlgItem(__hwndDlg, IDC_SETUP_CODE), true);

		try {
			int idx = 0;
			switch (ConfigDevice) {
				case ConfigStatus::None: idx = 0; break;
				case ConfigStatus::LoadDevice: idx = 1; break;
				case ConfigStatus::LoadFile: idx = 2; break;
				default: break;
			}
			ICONDATA* data = icons.at(idx);
			if ((data != nullptr) && (data->Get() != nullptr))
				SendDlgItemMessage(__hwndDlg, IDC_SETUP_ICON, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)data->Get());

			wchar_t wstatus[MAX_PATH]{};
			LoadStringW(__hinst, msgsId[idx], wstatus, _countof(wstatus));
			DialogSetup::ErrorCb(wstatus);
		}
		catch (const std::exception& ex) {
			DialogSetup::ErrorCb(std::string(ex.what()));
		}
		catch (...) {}

		if ((hwcl = GetDlgItem(__hwndDlg, IDC_SETUP_HELPLIST)) == nullptr)
			return;

		for (uint32_t i = 0; i <= static_cast<uint32_t>(Mackie::Target::SYS_Scrub); i++) {
			uint16_t idx = (i > 9) ? ((i > 99) ? 0 : 1) : 2;
			std::wstring ws = std::to_wstring(i) + blanks[idx] + L" - " + MackieHelper::GetTarget(i).data();
			int pos = (int)::SendMessage(hwcl, LB_ADDSTRING, 0, (LPARAM)ws.c_str());
			::SendMessage(hwcl, LB_SETITEMDATA, pos, (LPARAM)i);
		}
	}
	catch (const std::exception& ex) {
		DialogSetup::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
}

bool DialogSetup::ListViewSort(LPNMHDR lpmh) {
	if (lpmh == nullptr) return false;
	return __lv.get()->ListViewSort(lpmh);
}
bool DialogSetup::ListViewMenu(LPNMHDR lpmh) {
	if (lpmh == nullptr) return false;
	return __lv.get()->ListViewMenu(lpmh);
}
bool DialogSetup::ListViewMenu(uint32_t id) {

	try {
		bool b = __lv.get()->ListViewMenu(id);
		if (b) {
			switch (id) {
				case IDM_LV_NEW:
				case IDM_LV_PASTE:
				case IDM_LV_DELETE: {
					EnableWindow(GetDlgItem(__hwndDlg, IDC_SETUP_SAVE), true);
					SetDlgItemText(__hwndDlg, IDC_SETUP_COUNT, std::to_wstring(__lv.get()->ListViewCount()).c_str());
					break;
				}
				default: break;
			}
		}
		return b;
	}
	catch (const std::exception& ex) {
		DialogSetup::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
	return false;
}
bool DialogSetup::ListViewEdit(LPNMHDR lpmh) {
	if (lpmh == nullptr) return false;
	bool b = __lv.get()->ListViewSetItem(lpmh);
	if (b)
		EnableWindow(GetDlgItem(__hwndDlg, IDC_SETUP_SAVE), true);
	return b;
}
bool DialogSetup::ListViewClick(LPNMHDR lpmh) {
	if (lpmh == nullptr) return false;

	try {
		std::array<uint32_t, 5> arr = __lv.get()->ListViewGetSelectedRow(lpmh);
		if ((arr[0] >= 255) || (arr[1] >= 255U) || (arr[2] > 255U) || (arr[3] > 255U))
			return false;

		for (size_t i = 0; i < std::size(arr); i++) {
			std::wstring ws{};
			switch (i) {
				case 1: { ws = MackieHelper::GetScene(arr[i]); break; }
				case 2: { ws = MackieHelper::GetType(arr[i]); break; }
				case 3:
				case 4: { ws = MackieHelper::GetTarget(arr[i]); break; }
				default: break;
			}
			if (i != 0)
				SetDlgItemText(__hwndDlg, ids[i], ws.c_str());
			SetDlgItemText(__hwndDlg, idn[i], std::to_wstring(arr[i]).c_str());
		}
		return true;
	}
	catch (const std::exception& ex) {
		DialogSetup::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
	return false;
}
bool DialogSetup::ListViewDbClick(LPNMHDR lpmh) {
	if (lpmh == nullptr) return false;
	return __lv.get()->ListViewEdit(lpmh);
}

bool DialogSetup::ButtonSave() {
	try {
		bool b = __lv.get()->ListViewGetList(__dev);
		if (!b) return false;

		SetDlgItemText(__hwndDlg, IDC_SETUP_COUNT, std::to_wstring(__lv.get()->ListViewCount()).c_str());

#		if !defined(_DEBUG)
		if (ConfigDevice == ConfigStatus::LoadDevice)
			TMidiSave();
		else {
			JsonConfig cnf;
			cnf.Write(__dev.get(), CONFIG_NAME);
		}
#		endif
		EnableWindow(GetDlgItem(__hwndDlg, IDC_SETUP_SAVE), false);
	}
	catch (const std::exception& ex) {
		DialogSetup::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
	return true;
}
bool DialogSetup::ButtonMonitor() {
	if (!IsTMidiStarted()) return false;
	try {
		TMidiSetCallbackInMonitor(static_cast<f_Fn_monitor>(DialogSetup::MonitorCb));
		TMidiSetInMonitor(true);
		EnableWindow(GetDlgItem(__hwndDlg, IDC_SETUP_CODE), false);
	}
	catch (const std::exception& ex) {
		DialogSetup::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
	return true;
}

void DialogSetup::ErrorCb(const std::string &s) {
	if (DialogSetup::ctrl == nullptr) return;
	HWND hwnd = DialogSetup::ctrl->__hwndDlg;
	if (hwnd != nullptr)
		SetDlgItemText(hwnd, IDC_SETUP_MSG, std::wstring(s.begin(), s.end()).c_str());
}
void DialogSetup::ErrorCb(const std::wstring &ws) {
	if (DialogSetup::ctrl == nullptr) return;
	HWND hwnd = DialogSetup::ctrl->__hwndDlg;
	if (hwnd != nullptr)
		SetDlgItemText(hwnd, IDC_SETUP_MSG, ws.c_str());
}
void DialogSetup::MonitorCb(uint8_t scene, uint8_t id, uint8_t v, uint32_t t) {
	try {
		if (IsTMidiInMonitor()) {
			TMidiSetInMonitor(false);
			TMidiSetCallbackInMonitor(nullptr);
		}
	}
	catch (const std::exception& ex) {
		DialogSetup::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}

	std::wstringstream ss{};
	ss << L"New Key -> Id: " << (uint32_t)id << L", Scene: " << (uint32_t)scene << L", Value: " << (uint32_t)v << L", Time offset: " << t << L"\r\n";
	DialogSetup::ErrorCb(ss.str());
#	if defined(_DEBUG)
	OutputDebugStringW(ss.str().c_str());
#	endif

	if (DialogSetup::ctrl == nullptr) return;
	HWND hwnd = DialogSetup::ctrl->__hwndDlg;
	if (hwnd != nullptr) {
		try {
			std::array<uint32_t, 5> arr{ id, scene, (v == 127U) ? 3 : ((v > 0) ? 1U : 255U), 255U, 255U };
			bool b = DialogSetup::ctrl->__lv.get()->ListViewInsertItem(arr);
			if (b)
				EnableWindow(GetDlgItem(hwnd, IDC_SETUP_SAVE), true);
			if (IsTMidiStarted())
				EnableWindow(GetDlgItem(hwnd, IDC_SETUP_CODE), true);
		}
		catch (const std::exception& ex) {
			DialogSetup::ErrorCb(std::string(ex.what()));
		}
		catch (...) {}
	}
}

DialogSetup* DialogSetup::ctrl = nullptr;
