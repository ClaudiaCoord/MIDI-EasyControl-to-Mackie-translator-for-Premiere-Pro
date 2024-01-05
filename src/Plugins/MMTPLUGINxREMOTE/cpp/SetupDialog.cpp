/*
	MIDI-MT is a high level application driver for USB MIDI control surface.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (RemoteControlsPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace PLUGINS {

		typedef asio::ip::address_v4 IpAddr;

		RemoteSetupDialog::RemoteSetupDialog(IO::PluginCb& cb) : cb_(cb) {
		}
		RemoteSetupDialog::~RemoteSetupDialog() {
			dispose_();
		}

		void RemoteSetupDialog::dispose_() {
			isload_ = false;
			hwnd_.reset();
			btn_copy_.Release();
		}
		void RemoteSetupDialog::init_() {
			try {
				isload_ = false;
				auto f = std::async(std::launch::async, [=]() ->bool {
					auto& mmt = common_config::Get().GetConfig();
					config_.Copy(mmt->remoteconf);
					return true;
				});
				const bool _ = f.get();

				try {
					uint16_t btn_ico[]{ ICON_PLUGIN_REMOTE };
					btn_copy_.Init(
						btn_ico,
						std::size(btn_ico),
						[=](uint16_t n) -> HICON {
							return (HICON)::LoadImageW(hinst_, MAKEINTRESOURCEW(n), IMAGE_ICON, 24, 24, 0x0);
						}
					);
					btn_copy_.Init(hwnd_, DLG_PLUG_REMOTE_COPY_ADDR);

				} catch (...) {}

				::CheckDlgButton(hwnd_, DLG_PLUG_REMOTE_ISENABLE, CHECKBTN(config_.enable));

				::CheckDlgButton(hwnd_, DLG_PLUG_REMOTE_ISIPV6, CHECKBTN(config_.isipv6));
				::CheckDlgButton(hwnd_, DLG_PLUG_REMOTE_ISREUSE, CHECKBTN(config_.isreuseaddr));
				::CheckDlgButton(hwnd_, DLG_PLUG_REMOTE_ISFASTSOCK, CHECKBTN(config_.isfastsocket));

				::SetDlgItemTextW(hwnd_, DLG_PLUG_REMOTE_UA, config_.server_ua.c_str());
				::SetDlgItemTextW(hwnd_, DLG_PLUG_REMOTE_TMC, std::to_wstring(config_.timeoutreq).c_str());
				::SetDlgItemTextW(hwnd_, DLG_PLUG_REMOTE_TMI, std::to_wstring(config_.timeoutidle).c_str());
				::SetDlgItemTextW(hwnd_, DLG_PLUG_REMOTE_PORT, std::to_wstring(config_.port).c_str());

				if (config_.host.empty())
					::SetDlgItemTextW(hwnd_, DLG_PLUG_REMOTE_SYSLINK, L"-");
				else
					::SetDlgItemTextW(hwnd_, DLG_PLUG_REMOTE_SYSLINK, buildServerUrl_(config_.host, config_.port).c_str());

				isload_ = true;

				buildLogLevelComboBox_(config_.loglevel);
				buildNetInterfaceComboBox_(config_.host);
				::SetFocus(hwnd_);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		IO::PluginUi* RemoteSetupDialog::GetPluginUi() {
			return static_cast<IO::PluginUi*>(this);
		}

		#pragma region Builds events
		std::wstring RemoteSetupDialog::buildServerUrl_(const std::wstring& host, uint16_t port, bool issyslink) {
			return (log_string() << (issyslink ? L"<a>" : L"") << L"http://" << host << L":" << port << L"/" << (issyslink ? L"</a>" : L"")).str();
		}
		std::vector<std::wstring> RemoteSetupDialog::buildNetInterfaceList_() {
			std::vector<std::wstring> list{};
			PIP_ADAPTER_ADDRESSES paddr = nullptr;
			try {
				DWORD paddrsize = 1 << 19;
				PIP_ADAPTER_ADDRESSES paddr = (PIP_ADAPTER_ADDRESSES) new uint8_t[paddrsize];

				ULONG r = ::GetAdaptersAddresses(
					AF_UNSPEC,
					GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_DNS_SERVER,
					0, paddr, &paddrsize);

				if (r == NO_ERROR) {
					for (PIP_ADAPTER_ADDRESSES addr = paddr; addr != 0; addr = addr->Next) {
						if ((addr->OperStatus != IfOperStatusUp) || addr->NoMulticast) continue;
						if (addr->Ipv4Enabled) {
							for (PIP_ADAPTER_UNICAST_ADDRESS_LH maddr = addr->FirstUnicastAddress; maddr != 0; maddr = maddr->Next) {

								SOCKET_ADDRESS sa = maddr->Address;
								if (sa.lpSockaddr->sa_family != AF_INET) continue;
								IpAddr ip = asio::ip::make_address_v4(::ntohl(reinterpret_cast<sockaddr_in*>(sa.lpSockaddr)->sin_addr.s_addr));
								list.push_back(Utils::to_string(ip.to_string()));
							}
						}
					}
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			if (paddr)
				try { delete[] paddr; } catch (...) {}
			return list;
		}
		void RemoteSetupDialog::buildNetInterfaceComboBox_(const std::wstring& val) {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_REMOTE_IFACE_COMBO))) return;

				ComboBox_ResetContent(hi);

				auto f = std::async(std::launch::async, [=]() -> std::vector<std::wstring> {
					return buildNetInterfaceList_();
				});

				std::vector<std::wstring> list;
				try {
					list = f.get();
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					list = std::vector<std::wstring>();
				}

				if (list.empty()) {
					ComboBox_AddString(hi, common_error_code::Get().get_error(common_error_id::err_NET_IFACE_NONE).c_str());
					::CheckDlgButton(hwnd_, DLG_PLUG_REMOTE_ISENABLE, CHECKBTN(false));
					::EnableWindow(hi, false);
				} else {
					ComboBox_AddString(hi, common_error_code::Get().get_error(common_error_id::err_NET_IFACE_SELECT).c_str());
					for (auto& a : list)
						ComboBox_AddString(hi, a.c_str());

					if (!val.empty()) ComboBox_SelectString(hi, 0, val.c_str());
					else ComboBox_SetCurSel(hi, 0);
					::EnableWindow(hi, true);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RemoteSetupDialog::buildLogLevelComboBox_(uint16_t val) {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_REMOTE_LOGLEVEL_COMBO))) return;

				ComboBox_ResetContent(hi);

				const uint16_t field[] = {
					static_cast<uint16_t>(common_error_id::err_LOG_LEVEL_NONE),
					static_cast<uint16_t>(common_error_id::err_LOG_LEVEL_ERR),
					static_cast<uint16_t>(common_error_id::err_LOG_LEVEL_WARNING),
					static_cast<uint16_t>(common_error_id::err_LOG_LEVEL_INFO),
					static_cast<uint16_t>(common_error_id::err_LOG_LEVEL_DEBUG)
				};

				for (int32_t i = 0; i < static_cast<int32_t>(std::size(field)); i++)
					ComboBox_AddString(hi, common_error_code::Get().get_error(static_cast<common_error_id>(field[i])).c_str());

				ComboBox_SetCurSel(hi, val);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region Change On events
		void RemoteSetupDialog::changeOnEnable_() {
			try {
				if (!hwnd_) return;
				config_.enable = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_REMOTE_ISENABLE);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RemoteSetupDialog::changeOnIpv6_() {
			try {
				if (!hwnd_) return;
				config_.isipv6 = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_REMOTE_ISIPV6);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RemoteSetupDialog::changeOnFastSocket_() {
			try {
				if (!hwnd_) return;
				config_.isfastsocket = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_REMOTE_ISFASTSOCK);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RemoteSetupDialog::changeOnReuseAddr_() {
			try {
				if (!hwnd_) return;
				config_.isreuseaddr = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_REMOTE_ISREUSE);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RemoteSetupDialog::changeOnPort_() {
			try {
				if (!hwnd_) return;

				auto txt = UI::UiUtils::GetControlText(hwnd_, DLG_PLUG_REMOTE_PORT);
				if (txt.empty()) return;

				uint32_t p = std::stoul(txt);
				if (p < _UI16_MAX) {
					config_.port = static_cast<uint16_t>(p);
					::SetDlgItemTextW(hwnd_, DLG_PLUG_REMOTE_SYSLINK, buildServerUrl_(config_.host, config_.port).c_str());

					UI::UiUtils::SaveDialogEnabled(hwnd_);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RemoteSetupDialog::changeOnTmc_() {
			try {
				if (!hwnd_) return;

				auto txt = UI::UiUtils::GetControlText(hwnd_, DLG_PLUG_REMOTE_TMC);
				if (txt.empty()) return;

				config_.timeoutreq = std::stoul(txt);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RemoteSetupDialog::changeOnTmi_() {
			try {
				if (!hwnd_) return;

				auto txt = UI::UiUtils::GetControlText(hwnd_, DLG_PLUG_REMOTE_TMI);
				if (txt.empty()) return;

				config_.timeoutidle = std::stoul(txt);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RemoteSetupDialog::changeOnLogLevel_() {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_REMOTE_LOGLEVEL_COMBO))) return;

				int32_t val = static_cast<int32_t>(::SendMessageW(hi, CB_GETCURSEL, 0, 0));
				if (val == CB_ERR) return;

				config_.loglevel = static_cast<uint16_t>(val);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RemoteSetupDialog::changeOnNetInterface_() {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_REMOTE_IFACE_COMBO))) return;

				int32_t val = static_cast<int32_t>(::SendMessageW(hi, CB_GETCURSEL, 0, 0));
				if (val == CB_ERR) return;

				wchar_t buf[MAX_PATH]{};
				if ((::SendMessageW(hi, CB_GETLBTEXT, val, (LPARAM)&buf) == CB_ERR) || (buf[0] == L'\0')) return;

				config_.host = std::wstring(buf, buf + std::wcslen(buf));
				::SetDlgItemTextW(hwnd_, DLG_PLUG_REMOTE_SYSLINK, buildServerUrl_(config_.host, config_.port).c_str());

				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RemoteSetupDialog::changeOnCopyServerUrl_(bool isopenbrowser) {
			try {
				if (!hwnd_) return;

				std::wstring s = buildServerUrl_(config_.host, config_.port, false);
				if (s.empty()) return;
				size_t sz = (s.length() + 1) * sizeof(wchar_t);

				if (HGLOBAL mem; (mem = ::GlobalAlloc(GMEM_MOVEABLE, sz))) {
					if (void* p; (p = ::GlobalLock(mem))) {
						std::memcpy(p, s.data(), sz);
						::GlobalUnlock(mem);
					} else {
						::GlobalFree(mem);
						return;
					}
					if (::OpenClipboard(hwnd_)) {
						::EmptyClipboard();
						::SetClipboardData(CF_UNICODETEXT, mem);
						::CloseClipboard();
						cb_.ToLog(log_string().to_log_string(__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_COPY_LINK)
							).str()
						);
					}
					else ::GlobalFree(mem);
				}
				if (isopenbrowser)
					::ShellExecuteW(0, 0, s.c_str(), 0, 0, SW_SHOW);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RemoteSetupDialog::changeOnSaveConfig_() {
			try {
				if (!hwnd_) return;

				auto& mmt = common_config::Get().GetConfig();
				mmt->remoteconf.Copy(config_);
				UI::UiUtils::SaveDialogEnabled(hwnd_, false);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region Override
		LRESULT RemoteSetupDialog::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
			try {
				switch (m) {
					case WM_INITDIALOG: {
						hwnd_.reset(h, static_cast<SUBCLASSPROC>(&PluginUi::DialogProc_), reinterpret_cast<DWORD_PTR>(this), 0);
						init_();
						return static_cast<INT_PTR>(1);
					}
					case WM_NOTIFY: {
						if (!isload_ || (!l)) break;

						LPNMHDR lpmh = (LPNMHDR)l;
						switch (lpmh->idFrom) {
							case (UINT)DLG_PLUG_REMOTE_SYSLINK: {
								switch (lpmh->code) {
									case NM_CLICK: {
										changeOnCopyServerUrl_(true);
										return static_cast<INT_PTR>(1);
									}
									default: break;
								}
								break;
							}
							default: break;
						}
						break;
					}
					case WM_COMMAND: {
						if (!isload_) break;
						uint16_t c{ LOWORD(w) };
						switch (c) {
							case DLG_PLUG_SAVE: {
								changeOnSaveConfig_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_REMOTE_ISENABLE: {
								changeOnEnable_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_REMOTE_ISIPV6: {
								changeOnIpv6_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_REMOTE_ISFASTSOCK: {
								changeOnFastSocket_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_REMOTE_ISREUSE: {
								changeOnReuseAddr_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_REMOTE_PORT:
							case DLG_PLUG_REMOTE_TMC:
							case DLG_PLUG_REMOTE_TMI: {
								if (!isload_ || (HIWORD(w) != EN_CHANGE)) break;
								switch (c) {
									case DLG_PLUG_REMOTE_PORT: {
										changeOnPort_();
										break;
									}
									case DLG_PLUG_REMOTE_TMC: {
										changeOnTmc_();
										break;
									}
									case DLG_PLUG_REMOTE_TMI: {
										changeOnTmi_();
										break;
									}
									default: break;
								}
								break;
							}
							case DLG_PLUG_REMOTE_IFACE_COMBO: {
								if (HIWORD(w) == CBN_SELENDOK) /* CBN_SELCHANGE */
									changeOnNetInterface_();
								break;
							}
							case DLG_PLUG_REMOTE_LOGLEVEL_COMBO: {
								if (HIWORD(w) == CBN_SELENDOK) /* CBN_SELCHANGE */
									changeOnLogLevel_();
								break;
							}
							case DLG_PLUG_REMOTE_COPY_ADDR: {
								changeOnCopyServerUrl_(false);
								break;
							}
							case IDCANCEL:
							case DLG_PLUG_EXIT: {
								dispose_();
								return static_cast<INT_PTR>(1);
							}
							default: break;
						}
						break;
					}
					default: break;
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return ::DefSubclassProc(h, m, w, l);
		}
		#pragma endregion

	}
}