/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "framework.h"
#include "MIDIMT.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")


#define MAX_LOADSTRING 100
UINT const WMAPP_SHELLICON = WM_APP + 101;
const wchar_t wikiUrl[] = L"https://github.com/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/wiki";
const wchar_t gitUrl[] = L"https://github.com/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/releases";
const wchar_t webUrl[] = L"https://claudiacoord.github.io/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/";

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

std::unique_ptr<DialogStart> dlgs;
std::unique_ptr<DialogMonitor> dlgm;
std::unique_ptr<TrayNotify> tray;
std::unique_ptr<TrayMenu> menu;

ATOM                RegisterMainClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    AboutDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    StartDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    MonitorDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	INITCOMMONCONTROLSEX ctrl{};
	ctrl.dwICC = ICC_USEREX_CLASSES | ICC_STANDARD_CLASSES | ICC_LINK_CLASS | ICC_HOTKEY_CLASS | ICC_BAR_CLASSES | ICC_ANIMATE_CLASS;
	ctrl.dwSize = sizeof(ctrl);
	InitCommonControlsEx(&ctrl);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MIDIMT, szWindowClass, MAX_LOADSTRING);

	do {
		HWND hWnd = FindWindow(szWindowClass, szTitle);
		if (hWnd == NULL)
			break;
		PostMessage(hWnd, (UINT)WM_COMMAND, (WPARAM)IDM_GO_ABOUT, (LPARAM)0);
		return 0;
	} while (0);

	RegisterMainClass(hInstance);
	tray = std::make_unique<TrayNotify>();

	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	dlgs = std::make_unique<DialogStart>(hInstance);
	menu = std::make_unique<TrayMenu>(hInstance);
	dlgm = std::make_unique<DialogMonitor>();

	MSG msg;
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MIDIMT));

	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	try {
		dlgs->Stop();
		menu->EndDialog();
		CloseTMidiController();
	} catch (...) {}

	return (int)msg.wParam;
}

ATOM RegisterMainClass(HINSTANCE hInstance)
{
	hInst = hInstance;
	WNDCLASSEXW wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MIDIMT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MIDIMT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) return FALSE;

	tray->Init(hInst, hWnd, WMAPP_SHELLICON, szTitle);

	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);
	tray->Install();
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDM_GO_START: {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_FORMSTART), hwnd, StartDialogProc);
			return true;
		}
		case IDM_GO_MONITOR: {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_FORMMONITOR), hwnd, MonitorDialogProc);
			return true;
		}
		case IDM_GO_ABOUT: {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_FORMABOUT), hwnd, AboutDialogProc);
			return true;
		}
		case IDM_GO_STOP: {
			dlgs->Stop();
			return true;
		}
		case IDM_GO_INFO: {
			tray->Show(hInst);
			return true;
		}
		case IDM_EXIT: {
			DestroyWindow(hwnd);
			return true;
		}
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		break;
	}
	case WMAPP_SHELLICON: {

		switch (LOWORD(lParam)) {
		case WM_LBUTTONUP:
		case WM_CONTEXTMENU: {
			POINT const p = { LOWORD(wParam), HIWORD(wParam) };
			menu->Show(hInst, hwnd, p);
			return true;
		}
		default: break;
		}
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DESTROY: {
		tray->UnInstall();
		PostQuitMessage(0);
		return true;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return false;
}

INT_PTR CALLBACK AboutDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG: {
		return (INT_PTR)TRUE;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDOK3: {
			ShellExecute(0, 0, wikiUrl, 0, 0, SW_SHOW);
			break;
		}
		case IDOK2: {
			ShellExecute(0, 0, gitUrl, 0, 0, SW_SHOW);
			break;
		}
		case IDOK:
		case IDCANCEL: {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
			break;
		}
		}
		break;
	}
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK StartDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_INITDIALOG: {
		dlgs->InitDialog(hwndDlg);
		return true;
	}
	case WM_HSCROLL: {
		if (lParam == 0) break;
		switch (LOWORD(wParam)) {
		case SB_LINELEFT:
		case SB_LINERIGHT:
		case SB_THUMBPOSITION: {
			dlgs->ChangeOnSliders();
			break;
		}
		}
		break;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_WRITELOG_CHECK: {
			dlgs->ChangeOnLog();
			return true;
		}
		case IDC_AUTOBOOT_CHECK: {
			dlgs->ChangeOnAutoRun();
			return true;
		}
		case IDC_CONFIG_SAVE: {
			dlgs->ConfigSave();
			return true;
		}
		case IDC_GO_ABOUT: {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_FORMABOUT), hwndDlg, AboutDialogProc);
			return true;
		}
		case IDC_GO_URL: {
			ShellExecute(0, 0, gitUrl, 0, 0, SW_SHOW);
			return true;
		}
		case IDC_GO_START: {
			dlgs->Start();
			return true;
		}
		case IDC_CHECK3: {
			dlgs->ChangeOnAutoStart();
			break;
		}
		case IDC_CHECK9: {
			dlgs->ChangeOnManualPort();
			break;
		}
		case IDC_CHECK12: {
			dlgs->ChangeOnProxy();
			break;
		}
		case IDCANCEL: {
			dlgs->EndDialog();
			EndDialog(hwndDlg, wParam);
			return true;
		}
		}
	}
	return false;
}

INT_PTR CALLBACK MonitorDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_INITDIALOG: {
		dlgm->InitDialog(hwndDlg);
		return true;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_MONITOR_START: {
			dlgm->Start();
			return true;
		}
		case IDC_MONITOR_STOP: {
			dlgm->Stop();
			return true;
		}
		case IDC_MONITOR_CLEAR: {
			dlgm->Clear();
			return true;
		}
		case IDCANCEL: {
			dlgm->EndDialog();
			EndDialog(hwndDlg, wParam);
			return true;
		}
		}
	}
	return false;
}
