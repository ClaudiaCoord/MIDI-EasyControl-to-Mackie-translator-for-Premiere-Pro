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

		static const wchar_t guidString[]{ APPGUID };
		static inline std::wstring state_bool__(LangInterface& lang, const bool b) {
			return b ? lang.GetString(STRING_LANG_YES) : lang.GetString(STRING_LANG_NO);
		}

		void TrayNotify::Init(HWND hwnd, uint32_t id, std::wstring& title) {
			data_.cbSize = sizeof(NOTIFYICONDATA);
			data_.hWnd = hwnd;
			data_.uID = static_cast<UINT>(ICON_APP_MIDIMT_SMALL);
			data_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			data_.uVersion = NOTIFYICON_VERSION_4;
			data_.hIcon = LangInterface::Get().GetIcon(MAKEINTRESOURCE(ICON_APP_MIDIMT_SMALL));
			data_.uCallbackMessage = id;
			data_.guidItem = Utils::guid_from_string(guidString);
			::wcscpy_s(data_.szTip, title.c_str());
		}
		void TrayNotify::Install() {
			::Shell_NotifyIconW(NIM_ADD, &data_);
			::Shell_NotifyIconW(NIM_SETVERSION, &data_);
		}
		void TrayNotify::UnInstall() {
			data_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			::Shell_NotifyIconW(NIM_DELETE, &data_);
		}
		void TrayNotify::Show() {
			try {
				data_.uFlags |= NIF_INFO;
				data_.uTimeout = 1000;
				data_.dwInfoFlags = 0U;

				auto& lang = LangInterface::Get();
				auto& br = IO::IOBridge::Get();

				::wcscpy_s(data_.szInfoTitle, lang.GetString(br.IsStarted() ? STRING_NOTIFY_MSG2 : STRING_NOTIFY_MSG1).c_str());

				if (br.IsStarted()) {
					log_string ls{};

					for (uint16_t i = 0; i < br.PluginCount(); i++) {
						try {
							IO::plugin_t& p = br[i];
							if (p->empty()) continue;

							std::wstring name = p.get()->GetPluginInfo().File();
							if (name.empty()) continue;
							if (name.starts_with(L"MMTPLUGINx"))
								name = name.substr(10);

							ls << name.c_str() << L": "
								<< std::vformat(
									 std::wstring_view(common_error_code::Get().get_error(common_error_id::err_PLUGIN_LIST_INFO)),
									 std::make_wformat_args(
										 state_bool__(lang, p->enabled()),
										 state_bool__(lang, p->configure()),
										 state_bool__(lang, p->started())
									 )
								).c_str()
								<< L"\n";

							if (!ls.empty())
								::wcscpy_s(data_.szInfo, _countof(data_.szInfo), ls.str().c_str());

						} catch (...) {
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						}
					}
				}
				else if (br.IsLoaded()) {
					log_string ls{};
					auto& state = br.GetState();

					ls << std::vformat(
							 std::wstring_view(common_error_code::Get().get_error(common_error_id::err_BRIDGE_OK_INIT_LOAD)),
							 std::make_wformat_args(
								 std::get<0>(state),
								 std::get<1>(state)
							 )
						).c_str()
						<< L"\n";

					if (!ls.empty())
						::wcscpy_s(data_.szInfo, _countof(data_.szInfo), ls.str().c_str());
				}
				else {
					log_string ls{};
					auto& state = br.GetState();

					if (std::get<0>(state) == 0U)
						ls << common_error_code::Get().get_error(common_error_id::err_BRIDGE_NOT_INIT).c_str()
						   << L"\n";
					else
						ls << std::vformat(
								 std::wstring_view(common_error_code::Get().get_error(common_error_id::err_BRIDGE_OK_STOP)),
								 std::make_wformat_args(
									 std::get<0>(state),
									 std::get<1>(state),
									 std::get<2>(state),
									 std::get<3>(state)
								 )
							).c_str()
							<< L"\n";

					if (!ls.empty())
						::wcscpy_s(data_.szInfo, _countof(data_.szInfo), ls.str().c_str());
				}
				::Shell_NotifyIconW(NIM_MODIFY, &data_);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void TrayNotify::Warning(std::wstring title, std::wstring body) {
			try {
				if (body.empty()) return;

				data_.uFlags |= NIF_INFO;
				data_.uTimeout = 1000;
				data_.dwInfoFlags = 0U;
				std::wstring s = (log_string() << body << L"\r\n");

				::wcscpy_s(data_.szInfoTitle, title.c_str());
				::wcscpy_s(data_.szInfo, s.c_str());
				::Shell_NotifyIconW(NIM_MODIFY, &data_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
	}
}