/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"
static const wchar_t guidString[] = L"{cd3e2202-b792-4fd9-b9b1-313224a46d76}";

TrayNotify::TrayNotify() {}
TrayNotify::~TrayNotify() {}

void TrayNotify::Init(HINSTANCE hinst, HWND hwnd, uint32_t id, const wchar_t *title) {
	data.cbSize = sizeof(NOTIFYICONDATA);
	data.hWnd = (HWND)hwnd;
	data.uID = static_cast<UINT>(IDI_SMALL);
	data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	data.uVersion = NOTIFYICON_VERSION_4;
	data.hIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_SMALL));
	data.uCallbackMessage = id;
	if (CLSIDFromString(guidString, (LPCLSID)&guid) == S_OK)
		data.guidItem = guid;
	wcscpy_s(data.szTip, title);
}
void TrayNotify::Install() {
	Shell_NotifyIcon(NIM_ADD, &data);
	Shell_NotifyIcon(NIM_SETVERSION, &data);
}
void TrayNotify::UnInstall() {
	data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_DELETE, &data);
}
void TrayNotify::Show(HINSTANCE hinst) {
	try {
		data.uFlags |= NIF_INFO;
		data.uTimeout = 1000;
		data.dwInfoFlags = NULL;

		bool b = IsTMidiStarted();
		if (b) {
			LoadStringW(hinst, IDS_INFO2, data.szInfoTitle, _countof(data.szInfoTitle));
			if (IsTMidiConfig()) {
				const std::string s = TMidiInDeviceName();
				if (!s.empty()) {

					wchar_t infoString[3][MAX_PATH]{};
					LoadStringW(hinst, IDS_INFO3, infoString[0], _countof(infoString[0]));
					LoadStringW(hinst, IDS_INFO4, infoString[1], _countof(infoString[1]));
					LoadStringW(hinst, IDS_INFO5, infoString[2], _countof(infoString[2]));

					std::wstring ws = std::wstring(s.begin(), s.end());
					std::wstringstream wss;
					wss << infoString[0] << L" " << ws << L"\r\n";
					wss << infoString[1] << L" " << ws << DialogStart::outMackie << L"\r\n";
					wss << infoString[2] << L" " << ws << DialogStart::outProxy << L"\r\n";
					ws = wss.str();
					wcscpy_s(data.szInfo, _countof(data.szInfo), ws.c_str());
				}
			}
		}
		else {
			LoadStringW(hinst, IDS_INFO1, data.szInfoTitle, _countof(data.szInfoTitle));
			wcscpy_s(data.szInfo, L"-");
		}
		Shell_NotifyIcon(NIM_MODIFY, &data);
	}
	catch (...) {}
}
