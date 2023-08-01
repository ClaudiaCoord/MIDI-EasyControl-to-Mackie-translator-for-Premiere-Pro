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

namespace Common {
	namespace MIDIMT {

		static const wchar_t guidString[] = L"{cd3e2202-b792-4fd9-b9b1-313224a46d76}";

		TrayNotify::TrayNotify() : guid__(GUID_NULL) {}
		TrayNotify::~TrayNotify() {}

		void TrayNotify::Init(HWND hwnd, uint32_t id, std::wstring& title) {
			data__.cbSize = sizeof(NOTIFYICONDATA);
			data__.hWnd = hwnd;
			data__.uID = static_cast<UINT>(IDI_SMALL);
			data__.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			data__.uVersion = NOTIFYICON_VERSION_4;
			data__.hIcon = LangInterface::Get().GetIcon(MAKEINTRESOURCE(IDI_SMALL));
			data__.uCallbackMessage = id;
			if (CLSIDFromString(guidString, (LPCLSID)&guid__) == S_OK)
				data__.guidItem = guid__;
			::wcscpy_s(data__.szTip, title.c_str());
		}
		void TrayNotify::Install() {
			::Shell_NotifyIconW(NIM_ADD, &data__);
			::Shell_NotifyIconW(NIM_SETVERSION, &data__);
		}
		void TrayNotify::UnInstall() {
			data__.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			Shell_NotifyIcon(NIM_DELETE, &data__);
		}
		void TrayNotify::Show() {
			try {
				data__.uFlags |= NIF_INFO;
				data__.uTimeout = 1000;
				data__.dwInfoFlags = 0U;

				auto& cnf = common_config::Get();
				auto& lang = LangInterface::Get();

				if (cnf.Local.IsMidiBridgeRun()) {
					::wcscpy_s(data__.szInfoTitle, lang.GetString(IDS_INFO2).c_str());
					if (cnf.IsConfig()) {
						std::wstring ws = cnf.GetConfig()->name;
						if (!ws.empty()) {

							std::wstringstream wss;
							wss << lang.GetString(IDS_INFO3) << L" " << ws << L"\r\n";
							wss << lang.GetString(IDS_INFO4) << L" " << ws << MIDI::MidiHelper::GetSuffixMackieOut() << L"\r\n";

							if (cnf.IsProxy()) {
								uint32_t cnt = cnf.GetConfig()->proxy;
								wss << lang.GetString(IDS_INFO5) << L"\r\n";

								if (cnt == 1)
									wss << L"\t\t" << ws << MIDI::MidiHelper::GetSuffixProxyOut() << L"1\r\n";
								else
									for(uint32_t i = 0; i < cnt; i++)
										wss << L"\t\t" << ws << MIDI::MidiHelper::GetSuffixProxyOut() << std::to_wstring(i + 1U) << L"\r\n";
							}
							if (cnf.Local.IsAudioMixerRun())
								wss << lang.GetString(IDS_INFO6) << L"\r\n";
							if (cnf.Local.IsMMKeyesRun())
								wss << lang.GetString(IDS_INFO7) << L"\r\n";
							if (cnf.Local.IsSmartHomeRun())
								wss << lang.GetString(IDS_INF13) << L"\r\n";

							::wcscpy_s(data__.szInfo, _countof(data__.szInfo), wss.str().c_str());
						}
					}
				} else {
					::wcscpy_s(data__.szInfoTitle, lang.GetString(IDS_INFO1).c_str());
					::wcscpy_s(data__.szInfo, L"-");
				}
				::Shell_NotifyIconW(NIM_MODIFY, &data__);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void TrayNotify::Warning(std::wstring title, std::wstring body) {
			try {
				data__.uFlags |= NIF_INFO;
				data__.uTimeout = 1000;
				data__.dwInfoFlags = 0U;
				std::wstringstream wss;
				wss << body << L"\r\n";

				::wcscpy_s(data__.szInfoTitle, title.c_str());
				::wcscpy_s(data__.szInfo, wss.str().c_str());
				::Shell_NotifyIconW(NIM_MODIFY, &data__);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
	}
}