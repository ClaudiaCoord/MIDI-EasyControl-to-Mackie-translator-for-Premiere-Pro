/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

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
#pragma comment(lib, "EASYCTRL9.lib")
#pragma comment(lib, "MIDIMTBR.lib")
#pragma comment(lib, "MIDIMTMIX.lib")
#pragma comment(lib, "MIDIMTVEUI.lib")

UINT const WMAPP_SHELLICON = WM_APP + 1;
UINT const WMAPP_ASYNC_RUN = WM_APP + 2;
Gdiplus::GdiplusStartupInput gdiplusStartupInput{};
ULONG_PTR					 gdiplusToken{};

ATOM                reg_mainclass_();
bool                init_instance_();
LRESULT CALLBACK    wnd_proc_(HWND, UINT, WPARAM, LPARAM);

	#pragma region local Dialog template
	template <class T1>
	const bool IsDialog() {
		return Common::MIDIMT::ClassStorage::Get().IsDialog<T1>();
	}
	template <class T1>
	std::unique_ptr<T1>& GetDialog() {
		return Common::MIDIMT::ClassStorage::Get().GetDialog<T1>();
	}
	template <class T1>
	void OpenDialog(HWND h, bool b) {
		(void) Common::MIDIMT::ClassStorage::Get().OpenDialog<T1>(h, b);
	}
	#pragma endregion

LONG WINAPI uexception_handler(PEXCEPTION_POINTERS ep) {
	if ((ep) && (ep->ExceptionRecord))
		Common::to_log::Get() << (Common::log_string() << __FUNCTIONW__ << L", Unhandled exception: " << std::hex << ep->ExceptionRecord->ExceptionCode);
	return EXCEPTION_CONTINUE_SEARCH;
}

int APIENTRY wWinMain(
	_In_ HINSTANCE HINST_,
	_In_opt_ HINSTANCE,
	_In_ LPWSTR,
	_In_ int) {

	::SetUnhandledExceptionFilter(uexception_handler);
	std::set_terminate([]() {
		try {
			Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		} catch (...) {
			Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
		#if defined(_DEBUG)
		std::abort();
		#endif
	});

	Common::MIDIMT::LangInterface& lang = Common::MIDIMT::LangInterface::Get();
	lang.SetMainHinstance(HINST_);

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
	(void) reg_mainclass_();

	if (!init_instance_()) return 0;
	if (::CoInitializeEx(0, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE) != S_OK) return 0;

	try {
		Common::to_log::Get().filelog(HINST_);
		Common::worker_background& wb = Common::worker_background::Get();
		wb.start();
		wb.to_async(std::async(std::launch::async, []() {

			bool isrun{ true };
			Common::to_log& log = Common::to_log::Get();
			Common::common_config& cnf = Common::common_config::Get();
			Common::MIDIMT::LangInterface& lang = Common::MIDIMT::LangInterface::Get();

			if (log.filelog()) {
				log << (Common::log_string()
					<< lang.GetString(STRING_CHKR_MSG2)
					<< L": "
					<< cnf.GetConfig()->config
				);
				log << cnf.GetConfigPath();
			}
			if (!cnf.Load() || cnf.IsConfigEmpty()) {
				log << lang.GetString(STRING_EDIT_MSG3);
				isrun = false;
			}
			if (isrun && !cnf.Registry.GetAutoRun()) {
				log << lang.GetString(STRING_MAIN_MSG3);
				isrun = false;
			}
			if (isrun && !cnf.GetConfig()->auto_start) {
				log << lang.GetString(STRING_MAIN_MSG4);
				isrun = false;
			}
			Common::IO::IOBridge& br = Common::IO::IOBridge::Get();
			if (!br.Init(lang.GetMainHwnd()) || !br.IsLoaded()) return;

			if (isrun)
				(void) Common::MIDIMT::ClassStorage::Get().Start(true);

		}));
	} catch (...) {
		::MessageBoxW(0, lang.GetString(STRING_MAIN_MSG1).c_str(), lang.GetString(STRING_MAIN_MSG2).c_str(), MB_OK | MB_ICONERROR);
		return -1;
	}

	MSG msg{};
	HACCEL ha = lang.GetAccelerators(MAKEINTRESOURCEW(IDC_MIDIMT_ACCEL));

	while (GetMessageW(&msg, nullptr, 0, 0))
		if (!TranslateAcceleratorW(msg.hwnd, ha, &msg)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

	try {
		Common::MIDIMT::ClassStorage::Get().Stop();

		if (IsDialog<Common::MIDIMT::TrayMenu>())
			GetDialog<Common::MIDIMT::TrayMenu>()->End();

	} catch (...) { Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
	try {
		::CoUninitialize();
		Gdiplus::GdiplusShutdown(gdiplusToken);
	} catch (...) { Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
	try {
		Common::to_log::Get().end();
		Common::worker_background::Get().stop();
	} catch (...) {}

	return (int) msg.wParam;
}

ATOM reg_mainclass_() {
	auto& lang = Common::MIDIMT::LangInterface::Get();
	WNDCLASSEXW w{};

	w.cbSize = sizeof(WNDCLASSEX);

	w.style = CS_HREDRAW | CS_VREDRAW;
	w.lpfnWndProc = wnd_proc_;
	w.cbClsExtra = 0;
	w.cbWndExtra = 0;
	w.hInstance = lang.GetMainHinstance();
	w.hIcon = lang.GetIcon(MAKEINTRESOURCEW(ICON_APP_MIDIMT));
	w.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
	w.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	w.lpszMenuName = MAKEINTRESOURCEW(STRING_APP_CLASS);
	w.lpszClassName = lang.GetMainClass().c_str();
	w.hIconSm = lang.GetIcon(MAKEINTRESOURCEW(ICON_APP_MIDIMT_SMALL));

	return ::RegisterClassExW(&w);
}
bool init_instance_() {
	using Common::MIDIMT::ClassStorage;

	Common::MIDIMT::LangInterface& lang = Common::MIDIMT::LangInterface::Get();
	HWND hwnd = CreateWindowW(
		lang.GetMainClass().c_str(),
		lang.GetMainTitle().c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, lang.GetMainHinstance(), nullptr);
	if (!hwnd) return false;

	lang.SetMainHwnd(hwnd);
	auto& notify = ClassStorage::Get().GetDialog<Common::MIDIMT::TrayNotify>();
	notify->Init(hwnd, WMAPP_SHELLICON, lang.GetMainTitle());

	::ShowWindow(hwnd, SW_HIDE);
	::UpdateWindow(hwnd);
	notify->Install();
	return true;
}

LRESULT CALLBACK wnd_proc_(HWND h, UINT m, WPARAM w, LPARAM l) {
	switch (m) {
		case WM_HELP: {
			if (!l) break;
			Common::UI::UiUtils::ShowHelpPage(Common::MIDIMT::LangInterface::Get().GetHelpLangId(), static_cast<uint16_t>(w), reinterpret_cast<HELPINFO*>(l));
			return static_cast<INT_PTR>(1);
		}
		case WM_COMMAND: {
			switch (LOWORD(w)) {
				case IDM_GO_EMPTY: {
					if (!Common::MIDI::MidiDevices::Get().CheckVirtualDriver())
						Common::UI::UiUtils::ShowHelpPage(Common::MIDIMT::LangInterface::Get().GetHelpLangId(), DLG_START_WINDOW, DLG_GO_ABOUT);
					break;
				}
				case IDM_GO_STOP: {
					(void) Common::MIDIMT::ClassStorage::Get().StopAsync();
					return static_cast<INT_PTR>(1);
				}
				case IDM_GO_START: {
					OpenDialog<Common::MIDIMT::DialogStart>(h, true);
					return static_cast<INT_PTR>(1);
				}
				case IDM_GO_MONITOR: {
					OpenDialog<Common::MIDIMT::DialogMonitor>(h, true);
					return static_cast<INT_PTR>(1);
				}
				case IDM_GO_VIEWLOG: {
					OpenDialog<Common::MIDIMT::DialogLogView>(h, false);
					return static_cast<INT_PTR>(1);
				}
				case IDM_GO_EDIT: {
					if (!l)
						OpenDialog<Common::MIDIMT::DialogEdit>(h, true);
					else {
						auto& dlg = GetDialog<Common::MIDIMT::DialogEdit>();
						dlg->ExternalConfig(reinterpret_cast<COPYDATASTRUCT*>(l));
						(void) dlg->BuildDialog(h);
					}
					return static_cast<INT_PTR>(1);
				}
				case IDM_GO_ABOUT: {
					OpenDialog<Common::MIDIMT::DialogAbout>(h, true);
					return static_cast<INT_PTR>(1);
				}
				case IDM_GO_INFO: {
					GetDialog<Common::MIDIMT::TrayNotify>()->Show();
					return static_cast<INT_PTR>(1);
				}
				case IDM_GO_MIXER: {
					GetDialog<Common::MIDIMT::AudioMixerPanels>()->Show();
					return static_cast<INT_PTR>(1);
				}
				case IDM_GO_MIXER_INIT: {
					if (Common::common_config::Get().Registry.GetMixerEnable())
						GetDialog<Common::MIXER::AudioSessionMixer>()->Start();

					return static_cast<INT_PTR>(1);
				}
				case IDM_EXIT: {
					try {
						try {
							Common::MIDIMT::ClassStorage::Get().Stop();
						} catch (...) {}
						::DestroyWindow(h);
						return static_cast<INT_PTR>(1);
					} catch (...) {}
					return static_cast<INT_PTR>(0);
				}
				default: return DefWindowProcW(h, m, w, l);
			}
			break;
		}
		case WM_COPYDATA: {
			COPYDATASTRUCT* cds = Common::MIDIMT::CheckRun::build(reinterpret_cast<COPYDATASTRUCT*>(l));
			::PostMessageW(h, WM_COMMAND, MAKEWPARAM(IDM_GO_EDIT, 0), reinterpret_cast<LPARAM>(cds));
			return true;
		}
		case WMAPP_SHELLICON: {
			switch (LOWORD(l)) {
				case WM_LBUTTONUP: {
					GetDialog<Common::MIDIMT::TrayMenu>()->Show(h, POINT(LOWORD(w), HIWORD(w)));
					return static_cast<INT_PTR>(1);
				}
				case WM_CONTEXTMENU: {
					Common::common_config& cnf = Common::common_config::Get();
					if (cnf.Registry.GetMixerRightClick() && cnf.Local.IsAudioMixerRun())
						GetDialog<Common::MIDIMT::AudioMixerPanels>()->Show();
					else
						GetDialog<Common::MIDIMT::TrayMenu>()->Show(h, POINT(LOWORD(w), HIWORD(w)));
					return static_cast<INT_PTR>(1);
				}
				default: break;
			}
			break;
		}
		case WMAPP_ASYNC_RUN: {
			Common::UI::UiUtils::PostExec(l);
			return static_cast<INT_PTR>(0);
		}
		case WM_PAINT: {
			break;
		}
		case WM_DESTROY: {
			if (IsDialog<Common::MIDIMT::TrayNotify>())
				GetDialog<Common::MIDIMT::TrayNotify>()->UnInstall();

			::PostQuitMessage(0);
			return static_cast<INT_PTR>(1);
		}
		default: return DefWindowProc(h, m, w, l);
	}
	return static_cast<INT_PTR>(0);
}
