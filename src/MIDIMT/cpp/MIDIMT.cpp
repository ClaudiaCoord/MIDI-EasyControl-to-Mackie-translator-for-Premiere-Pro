/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "MIDIMTLIB.lib")
#pragma comment(lib, "MIDIMTMIX.lib")
#pragma comment(lib, "MEDIAKEYS.lib")
#pragma comment(lib, "EASYCTRL9.lib")

UINT const WMAPP_SHELLICON = WM_APP + 101;

std::unique_ptr<Common::MIDIMT::DialogStart> dlgs;
std::unique_ptr<Common::MIDIMT::DialogConfig> dlgc;
std::unique_ptr<Common::MIDIMT::DialogMonitor> dlgm;
std::unique_ptr<Common::MIDIMT::DialogAbout> dlga;
std::unique_ptr<Common::MIDIMT::TrayNotify> trayn;
std::unique_ptr<Common::MIDIMT::TrayMenu> menu;
std::unique_ptr<Common::MIDIMT::AudioMixerPanels> mctrl;

ATOM                RegisterMainClass();
BOOL                InitInstance(int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    AboutDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ConfigDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    StartDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    MonitorDialogProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	INITCOMMONCONTROLSEX ctrl{};
	ctrl.dwICC = ICC_USEREX_CLASSES | ICC_STANDARD_CLASSES | ICC_LINK_CLASS | ICC_HOTKEY_CLASS | ICC_BAR_CLASSES | ICC_ANIMATE_CLASS | ICC_LISTVIEW_CLASSES | ICC_HOTKEY_CLASS;
	ctrl.dwSize = sizeof(ctrl);
	InitCommonControlsEx(&ctrl);

	Common::MIDIMT::LangInterface& lang = Common::MIDIMT::LangInterface::Get();
	lang.SetMainHinstance(hInstance);

	do {
		HWND hwnd = FindWindow(lang.GetMainClass().c_str(), lang.GetMainTitle().c_str());
		if (hwnd == NULL) break;
		PostMessage(hwnd, (UINT)WM_COMMAND, (WPARAM)IDM_GO_ABOUT, (LPARAM)0);
		return 0;
	} while (0);

	RegisterMainClass();
	trayn = std::make_unique<Common::MIDIMT::TrayNotify>();

	if (!InitInstance(nCmdShow)) return 0;
	if (::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE) != S_OK) return 0;

	Common::to_log::Get().filelog();
	Common::worker_background::Get().start();

	menu = std::make_unique<Common::MIDIMT::TrayMenu>();
	dlgs = std::make_unique<Common::MIDIMT::DialogStart>();
	dlgc = std::make_unique<Common::MIDIMT::DialogConfig>();
	dlgm = std::make_unique<Common::MIDIMT::DialogMonitor>();
	dlga = std::make_unique<Common::MIDIMT::DialogAbout>();
	mctrl = std::make_unique<Common::MIDIMT::AudioMixerPanels>(trayn.get());

	MSG msg;
	HACCEL hAccelTable = lang.GetAccelerators(MAKEINTRESOURCEW(IDC_MIDIMT));

	dlgs->AutoStart();

	while (GetMessageW(&msg, nullptr, 0, 0)) {
		if (!TranslateAcceleratorW(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	try {
		dlgs->Stop();
		menu->EndDialog();

	} catch (...) {
		Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__);
	}
	try {
		::CoUninitialize();
	}
	catch (...) {
		Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__);
	}

	try {
		Common::to_log::Get().end();
		Common::worker_background::Get().stop();
	} catch (...) {}

	return (int)msg.wParam;
}

ATOM RegisterMainClass()
{
	auto& lang = Common::MIDIMT::LangInterface::Get();
	WNDCLASSEXW wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = lang.GetMainHinstance();
	wcex.hIcon = lang.GetIcon(MAKEINTRESOURCEW(IDI_MIDIMT));
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MIDIMT);
	wcex.lpszClassName = lang.GetMainClass().c_str();
	wcex.hIconSm = lang.GetIcon(MAKEINTRESOURCEW(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(int nCmdShow)
{
	Common::MIDIMT::LangInterface& lang = Common::MIDIMT::LangInterface::Get();
	HWND hwnd = CreateWindowW(
		lang.GetMainClass().c_str(),
		lang.GetMainTitle().c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, lang.GetMainHinstance(), nullptr);
	if (!hwnd) return FALSE;
	Common::MIDIMT::LangInterface::Get().SetMainHwnd(hwnd);

	trayn->Init(hwnd, WMAPP_SHELLICON, lang.GetMainTitle());

	ShowWindow(hwnd, SW_HIDE);
	UpdateWindow(hwnd);
	trayn->Install();
	return TRUE;
}

bool showMenu(HWND hwnd, WPARAM w) {
	POINT const p = { LOWORD(w), HIWORD(w) };
	menu->Show(hwnd, p);
	return true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT m, WPARAM w, LPARAM l)
{
	switch (m) {
		case WM_COMMAND: {
			switch (LOWORD(w)) {
				case IDM_GO_START: {
					if (dlgs->IsRunOnce() && dlgc->IsRunOnce() && dlgm->IsRunOnce())
						Common::MIDIMT::LangInterface::Get().GetDialog(hwnd, StartDialogProc, MAKEINTRESOURCEW(IDD_FORMSTART));
					else
						dlgs->SetFocus();
					return true;
				}
				case IDM_GO_MONITOR: {
					if (dlgc->IsRunOnce() && dlgm->IsRunOnce())
						Common::MIDIMT::LangInterface::Get().GetDialog(hwnd, MonitorDialogProc, MAKEINTRESOURCEW(IDD_FORMMONITOR));
					else
						dlgm->SetFocus();
					return true;
				}
				case IDM_GO_SETUP: {
					if (dlgs->IsRunOnce() && dlgc->IsRunOnce() && dlgm->IsRunOnce())
						Common::MIDIMT::LangInterface::Get().GetDialog(hwnd, ConfigDialogProc, MAKEINTRESOURCEW(IDD_FORMSETUP));
					else
						dlgc->SetFocus();
					return true;
				}
				case IDM_GO_ABOUT: {
					Common::MIDIMT::LangInterface::Get().GetDialog(hwnd, AboutDialogProc, MAKEINTRESOURCEW(IDD_FORMABOUT));
					return true;
				}
				case IDM_GO_STOP: {
					dlgs->Stop();
					return true;
				}
				case IDM_GO_INFO: {
					trayn->Show();
					return true;
				}
				case IDM_GO_MIXER: {
					mctrl->Show();
					return true;
				}
				case IDM_EXIT: {
					::DestroyWindow(hwnd);
					return true;
				}
				default: return DefWindowProc(hwnd, m, w, l);
			}
			break;
		}
		case WMAPP_SHELLICON: {
			switch (LOWORD(l)) {
				case WM_LBUTTONUP: {
					return showMenu(hwnd, w);
				}
				case WM_CONTEXTMENU: {
					Common::common_config& cnf = Common::common_config::Get();
					if (cnf.Registry.GetMixerRightClick() && cnf.Local.IsAudioMixerRun()) {
						mctrl->Show();
						return true;
					}
					return showMenu(hwnd, w);
				}
				default: break;
			}
			break;
		}
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = ::BeginPaint(hwnd, &ps);
			::EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY: {
			trayn->UnInstall();
			::PostQuitMessage(0);
			return true;
		}
		default: return DefWindowProc(hwnd, m, w, l);
	}
	return false;
}
INT_PTR CALLBACK AboutDialogProc(HWND hwnd, UINT m, WPARAM w, LPARAM l)
{
	UNREFERENCED_PARAMETER(l);
	switch (m) {
		case WM_INITDIALOG: {
			dlga->InitDialog(hwnd);
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			switch (LOWORD(w)) {
				case IDC_OK3: {
					ShellExecute(0, 0, Common::MIDIMT::LangInterface::Get().GetString(IDS_URL_WIKI).c_str(), 0, 0, SW_SHOW);
					break;
				}
				case IDC_OK2: {
					ShellExecute(0, 0, Common::MIDIMT::LangInterface::Get().GetString(IDS_URL_GIT).c_str(), 0, 0, SW_SHOW);
					break;
				}
				case IDOK:
				case IDCANCEL: {
					dlga->EndDialog();
					EndDialog(hwnd, LOWORD(w));
					return (INT_PTR)TRUE;
					break;
				}
			}
			break;
		}
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK StartDialogProc(HWND hwnd, UINT m, WPARAM w, LPARAM l) {
	switch (m) {
		case WM_INITDIALOG: {
			dlgs->InitDialog(hwnd);
			return true;
		}
		case WM_HSCROLL: {
			if (l == 0) break;
			switch (LOWORD(w)) {
				case SB_LINELEFT:
				case SB_LINERIGHT:
				case SB_THUMBPOSITION: {
					dlgs->ChangeOnSliders();
					break;
				}
			}
			break;
		}
		case WM_COMMAND: {
			switch (LOWORD(w)) {
				case IDC_GO_ABOUT: {
					Common::MIDIMT::LangInterface::Get().GetDialog(hwnd, AboutDialogProc, MAKEINTRESOURCEW(IDD_FORMABOUT));
					break;
				}
				case IDC_GO_URL: {
					::ShellExecuteW(0, 0, Common::MIDIMT::LangInterface::Get().GetString(IDS_URL_GIT).c_str(), 0, 0, SW_SHOW);
					break;
				}
				case IDC_GO_START: {
					dlgs->StartFromUi();
					break;
				}
				case IDC_CONFIG_SAVE: {
					dlgs->ConfigSave();
					break;
				}
				case IDC_OPEN_CONFIG: {
					dlgs->ChangeOnConfigFileOpen();
					break;
				}
				case IDC_DEVICE_COMBO: {
					if (HIWORD(w) == CBN_SELENDOK) /* CBN_SELCHANGE */
						dlgs->ChangeOnDevice();
					break;
				}
				case IDC_PROXY_COMBO: {
					if (HIWORD(w) == CBN_SELENDOK) /* CBN_SELCHANGE */
						dlgs->ChangeOnProxy();
					break;
				}
				case IDC_LANG_COMBO: {
					if (HIWORD(w) == CBN_SELENDOK) /* CBN_SELCHANGE */
						dlgs->ChangeOnLang();
					break;
				}
				case IDC_AUTOBOOT_SYS: {
					dlgs->ChangeOnSysAutoStart();
					break;
				}
				case IDC_AUTORUN_SYS: {
					dlgs->ChangeOnSysAutoRun();
					break;
				}
				case IDC_AUTORUN_CONFIG: {
					dlgs->ChangeOnAutoRunConfig();
					break;
				}
				case IDC_WRITE_FILELOG: {
					dlgs->ChangeOnLog();
					break;
				}
				case IDC_JOGFILTER_CONFIG: {
					dlgs->ChangeOnJogfilter();
					break;
				}
				case IDC_MANUALPORT_CONFIG: {
					dlgs->ChangeOnManualPort();
					break;
				}
				case IDC_MMKEY_ENABLE: {
					dlgs->ChangeOnMmkeyEnable();
					break;
				}
				case IDC_MIXER_ENABLE: {
					dlgs->ChangeOnMixerEnable();
					break;
				}
				case IDC_MIXER_RIGHT_CLICK: {
					dlgs->ChangeOnMixerRightClick();
					break;
				}
				case IDC_MIXER_FAST_VALUE: {
					dlgs->ChangeOnMixerfastvalue();
					break;
				}
				case IDC_MIXER_OLD_VALUE: {
					dlgs->ChangeOnMixeroldvalue();
					break;
				}
				case IDC_IEVENT_LOG: {
					dlgs->EventLog();
					break;
				}
				case IDC_IEVENT_MONITOR: {
					dlgs->EventMonitor();
					break;
				}
				case IDCANCEL: {
					dlgs->EndDialog();
					EndDialog(hwnd, w);
					return true;
				}
				default: return false;
			}
			return true;
		}
	}
	return false;
}
INT_PTR CALLBACK MonitorDialogProc(HWND hwnd, UINT m, WPARAM w, LPARAM l) {
	switch (m) {
		case WM_INITDIALOG: {
			dlgm->InitDialog(hwnd);
			return true;
		}
		case WM_COMMAND: {
			switch (LOWORD(w)) {
				case IDC_MONITOR_START: {
					dlgm->Start();
					break;
				}
				case IDC_MONITOR_STOP: {
					dlgm->Stop();
					break;
				}
				case IDC_MONITOR_CLEAR: {
					dlgm->Clear();
					break;
				}
				case IDC_IEVENT_LOG: {
					dlgm->EventLog();
					break;
				}
				case IDC_IEVENT_MONITOR: {
					dlgm->EventMonitor();
					break;
				}
				case IDCANCEL: {
					dlgm->EndDialog();
					EndDialog(hwnd, w);
					break;
				}
				default: return false;
			}
			return true;
		}
	}
	return false;
}
INT_PTR CALLBACK ConfigDialogProc(HWND hwnd, UINT m, WPARAM w, LPARAM l) {
	switch (m) {
		case WM_INITDIALOG: {
			dlgc->InitDialog(hwnd);
			return true;
		}
		case WM_HSCROLL: {
			if (l == 0) break;
			switch (LOWORD(w)) {
				case SB_LINELEFT:
				case SB_LINERIGHT:
				case SB_THUMBPOSITION: {
					dlgc->ChangeOnSlider();
					break;
				}
			}
			break;
		}
		case WM_NOTIFY: {
			LPNMHDR lpmh = (LPNMHDR)l;
			switch (lpmh->idFrom) {
				case IDC_SETUP_LIST: {
					switch (lpmh->code) {
						case (UINT)LVN_BEGINLABELEDIT: {
							break;
						}
						case (UINT)LVN_ENDLABELEDIT:   {
							dlgc->ListViewEdit(lpmh);
							break;
						}
						case (UINT)NM_DBLCLK: {
							dlgc->ListViewDbClick(lpmh);
							break;
						}
						case (UINT)NM_CLICK:  {
							dlgc->ListViewClick(lpmh);
							break;
						}
						case (UINT)NM_RCLICK: {
							dlgc->ListViewMenu(lpmh);
							break;
						}
						case (UINT)LVN_COLUMNCLICK: {
							dlgc->ListViewSort(lpmh);
							break;
						}
						default: return false;
					}
					return true;
				}
			}
			break;
		}
		case WM_COMMAND: {
			switch (LOWORD(w)) {
				case IDC_SETUP_SAVE: {
					dlgc->ButtonSave();
					break;
				}
				case IDC_SETUP_CODE: {
					dlgc->ButtonMonitor();
					break;
				}
				case IDC_SETUP_RADIO1: {
					dlgc->HelpCategorySelected(IDC_SETUP_RADIO1);
					break;
				}
				case IDC_SETUP_RADIO2: {
					dlgc->HelpCategorySelected(IDC_SETUP_RADIO2);
					break;
				}
				case IDC_SETUP_RADIO3: {
					dlgc->HelpCategorySelected(IDC_SETUP_RADIO3);
					break;
				}
				case IDC_SETUP_RADIO4: {
					dlgc->HelpCategorySelected(IDC_SETUP_RADIO4);
					break;
				}
				case IDM_LV_NEW:
				case IDM_LV_COPY:
				case IDM_LV_PASTE:
				case IDM_LV_DELETE:
				case IDM_LV_SET_MMKEY:
				case IDM_LV_SET_MIXER:
				case IDM_LV_READ_MIDI_CODE: {
					dlgc->ListViewMenu(LOWORD(w));
					break;
				}
				case IDC_SETUP_APPLIST: {
					if (HIWORD(w) == LBN_SELCHANGE || HIWORD(w) == LBN_DBLCLK)
						dlgc->ChangeOnAppBox();
					break;
				}
				case IDC_BTN_FOLDER: {
					dlgc->ChangeOnBtnAppFolder();
					break;
				}
				case IDC_BTN_RUNAPP: {
					dlgc->ChangeOnBtnAppRunning();
					break;
				}
				case IDC_BTN_REMOVE: {
					dlgc->ChangeOnBtnAppDelete();
					break;
				}
				case IDC_BTN_MUTE: {
					dlgc->ChangeOnBtnMute();
					break;
				}
				case IDC_IEVENT_LOG: {
					dlgc->EventLog();
					break;
				}
				case IDC_IEVENT_MONITOR: {
					dlgc->EventMonitor();
					break;
				}
				case IDCANCEL: {
					dlgc->EndDialog();
					::EndDialog(hwnd, w);
					break;
				}
				default: return false;
			}
			return true;
		}
	}
	return false;
}
