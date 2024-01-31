/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session values/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (LightKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace PLUGINS {

		static const uint16_t elements_dmx_[] {
			DLG_PLUG_LIGHT_DMX_COMBO,
			DLG_PLUG_LIGHT_DMX_COMNAME,
			DLG_PLUG_LIGHT_DMX_COMPORT,
			DLG_PLUG_LIGHT_DMX_COMBAUDRATE,
			DLG_PLUG_LIGHT_DMX_COMSTOPBITS,
			DLG_PLUG_LIGHT_DMX_COMTIMEOUT
		};
		static const uint16_t elements_artnet_[]{
			DLG_PLUG_LIGHT_ARTNET_COMBO,
			DLG_PLUG_LIGHT_ARTNET_UNIVERSE,
			DLG_PLUG_LIGHT_ARTNET_PORT,
			DLG_PLUG_LIGHT_ARTNET_IP,
			DLG_PLUG_LIGHT_ARTNET_MASK,
			DLG_PLUG_LIGHT_ARTNET_BCAST
		};

		static inline const bool find_device__(std::vector<std::wstring>& v, std::wstring& s) {
			return (std::find(v.begin(), v.end(), s) != v.end());
		}

		LightsSetupDialog::LightsSetupDialog(IO::PluginCb& cb, std::function<LIGHT::SerialPortConfigs& ()> f1, std::function<LIGHT::ArtnetConfigs& ()> f2, HWND mhwnd)
			: cb_(cb), get_serial_devices_(f1), get_network_interfaces_(f2) {
			mhwnd_.reset(mhwnd);
		}
		LightsSetupDialog::~LightsSetupDialog() {
			dispose_();
		}

		void LightsSetupDialog::dispose_() {
			isload_.store(false);
			hwnd_.reset();
		}
		void LightsSetupDialog::init_() {
			try {
				isload_.store(false);
				auto& mmt = common_config::Get().GetConfig();
				config_.Copy(mmt->lightconf);
				::CheckDlgButton(hwnd_, DLG_PLUG_LIGHT_POLL, CHECKBTN(config_.ispool));

				buildDmxDevicesComboBox(config_.dmxconf.port);
				buildArtnetInterfacesComboBox(config_.artnetconf.broadcast);

				showDmxConfig_();
				showArtnetConfig_();

				isload_.store(true);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		IO::PluginUi* LightsSetupDialog::GetPluginUi() {
			return static_cast<IO::PluginUi*>(this);
		}

		#pragma region Override
		LRESULT LightsSetupDialog::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
			try {
				switch (m) {
					case WM_INITDIALOG: {
						hwnd_.reset(h, static_cast<SUBCLASSPROC>(&PluginUi::DialogProc_), reinterpret_cast<DWORD_PTR>(this), 0);
						init_();
						return static_cast<INT_PTR>(1);
					}
					case WM_HELP: {
						if (!l || !mhwnd_) break;
						return ::SendMessageW(mhwnd_, m, DLG_PLUG_LIGHT_WINDOW, l);
					}
					case WM_COMMAND: {
						if (!isload_) break;
						uint16_t c{ LOWORD(w) };
						switch (c) {
							case DLG_PLUG_SAVE: {
								changeOnSaveConfig_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_LIGHT_DMX_ISENABLE: {
								changeOnDmxEnable_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_LIGHT_ARTNET_ISENABLE: {
								changeOnArtnetEnable_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_LIGHT_POLL: {
								changeOnDmxPool_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_LIGHT_DMX_COMBO: {
								if (HIWORD(w) == CBN_SELENDOK) /* CBN_SELCHANGE */
									changeOnDmxDevice_();
								break;
							}
							case DLG_PLUG_LIGHT_ARTNET_COMBO: {
								if (HIWORD(w) == CBN_SELENDOK) /* CBN_SELCHANGE */
									changeOnArtnetNetwork_();
								break;
							}
							case DLG_PLUG_LIGHT_ARTNET_PORT:
							case DLG_PLUG_LIGHT_ARTNET_UNIVERSE: {
								if (!isload_ || (HIWORD(w) != EN_CHANGE)) break;
								switch (c) {
									case DLG_PLUG_LIGHT_ARTNET_PORT: {
										changeOnArtnetPort_();
										break;
									}
									case DLG_PLUG_LIGHT_ARTNET_UNIVERSE: {
										changeOnArtnetUniverse_();
										break;
									}
									default: break;
								}
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

		#pragma region Change On events
		void LightsSetupDialog::clearDmxConfig_() {
			try {
				if (!hwnd_) return;

				for (uint16_t i = 1U; i < std::size(elements_dmx_); i++)
					::SetDlgItemTextW(hwnd_, elements_dmx_[i], L"-");

				config_.dmxconf = LIGHT::SerialPortConfig();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void LightsSetupDialog::clearArtnetConfig_() {
			try {
				if (!hwnd_) return;

				for (uint16_t i = 1U; i < std::size(elements_artnet_); i++)
					::SetDlgItemTextW(hwnd_, elements_artnet_[i], L"-");

				config_.artnetconf = LIGHT::ArtnetConfig();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void LightsSetupDialog::showDmxConfig_() {
			bool valid = !config_.dmxconf.empty();
			::CheckDlgButton(hwnd_, DLG_PLUG_LIGHT_DMX_ISENABLE, CHECKBTN(valid && config_.dmxconf.enable));

			::SetDlgItemTextW(hwnd_, DLG_PLUG_LIGHT_DMX_COMNAME,
				(!valid || config_.dmxconf.name.empty()) ? L"-" : config_.dmxconf.name.c_str()
			);
			::SetDlgItemTextW(hwnd_, DLG_PLUG_LIGHT_DMX_COMBAUDRATE,
				valid ? std::to_wstring(config_.dmxconf.baudrate).c_str() : L"-"
			);
			::SetDlgItemTextW(hwnd_, DLG_PLUG_LIGHT_DMX_COMSTOPBITS,
				valid ? std::to_wstring(config_.dmxconf.stop_bits).c_str() : L"-"
			);
			::SetDlgItemTextW(hwnd_, DLG_PLUG_LIGHT_DMX_COMTIMEOUT,
				valid ? std::to_wstring(config_.dmxconf.timeout).c_str() : L"-"
			);
			if (valid && config_.dmxconf.port > 0) {
				log_string ls;
				ls << L"COM" << config_.dmxconf.port;
				::SetDlgItemTextW(hwnd_, DLG_PLUG_LIGHT_DMX_COMPORT, ls.str().c_str());
			} else ::SetDlgItemTextW(hwnd_, DLG_PLUG_LIGHT_DMX_COMPORT, L"-");
		}
		void LightsSetupDialog::showArtnetConfig_() {
			bool valid = !config_.artnetconf.empty();
			::CheckDlgButton(hwnd_, DLG_PLUG_LIGHT_ARTNET_ISENABLE, CHECKBTN(valid && config_.artnetconf.enable));

			::SetDlgItemTextW(hwnd_, DLG_PLUG_LIGHT_ARTNET_PORT,
				valid ? std::to_wstring(config_.artnetconf.port).c_str() : L"-"
			);
			::SetDlgItemTextW(hwnd_, DLG_PLUG_LIGHT_ARTNET_UNIVERSE,
				valid ? std::to_wstring(config_.artnetconf.universe).c_str() : L"-"
			);
			::SetDlgItemTextW(hwnd_, DLG_PLUG_LIGHT_ARTNET_IP,
				(!valid || config_.artnetconf.ip.empty()) ? L"-" : config_.artnetconf.ip.c_str()
			);
			::SetDlgItemTextW(hwnd_, DLG_PLUG_LIGHT_ARTNET_MASK,
				(!valid || config_.artnetconf.mask.empty()) ? L"-" : config_.artnetconf.mask.c_str()
			);
			::SetDlgItemTextW(hwnd_, DLG_PLUG_LIGHT_ARTNET_BCAST,
				(!valid || config_.artnetconf.broadcast.empty()) ? L"-" : config_.artnetconf.broadcast.c_str()
			);
		}
		
		void LightsSetupDialog::changeOnSaveConfig_() {
			try {
				if (!hwnd_) return;

				auto& mmt = common_config::Get().GetConfig();
				mmt->lightconf.Copy(config_);
				UI::UiUtils::SaveDialogEnabled(hwnd_, false);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void LightsSetupDialog::changeOnDmxPool_() {
			try {
				if (!hwnd_) return;
				config_.ispool = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_LIGHT_POLL);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void LightsSetupDialog::changeOnDmxEnable_() {
			try {
				if (!hwnd_) return;
				config_.dmxconf.enable = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_LIGHT_DMX_ISENABLE);

				for (int i : elements_dmx_)
					UI::UiUtils::SetControlEnable(hwnd_, i, config_.dmxconf.enable);

				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void LightsSetupDialog::changeOnDmxDevice_() {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_LIGHT_DMX_COMBO))) return;

				int32_t val = static_cast<int32_t>(::SendMessageW(hi, CB_GETCURSEL, 0, 0));
				if (val == CB_ERR) return;
				if (val == 0) {
					clearDmxConfig_();
					return;
				}

				auto f = std::async(std::launch::async, [=](int32_t idx) -> bool {
					try {
						LIGHT::SerialPortConfig& cfg = get_serial_devices_().get(idx);
						if (!cfg.empty()) config_.dmxconf.Copy(cfg);
						return !cfg.empty();
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
					return false;
				}, (val - 1));

				try {
					const bool b = f.get();
					if (!b) {
						clearDmxConfig_();
						return;
					}
					showDmxConfig_();
					UI::UiUtils::SaveDialogEnabled(hwnd_);
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void LightsSetupDialog::changeOnArtnetEnable_() {
			try {
				if (!hwnd_) return;
				config_.artnetconf.enable = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_LIGHT_ARTNET_ISENABLE);

				for (int i : elements_artnet_)
					UI::UiUtils::SetControlEnable(hwnd_, i, config_.artnetconf.enable);

				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void LightsSetupDialog::changeOnArtnetNetwork_() {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_LIGHT_ARTNET_COMBO))) return;

				int32_t val = static_cast<int32_t>(::SendMessageW(hi, CB_GETCURSEL, 0, 0));
				if (val == CB_ERR) return;
				if (val == 0) {
					clearArtnetConfig_();
					return;
				}

				auto f = std::async(std::launch::async, [=](int32_t idx) -> bool {
					try {
						LIGHT::ArtnetConfig& cfg = get_network_interfaces_().get(idx);
						if (!cfg.empty()) config_.artnetconf.Copy(cfg);
						return !cfg.empty();
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
					return false;
				}, (val - 1));

				try {
					const bool b = f.get();
					if (!b) {
						clearArtnetConfig_();
						return;
					}
					showArtnetConfig_();
					UI::UiUtils::SaveDialogEnabled(hwnd_);
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void LightsSetupDialog::changeOnArtnetPort_() {
			try {
				if (!hwnd_) return;

				auto port = UI::UiUtils::GetControlText(hwnd_, DLG_PLUG_LIGHT_ARTNET_PORT);
				if (port.empty() || !std::all_of(port.begin(), port.end(), ::iswdigit)) return;
				config_.artnetconf.port = std::stoul(port);

				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void LightsSetupDialog::changeOnArtnetUniverse_() {
			try {
				if (!hwnd_) return;

				auto unv = UI::UiUtils::GetControlText(hwnd_, DLG_PLUG_LIGHT_ARTNET_UNIVERSE);
				if (unv.empty() || !std::all_of(unv.begin(), unv.end(), ::iswdigit)) return;
				config_.artnetconf.universe = std::stoul(unv);

				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region Builds events
		void LightsSetupDialog::buildDmxDevicesComboBox(const int32_t n) {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_LIGHT_DMX_COMBO))) return;

				ComboBox_ResetContent(hi);

				LIGHT::SerialPortConfigs lcfg{};
				auto f = std::async(std::launch::async, [=](LIGHT::SerialPortConfigs& lcfg_) -> LIGHT::SerialPortConfigs& {
					try {
						return get_serial_devices_();
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
					#pragma warning( push )
					#pragma warning( disable : 4172 )
					return std::ref(lcfg_);
					#pragma warning( pop )
				}, std::ref(lcfg));

				try {
					LIGHT::SerialPortConfigs& v = f.get();
					if (v.empty()) {
						ComboBox_AddString(hi, common_error_code::Get().get_error(common_error_id::err_LIGHT_DMX_NODEV).c_str());
						::CheckDlgButton(hwnd_, DLG_PLUG_LIGHT_DMX_ISENABLE, CHECKBTN(false));
						::EnableWindow(hi, false);

					} else {
						ComboBox_AddString(hi, common_error_code::Get().get_error(common_error_id::err_LIGHT_DMX_SELDEV).c_str());
						std::wstring selected{};
						for (auto& i : v.get()) {
							log_string ls;
							ls << L"COM" << i.port << L" : " << i.name.c_str();
							if ((n > 0) && (i.port == n)) {
								selected = ls.str();
								ComboBox_AddString(hi, selected.c_str());
							} else ComboBox_AddString(hi, ls.str().c_str());
						}
						if (!selected.empty()) ComboBox_SelectString(hi, 0, selected.c_str());
						else ComboBox_SetCurSel(hi, 0);
						::EnableWindow(hi, true);
					}
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void LightsSetupDialog::buildArtnetInterfacesComboBox(const std::wstring& s) {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_LIGHT_ARTNET_COMBO))) return;

				ComboBox_ResetContent(hi);

				LIGHT::ArtnetConfigs lcfg{};
				auto f = std::async(std::launch::async, [=](LIGHT::ArtnetConfigs& lcfg_) -> LIGHT::ArtnetConfigs& {
					try {
						return get_network_interfaces_();
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
					#pragma warning( push )
					#pragma warning( disable : 4172 )
					return std::ref(lcfg_);
					#pragma warning( pop )
				}, std::ref(lcfg));

				try {
					LIGHT::ArtnetConfigs& v = f.get();
					if (v.empty()) {
						ComboBox_AddString(hi, common_error_code::Get().get_error(common_error_id::err_LIGHT_ARTNET_NOINT).c_str());
						::CheckDlgButton(hwnd_, DLG_PLUG_LIGHT_ARTNET_ISENABLE, CHECKBTN(false));
						::EnableWindow(hi, false);

					} else {
						ComboBox_AddString(hi, common_error_code::Get().get_error(common_error_id::err_LIGHT_ARTNET_SELINT).c_str());
						for (auto& i : v.get())
							ComboBox_AddString(hi, i.broadcast.c_str());

						if (!s.empty()) ComboBox_SelectString(hi, 0, s.c_str());
						else ComboBox_SetCurSel(hi, 0);
						::EnableWindow(hi, true);
					}
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion
	}
}