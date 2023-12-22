/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MMTPLUGINx* DLL (SmartHomeKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include <shellapi.h>
#include <Shlobj.h>
#include <Shlobj_core.h>
#include <objbase.h>

namespace Common {
	namespace PLUGINS {

		static const uint16_t elements_[] {
			DLG_PLUG_MQTT_IPADDR,
			DLG_PLUG_MQTT_PORT,
			DLG_PLUG_MQTT_LOGIN,
			DLG_PLUG_MQTT_PASS,
			DLG_PLUG_MQTT_PSK,
			DLG_PLUG_MQTT_ISSSL,
			DLG_PLUG_MQTT_ISSELFSIGN,
			DLG_PLUG_MQTT_CA,
			DLG_PLUG_MQTT_CAOPEN,
			DLG_PLUG_MQTT_PREFIX,
			DLG_PLUG_MQTT_LOGLEVEL
		};

		static inline const int32_t get_log_level__(const int32_t n) {
			switch (n) {
				case 0: return 0;
				case 1: return (1 << 0);
				case 2: return (1 << 1);
				case 3: return (1 << 2);
				case 4: return (1 << 3);
				case 5: return (1 << 4);
				case -1:
				default: return CB_ERR;
			}
		}

		SmartHomeSetupDialog::SmartHomeSetupDialog(IO::PluginCb& cb) : cb_(cb) {
		}
		SmartHomeSetupDialog::~SmartHomeSetupDialog() {
			dispose_();
		}

		void SmartHomeSetupDialog::dispose_() {
			isload_ = false;
			hwnd_.reset();
		}
		void SmartHomeSetupDialog::init_() {
			try {
				isload_ = false;
				auto& mmt = common_config::Get().GetConfig();
				auto ft = std::async(std::launch::async, [=]() ->bool {
					config_.Copy(mmt->mqttconf);
					return true;
				});
				const bool _ = ft.get();

				::SetDlgItemTextW(hwnd_, DLG_PLUG_MQTT_PSK, config_.sslpsk.c_str());
				::SetDlgItemTextW(hwnd_, DLG_PLUG_MQTT_PORT, std::to_wstring(config_.port).c_str());
				::SetDlgItemTextW(hwnd_, DLG_PLUG_MQTT_PASS, config_.password.c_str());
				::SetDlgItemTextW(hwnd_, DLG_PLUG_MQTT_LOGIN, config_.login.c_str());
				::SetDlgItemTextW(hwnd_, DLG_PLUG_MQTT_IPADDR, config_.host.c_str());
				::SetDlgItemTextW(hwnd_, DLG_PLUG_MQTT_PREFIX, config_.mqttprefix.c_str());

				std::filesystem::path p(config_.certcapath);
				if (std::filesystem::exists(p))
					::SetDlgItemTextW(hwnd_, DLG_PLUG_MQTT_CA, p.stem().wstring().c_str());

				::CheckDlgButton(hwnd_, DLG_PLUG_MQTT_ISSSL, CHECKBTN(config_.isssl));
				::CheckDlgButton(hwnd_, DLG_PLUG_MQTT_ISENABLE, CHECKBTN(config_.enable));
				::CheckDlgButton(hwnd_, DLG_PLUG_MQTT_ISSELFSIGN, CHECKBTN(config_.isselfsigned));

				buildLogLevelComboBox_(config_.loglevel);

				isload_ = true;

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		IO::PluginUi* SmartHomeSetupDialog::GetPluginUi() {
			return static_cast<IO::PluginUi*>(this);
		}

		#pragma region Override
		LRESULT SmartHomeSetupDialog::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
			try {
				switch (m) {
					case WM_INITDIALOG: {
						hwnd_.reset(h, static_cast<SUBCLASSPROC>(&PluginUi::DialogProc_), reinterpret_cast<DWORD_PTR>(this), 0);
						init_();
						return static_cast<INT_PTR>(1);
					}
					case WM_COMMAND: {
						if (!isload_) break;
						uint16_t c{ LOWORD(w) };
						switch (c) {
							case DLG_PLUG_SAVE: {
								changeOnSaveConfig_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_MIDI_ISENABLE: {
								changeOnEnable_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_MQTT_LOGLEVEL: {
								if (HIWORD(w) == CBN_SELENDOK) /* CBN_SELCHANGE */
									changeOnLogLevel_();
								break;
							}
							case DLG_PLUG_MQTT_PSK:
							case DLG_PLUG_MQTT_PORT:
							case DLG_PLUG_MQTT_PASS:
							case DLG_PLUG_MQTT_LOGIN:
							case DLG_PLUG_MQTT_IPADDR:
							case DLG_PLUG_MQTT_PREFIX: {
								if (!isload_ || (HIWORD(w) != EN_CHANGE)) break;
								switch (c) {
									case DLG_PLUG_MQTT_PSK: {
										changeOnPsk_();
										break;
									}
									case DLG_PLUG_MQTT_PORT: {
										changeOnPort_();
										break;
									}
									case DLG_PLUG_MQTT_PASS: {
										changeOnPass_();
										break;
									}
									case DLG_PLUG_MQTT_LOGIN: {
										changeOnLogin_();
										break;
									}
									case DLG_PLUG_MQTT_IPADDR: {
										changeOnIpAddress_();
										break;
									}
									case DLG_PLUG_MQTT_PREFIX: {
										changeOnPrefix_();
										break;
									}
									default: break;
								}
								break;
							}
							case DLG_PLUG_MQTT_CAOPEN: {
								changeOnCa_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_MQTT_ISSSL: {
								changeOnSsl_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_MQTT_ISSELFSIGN: {
								changeOnSelfSign_();
								return static_cast<INT_PTR>(1);
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

		#pragma region Change On events
		void SmartHomeSetupDialog::changeOnEnable_() {
			try {
				if (!hwnd_) return;
				config_.enable = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_MQTT_ISENABLE);

				for (int i : elements_)
					UI::UiUtils::SetControlEnable(hwnd_, i, config_.enable);

				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void SmartHomeSetupDialog::changeOnSaveConfig_() {
			try {
				if (!hwnd_) return;

				auto& mmt = common_config::Get().GetConfig();
				mmt->mqttconf.Copy(config_);
				UI::UiUtils::SaveDialogEnabled(hwnd_, false);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void SmartHomeSetupDialog::changeOnLogLevel_() {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_MQTT_LOGLEVEL))) return;

				int32_t val = get_log_level__(static_cast<int32_t>(::SendMessageW(hi, CB_GETCURSEL, 0, 0)));
				if (val == CB_ERR) return;

				config_.loglevel = val;
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void SmartHomeSetupDialog::changeOnSsl_() {
			try {
				if (!hwnd_) return;

				config_.isssl = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_MQTT_ISSSL);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void SmartHomeSetupDialog::changeOnSelfSign_() {
			try {
				if (!hwnd_) return;

				config_.isselfsigned = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_MQTT_ISSELFSIGN);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void SmartHomeSetupDialog::changeOnIpAddress_() {
			try {
				if (!hwnd_) return;

				config_.host = UI::UiUtils::GetControlText(hwnd_, DLG_PLUG_MQTT_IPADDR);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void SmartHomeSetupDialog::changeOnPort_() {
			try {
				if (!hwnd_) return;

				auto txt = UI::UiUtils::GetControlText(hwnd_, DLG_PLUG_MQTT_PORT);
				if (txt.empty()) return;

				uint32_t p = std::stoul(txt);
				if (p < _UI16_MAX) {
					config_.port = p;
					UI::UiUtils::SaveDialogEnabled(hwnd_);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void SmartHomeSetupDialog::changeOnLogin_() {
			try {
				if (!hwnd_) return;

				config_.login = UI::UiUtils::GetControlText(hwnd_, DLG_PLUG_MQTT_LOGIN);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void SmartHomeSetupDialog::changeOnPass_() {
			try {
				if (!hwnd_) return;

				config_.password = UI::UiUtils::GetControlText(hwnd_, DLG_PLUG_MQTT_PASS);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void SmartHomeSetupDialog::changeOnPsk_() {
			try {
				if (!hwnd_) return;

				config_.sslpsk = UI::UiUtils::GetControlText(hwnd_, DLG_PLUG_MQTT_PSK);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void SmartHomeSetupDialog::changeOnCa_() {
			try {
				if (!hwnd_) return;

				PWSTR pws{ nullptr };
				IShellItem* item{ nullptr };
				IFileOpenDialog* ptr{ nullptr };

				try {
					std::wstring filter = common_error_code::Get().get_error(common_error_id::err_MQTT_CA_FILTER);
					COMDLG_FILTERSPEC extfilter[] = {
						{ filter.c_str(), L"*.cert;*.pem"}
					};
					do {
						HRESULT h = CoCreateInstance(
							CLSID_FileOpenDialog,
							NULL, CLSCTX_ALL,
							IID_IFileOpenDialog,
							reinterpret_cast<void**>(&ptr));

						if (h != S_OK) break;

						#pragma warning( push )
						#pragma warning( disable : 4267 )
						h = ptr->SetFileTypes(static_cast<UINT>(std::size(extfilter)), extfilter);
						#pragma warning( pop )

						if (h != S_OK) break;
						h = ptr->Show(hwnd_);
						if (h != S_OK) break;
						h = ptr->GetResult(&item);
						if (h != S_OK) break;
						h = item->GetDisplayName(SIGDN_FILESYSPATH, &pws);
						if (h != S_OK) break;

						config_.certcapath = Utils::to_string(pws);
						std::filesystem::path p(config_.certcapath);
						::SetDlgItemTextW(hwnd_, DLG_PLUG_MQTT_CA, p.stem().wstring().c_str());
						UI::UiUtils::SaveDialogEnabled(hwnd_);

					} while (0);
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				if (item) item->Release();
				if (ptr)  ptr->Release();
				if (pws)  ::CoTaskMemFree(pws);
				
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void SmartHomeSetupDialog::changeOnPrefix_() {
			try {
				if (!hwnd_) return;

				config_.mqttprefix = UI::UiUtils::GetControlText(hwnd_, DLG_PLUG_MQTT_PREFIX);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void SmartHomeSetupDialog::buildLogLevelComboBox_(const int32_t n) {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_MQTT_LOGLEVEL))) return;

				ComboBox_ResetContent(hi);

				for (int32_t i = static_cast<uint32_t>(common_error_id::err_MQTT_LOG_NONE);
							 i <= static_cast<uint32_t>(common_error_id::err_MQTT_LOG_DEBUG);
							 i++)
					ComboBox_AddString(hi, common_error_code::Get().get_error(static_cast<common_error_id>(i)).c_str());

				int32_t val;
				switch (n) {
					case 0: val = 0; break;
					case (1 << 0): val = 1; break;
					case (1 << 1): val = 2; break;
					case (1 << 2): val = 3; break;
					case (1 << 3): val = 4; break;
					case (1 << 4): val = 5; break;
					default: return;
				}
				ComboBox_SetCurSel(hi, val);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

	}
}