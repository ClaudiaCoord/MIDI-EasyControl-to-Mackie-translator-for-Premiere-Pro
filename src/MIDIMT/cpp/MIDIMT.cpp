﻿/*
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
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "propsys.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "MIDIMTLIB.lib")
#pragma comment(lib, "MIDIMTMIX.lib")
#pragma comment(lib, "MEDIAKEYS.lib")
#pragma comment(lib, "EASYCTRL9.lib")
#pragma comment(lib, "SMARTHOME.lib")

UINT const WMAPP_SHELLICON = WM_APP + 101;
Gdiplus::GdiplusStartupInput gdiplusStartupInput{};
ULONG_PTR					 gdiplusToken{};

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
INT_PTR CALLBACK    AssignDialogProc(HWND, UINT, WPARAM, LPARAM);
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

	Common::MIDIMT::LangInterface& lang = Common::MIDIMT::LangInterface::Get();
	lang.SetMainHinstance(hInstance);

	if (!Common::MIDIMT::CheckRun::Get().Begin())
		return 0;

	INITCOMMONCONTROLSEX ctrl{};
	ctrl.dwICC = ICC_USEREX_CLASSES |
				 ICC_STANDARD_CLASSES |
				 ICC_LINK_CLASS |
				 ICC_BAR_CLASSES |
				 ICC_UPDOWN_CLASS |
				 ICC_HOTKEY_CLASS |
				 ICC_ANIMATE_CLASS |
				 ICC_LISTVIEW_CLASSES |
				 ICC_INTERNET_CLASSES |
				 ICC_NATIVEFNTCTL_CLASS |
				 ICC_PAGESCROLLER_CLASS;
	ctrl.dwSize = sizeof(ctrl);
	::InitCommonControlsEx(&ctrl);
	(void) Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, 0);

	RegisterMainClass();
	trayn = std::make_unique<Common::MIDIMT::TrayNotify>();

	if (!InitInstance(nCmdShow)) return 0;
	if (::CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE) != S_OK) return 0;

	Common::to_log::Get().filelog();
	Common::worker_background::Get().start();

	menu = std::make_unique<Common::MIDIMT::TrayMenu>();
	dlgs = std::make_unique<Common::MIDIMT::DialogStart>();
	dlgc = std::make_unique<Common::MIDIMT::DialogConfig>();
	dlgm = std::make_unique<Common::MIDIMT::DialogMonitor>();
	dlga = std::make_unique<Common::MIDIMT::DialogAbout>();
	mctrl = std::make_unique<Common::MIDIMT::AudioMixerPanels>(trayn.get());

	MSG msg;
	HACCEL ha = lang.GetAccelerators(MAKEINTRESOURCEW(IDC_MIDIMT_ACCEL));

	dlgs->AutoStart();

	while (GetMessageW(&msg, nullptr, 0, 0)) {
		if (!TranslateAcceleratorW(msg.hwnd, ha, &msg)) {
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
		Gdiplus::GdiplusShutdown(gdiplusToken);
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
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDS_MIDIMT);
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

	lang.SetMainHwnd(hwnd);
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT m, WPARAM w, LPARAM l) {
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
				case IDM_GO_CONFIGURE: {
					if (dlgc->IsRunOnce() && dlgs->IsRunOnce() && dlgm->IsRunOnce())
						Common::MIDIMT::LangInterface::Get().GetDialog(hwnd, ConfigDialogProc, MAKEINTRESOURCEW(IDD_FORMSETUP), l);
					else
						dlgc->SetFocus();
					return true;
				}
				case IDM_GO_ABOUT: {
					Common::MIDIMT::LangInterface::Get().GetDialog(hwnd, AboutDialogProc, MAKEINTRESOURCEW(IDD_FORMABOUT));
					return true;
				}
				case IDM_GO_STOP: {
					try {
						mctrl->Close();
						dlgs->Stop();
					} catch (...) {}
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
					try {
						mctrl->Close();
						dlgs->Stop();
						dlgs->EndDialog();
						dlgm->EndDialog();
						dlgc->EndDialog();
					} catch (...) {}
					::DestroyWindow(hwnd);
					return true;
				}
				default: return DefWindowProc(hwnd, m, w, l);
			}
			break;
		}
		case WM_COPYDATA: {
			COPYDATASTRUCT* cds = Common::MIDIMT::CheckRun::build(reinterpret_cast<COPYDATASTRUCT*>(l));
			::PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_GO_CONFIGURE, 0), reinterpret_cast<LPARAM>(cds));
			return true;
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
INT_PTR CALLBACK AboutDialogProc(HWND hwnd, UINT m, WPARAM w, LPARAM l) {
	UNREFERENCED_PARAMETER(l);
	switch (m) {
		case WM_INITDIALOG: {
			dlga->InitDialog(hwnd);
			return (INT_PTR)true;
		}
		case WM_COMMAND: {
			switch (LOWORD(w)) {
				case IDC_OK3: {
					ShellExecuteW(0, 0, Common::MIDIMT::LangInterface::Get().GetString(IDS_URL_WIKI).c_str(), 0, 0, SW_SHOW);
					break;
				}
				case IDC_OK2: {
					ShellExecuteW(0, 0, Common::MIDIMT::LangInterface::Get().GetString(IDS_URL_GIT).c_str(), 0, 0, SW_SHOW);
					break;
				}
				case IDOK:
				case IDCANCEL:
				case IDM_DIALOG_EXIT: {
					dlga->EndDialog();
					EndDialog(hwnd, LOWORD(w));
					return (INT_PTR)true;
					break;
				}
			}
			break;
		}
	}
	return (INT_PTR)false;
}
INT_PTR CALLBACK AssignDialogProc(HWND hwnd, UINT m, WPARAM w, LPARAM l)
{
	UNREFERENCED_PARAMETER(l);
	switch (m) {
		case WM_INITDIALOG: {
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			switch (LOWORD(w)) {
				case IDC_OK2: {
					ShellExecute(0, 0, Common::MIDIMT::LangInterface::Get().GetString(IDS_URL_ESETUP).c_str(), 0, 0, SW_SHOW);
					break;
				}
				case IDOK:
				case IDCANCEL:
				case IDM_DIALOG_EXIT: {
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
		case WM_DROPFILES: {
			dlgs->OpenDragAndDrop(Common::MIDIMT::Gui::GetDragAndDrop(reinterpret_cast<HDROP>(w)));
			break;
		}
		case WM_HELP: {
			Common::MIDIMT::Gui::ShowHelpPage(IDD_FORMSTART, reinterpret_cast<HELPINFO*>(l));
			return true;
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
				case IDC_DIALOG_SAVE: {
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
				case IDC_MQTT_LOGLEVEL: {
					if (HIWORD(w) == CBN_SELENDOK) /* CBN_SELCHANGE */
						dlgs->ChangeOnSmartHouseLogLevel();
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
				case IDC_MIXER_DUPLICATE: {
					dlgs->ChangeOnMixerDupAppRemove();
					break;
				}
				case IDC_MQTT_ENABLE: {
					dlgs->ChangeOnSmartHouseEnable();
					break;
				}
				case IDC_MQTT_ISSSL: {
					dlgs->ChangeOnSmartHouseSsl();
					break;
				}
				case IDC_MQTT_ISSELFSIGN: {
					dlgs->ChangeOnSmartHouseSelfSign();
					break;
				}
				case IDC_MQTT_CAOPEN: {
					dlgs->ChangeOnSmartHouseCa();
					break;
				}
				case IDC_MQTT_PSK:
				case IDC_MQTT_PORT:
				case IDC_MQTT_PASS:
				case IDC_MQTT_LOGIN:
				case IDC_MQTT_IPADDR:
				case IDC_MQTT_PREFIX: {
					if (HIWORD(w) != EN_CHANGE) break;
					switch (LOWORD(w)) {
						case IDC_MQTT_IPADDR: {
							dlgs->ChangeOnSmartHouseIpAddress();
							break;
						}
						case IDC_MQTT_PORT: {
							dlgs->ChangeOnSmartHousePort();
							break;
						}
						case IDC_MQTT_LOGIN: {
							dlgs->ChangeOnSmartHouseLogin();
							break;
						}
						case IDC_MQTT_PASS: {
							dlgs->ChangeOnSmartHousePass();
							break;
						}
						case IDC_MQTT_PSK: {
							dlgs->ChangeOnSmartHousePsk();
							break;
						}
						case IDC_MQTT_PREFIX: {
							dlgs->ChangeOnSmartHousePrefix();
							break;
						}
						default: break;
					}
					break;
				}
				case IDC_IEVENT_LOG: {
					dlgs->EventLog(reinterpret_cast<Common::MIDIMT::CbEventData*>(l));
					break;
				}
				case IDC_IEVENT_MONITOR: {
					dlgs->EventMonitor(reinterpret_cast<Common::MIDIMT::CbEventData*>(l));
					break;
				}
				case IDCANCEL:
				case IDM_DIALOG_EXIT: {
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
		case WM_HELP: {
			Common::MIDIMT::Gui::ShowHelpPage(IDD_FORMMONITOR, reinterpret_cast<HELPINFO*>(l));
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
					dlgm->EventLog(reinterpret_cast<Common::MIDIMT::CbEventData*>(l));
					break;
				}
				case IDC_IEVENT_MONITOR: {
					dlgm->EventMonitor(reinterpret_cast<Common::MIDIMT::CbEventData*>(l));
					break;
				}
				case IDCANCEL:
				case IDM_DIALOG_EXIT: {
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
			if (!l)
				dlgc->InitDialog(hwnd);
			else
				dlgc->InitDialog(hwnd, reinterpret_cast<COPYDATASTRUCT*>(l));
			return true;
		}
		case WM_DROPFILES: {
			dlgc->OpenDragAndDrop(Common::MIDIMT::Gui::GetDragAndDrop(reinterpret_cast<HDROP>(w)));
			break;
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
				case 0: {
					switch (lpmh->code) {
						case HDN_FILTERCHANGE:
						case HDN_FILTERBTNCLICK: {
							dlgc->ListViewFilter(lpmh);
							break;
						}
						default: return false;
					}
					return true;
				}
				default: break;
			}
			break;
		}
		case WM_HELP: {
			Common::MIDIMT::Gui::ShowHelpPage(IDD_FORMSETUP, reinterpret_cast<HELPINFO*>(l));
			return true;
		}
		case WM_COMMAND: {
			switch (LOWORD(w)) {
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
				case IDM_LV_SET_MQTT:
				case IDM_LV_SET_MMKEY:
				case IDM_LV_SET_MIXER: {
					dlgc->ListViewMenu(LOWORD(w));
					break;
				}
				case IDM_LV_EDIT_MODE: {
					dlgc->ToolBarEditDigitMode();
					break;
				}
				case IDM_DIALOG_SAVE: {
					dlgc->ToolBarSave();
					break;
				}
				case IDM_DIALOG_IMPORT: {
					dlgc->ToolBarImport();
					break;
				}
				case IDM_DIALOG_EXPORT: {
					dlgc->ToolBarExport();
					break;
				}
				case IDM_DIALOG_LAST_OPEN: {
					dlgc->ToolBarRecentOpen(HIWORD(w));
					break;
				}
				case IDM_LV_READ_MIDI_CODE: {
					dlgc->ToolBarMonitor();
					break;
				}
				case IDM_LV_EXT_MODE: {
					dlgc->ToolBarSetMode();
					break;
				}
				case IDM_LV_FILTER_EMBED: {
					dlgc->ToolBarFilterEmbed();
					break;
				}
				case IDM_LV_PASTE_NOTIFY: {
					dlgc->ToolBarEditorNotify(static_cast<Common::MIDIMT::EditorNotify>(HIWORD(w)));
					break;
				}
				case IDM_LV_SORTUP_KEY:
				case IDM_LV_SORTUP_SCENE:
				case IDM_LV_SORTUP_TARGET:
				case IDM_LV_SORTUP_TARGETLONG:
				case IDM_LV_SORTDOWN_KEY:
				case IDM_LV_SORTDOWN_SCENE:
				case IDM_LV_SORTDOWN_TARGET:
				case IDM_LV_SORTDOWN_TARGETLONG: {
					dlgc->ToolBarSort(LOWORD(w));
					break;
				}
				case IDM_LV_FILTER_ON:
				case IDM_LV_FILTER_OFF:
				case IDM_LV_FILTER_SET:
				case IDM_LV_FILTER_MQTT:
				case IDM_LV_FILTER_MMKEY:
				case IDM_LV_FILTER_MIXER:
				case IDM_LV_FILTER_CLEAR: {
					dlgc->ToolBarFilters(LOWORD(w));
					break;
				}
				case IDM_LV_FILTER_TYPEOR:
				case IDM_LV_FILTER_TYPEAND: {
					dlgc->ToolBarFilterType(LOWORD(w));
					break;
				}
				case IDM_LV_FILTER_AUTOCOMMIT: {
					dlgc->ToolBarFilterAutoCommit();
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
				case IDC_BTN_INFO:
				case IDM_DIALOG_SHOW_MIDI_KEYS: {
					Common::MIDIMT::LangInterface::Get().GetDialog(hwnd, AssignDialogProc, MAKEINTRESOURCEW(IDD_ASSIGNBOX));
					break;
				}
				case IDC_IEVENT_LOG: {
					dlgc->EventLog(reinterpret_cast<Common::MIDIMT::CbEventData*>(l));
					break;
				}
				case IDC_IEVENT_MONITOR: {
					dlgc->EventMonitor(reinterpret_cast<Common::MIDIMT::CbEventData*>(l));
					break;
				}
				case IDCANCEL:
				case IDM_DIALOG_EXIT: {
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