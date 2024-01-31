/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session values/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (MidiCtrlPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace PLUGINS {

		static const uint16_t elements_[] {
			DLG_PLUG_MIDI_LIST,
			DLG_PLUG_MIDI_LIST_IN,
			DLG_PLUG_MIDI_LIST_OUT,
			DLG_PLUG_MIDI_SLIDER1,
			DLG_PLUG_MIDI_SLIDER2,
			DLG_PLUG_MIDI_ISOUTSYSPORT,
			DLG_PLUG_MIDI_ISJOGFILTER,
			DLG_PLUG_MIDI_SLIDER1_VAL,
			DLG_PLUG_MIDI_SLIDER2_VAL
		};

		static inline const bool find_device__(std::vector<std::wstring>& v, std::wstring& s) {
			return (std::find(v.begin(), v.end(), s) != v.end());
		}
		void MidiCtrlSetupDialog::setOutPort_(HWND h, const uint32_t val) {
			config_.out_count = val;
			ComboBox_SelectString(h, 0, ((val) ? L"1" : L"0"));
			::CheckDlgButton(hwnd_, DLG_PLUG_MIDI_ISOUT, CHECKBTN((val)));
		}

		MidiCtrlSetupDialog::MidiCtrlSetupDialog(IO::PluginCb& cb, HWND mhwnd) : cb_(cb) {
			mhwnd_.reset(mhwnd);
		}
		MidiCtrlSetupDialog::~MidiCtrlSetupDialog() {
			dispose_();
		}

		void MidiCtrlSetupDialog::dispose_() {
			isload_.store(false);
			hwnd_.reset();
		}
		void MidiCtrlSetupDialog::init_() {
			try {
				isload_.store(false);
				auto& mmt = common_config::Get().GetConfig();
				auto ft = std::async(std::launch::async, [=]() ->bool {
					config_.Copy(mmt->midiconf);
					return MIDI::MidiDevices::Get().CheckVirtualDriver();
				});
				bool chkd{ false };
				try {
					chkd = ft.get();
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				if (chkd) {
					std::wstring ws = MIDI::MidiDevices::Get().GetVirtualDriverVersion();
					::CheckDlgButton(hwnd_,  DLG_PLUG_MIDI_VMDRV_CHECK, CHECKBTN(!ws.empty()));
					::SetDlgItemTextW(hwnd_, DLG_PLUG_MIDI_VMDRV_VER, ws.c_str());

					if (ws.empty())
						cb_.ToLog(log_string().to_log_string(__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_MIDI_EMPTY_DRIVER)
							).str()
						);
				} else {
					cb_.ToLog(log_string().to_log_string(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_MIDI_NOT_DRIVER)
						).str()
					);
					::CheckDlgButton(hwnd_,  DLG_PLUG_MIDI_VMDRV_CHECK, false);
					::SetDlgItemTextW(hwnd_, DLG_PLUG_MIDI_VMDRV_VER, L"-");
					::ShowWindow(::GetDlgItem(hwnd_, DLG_PLUG_MIDI_INSTALL_VMDRV), SW_SHOW);
				}

				::CheckDlgButton(hwnd_,   DLG_PLUG_MIDI_ISENABLE, CHECKBTN(config_.enable));
				::CheckDlgButton(hwnd_,   DLG_PLUG_MIDI_ISCONFIG, CHECKBTN(!config_.empty()));
				::CheckDlgButton(hwnd_,	  DLG_PLUG_MIDI_ISJOGFILTER, CHECKBTN(config_.jog_scene_filter));
				::CheckDlgButton(hwnd_,   DLG_PLUG_MIDI_ISOUTSYSPORT, CHECKBTN(config_.out_system_port));
				::CheckDlgButton(hwnd_,   DLG_PLUG_MIDI_ISUNITS, CHECKBTN(!mmt->units.empty()));
				::SetDlgItemTextW(hwnd_,  DLG_PLUG_MIDI_UNITCOUNT, std::to_wstring(mmt->units.size()).c_str());

				buildOutComboBox_(config_.out_count);
				buildProxyComboBox_(config_.proxy_count);

				const uint32_t i1 = (config_.btn_interval > 0U) ? config_.btn_interval : 50U,
							   i2 = (config_.btn_long_interval > 0U) ? config_.btn_long_interval : 500U;
				UI::UiUtils::SetSliderValues(hwnd_, DLG_PLUG_MIDI_SLIDER1, DLG_PLUG_MIDI_SLIDER1_VAL, 10U, 1000U, i1);
				UI::UiUtils::SetSliderValues(hwnd_, DLG_PLUG_MIDI_SLIDER2, DLG_PLUG_MIDI_SLIDER2_VAL, 100U, 1500U, i2);

				buildDeviceListBox_(true, true);
				isload_.store(true);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		IO::PluginUi* MidiCtrlSetupDialog::GetPluginUi() {
			return static_cast<IO::PluginUi*>(this);
		}

		#pragma region Override
		LRESULT MidiCtrlSetupDialog::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
			try {
				switch (m) {
					case WM_INITDIALOG: {
						hwnd_.reset(h, static_cast<SUBCLASSPROC>(&PluginUi::DialogProc_), reinterpret_cast<DWORD_PTR>(this), 0);
						init_();
						return static_cast<INT_PTR>(1);
					}
					case WM_HSCROLL: {

						if (!isload_) break;

						switch (LOWORD(w)) {
							case SB_LINELEFT:
							case SB_LINERIGHT:
							case SB_THUMBPOSITION: {
								changeOnSliders_();
								return static_cast<INT_PTR>(1);
							}
						}
						break;
					}
					case WM_NOTIFY: {

						if (!isload_ || (!l)) break;

						LPNMHDR lpmh = (LPNMHDR)l;
						switch (lpmh->idFrom) {
							case (UINT)DLG_PLUG_MIDI_LIST: {
								#pragma warning( push )
								#pragma warning( disable : 26454 )
								if (lpmh->code == static_cast<UINT>(LVN_ITEMCHANGED))
									changeOnListViewCheck_();
								#pragma warning( pop )
								break;
							}
							default: break;
						}
						break;
					}
					case WM_HELP: {
						if (!l || !mhwnd_) break;
						return ::SendMessageW(mhwnd_, m, DLG_PLUG_MIDI_WINDOW, l);
					}
					case WM_COMMAND: {
						if (!isload_) break;
						uint16_t c{ LOWORD(w) };
						switch (c) {
							case DLG_PLUG_SAVE: {
								changeOnSaveConfig_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_MIDI_LIST_IN: {
								buildDeviceListBox_(true);
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_MIDI_LIST_OUT: {
								buildDeviceListBox_(false);
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_MIDI_ISENABLE: {
								changeOnEnable_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_MIDI_ISJOGFILTER: {
								changeOnJogfilter_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_MIDI_ISOUTSYSPORT: {
								changeOnSystemPort_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_MIDI_OUT_COMBO: {
								if (HIWORD(w) == CBN_SELENDOK) /* CBN_SELCHANGE */
									changeOnOut_();
								break;
							}
							case DLG_PLUG_MIDI_PROXY_COMBO: {
								if (HIWORD(w) == CBN_SELENDOK) /* CBN_SELCHANGE */
									changeOnProxy_();
								break;
							}
							case DLG_PLUG_MIDI_INSTALL_VMDRV: {
								if (mhwnd_) {
									HELPINFO hi{};
									hi.cbSize = sizeof(hi);
									hi.dwContextId = DLG_PLUG_MIDI_WINDOW;
									hi.iCtrlId = DLG_PLUG_MIDI_INSTALL_VMDRV;
									::SendMessageW(mhwnd_, m, DLG_PLUG_MIDI_WINDOW, reinterpret_cast<LPARAM>(&hi));
									/* virtual drivers install -> Action */
								}
								dispose_();
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
		void MidiCtrlSetupDialog::changeOnEnable_() {
			try {
				if (!hwnd_) return;
				config_.enable = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_MIDI_ISENABLE);

				for (int i : elements_)
					UI::UiUtils::SetControlEnable(hwnd_, i, config_.enable);

				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void MidiCtrlSetupDialog::changeOnSaveConfig_() {
			try {
				if (!hwnd_) return;

				/* Logic corrector */
				config_.out_system_port = (config_.out_system_port && !config_.midi_out_devices.empty());
				config_.out_count = (config_.out_system_port) ? 0U : config_.out_count;
				if (!config_.out_system_port && !config_.midi_out_devices.empty())
					config_.midi_out_devices.clear();

				auto& mmt = common_config::Get().GetConfig();
				mmt->midiconf.Copy(config_);
				UI::UiUtils::SaveDialogEnabled(hwnd_, false);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void MidiCtrlSetupDialog::changeOnJogfilter_() {
			try {
				if (!hwnd_) return;
				config_.jog_scene_filter = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_MIDI_ISJOGFILTER);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void MidiCtrlSetupDialog::changeOnSystemPort_() {
			try {
				if (!hwnd_) return;
				config_.out_system_port = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_MIDI_ISOUTSYSPORT);
				HWND hi = ::GetDlgItem(hwnd_, DLG_PLUG_MIDI_OUT_COMBO);

				if (!config_.out_system_port) {
					if (!config_.midi_out_devices.empty())
						config_.midi_out_devices.clear();
					if (hi) setOutPort_(hi, 1U);
				} else if (hi) setOutPort_(hi, 0U);

				if (UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_MIDI_LIST_OUT)) {
					if ((hi = ::GetDlgItem(hwnd_, DLG_PLUG_MIDI_LIST))) {
						if (!config_.out_system_port) {
							const uint16_t x = ListView_GetItemCount(hi);
							for (uint16_t i = 0; i < x; i++) {
								if (ListView_GetCheckState(hi, i))
									ListView_SetCheckState(hi, i, false);
							}
						}
						::EnableWindow(hi, config_.out_system_port);
					}
				}
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void MidiCtrlSetupDialog::changeOnSliders_() {
			try {
				if (!hwnd_) return;
				bool ischange{ false };

				DWORD pos1 = UI::UiUtils::GetSliderValue(hwnd_, DLG_PLUG_MIDI_SLIDER1);
				if (config_.btn_interval != pos1) {
					if (pos1 <= 1000) {
						config_.btn_interval = pos1;
						UI::UiUtils::SetSliderInfo(hwnd_, DLG_PLUG_MIDI_SLIDER1_VAL, pos1);
						ischange = true;
					} else {
						pos1 = config_.btn_interval;
						UI::UiUtils::SetSliderValues(hwnd_, DLG_PLUG_MIDI_SLIDER1, DLG_PLUG_MIDI_SLIDER1_VAL, 10, 1000, pos1);
					}
				}

				DWORD pos2 = UI::UiUtils::GetSliderValue(hwnd_, DLG_PLUG_MIDI_SLIDER2);
				if (config_.btn_long_interval != pos2) {
					if (pos2 <= 1500U) {
						bool b = (pos2 > pos1);
						pos2 = b ? pos2 : (pos1 + 150U);
						config_.btn_long_interval = pos2;
						if (!b) UI::UiUtils::SetSliderValues(hwnd_, DLG_PLUG_MIDI_SLIDER2, DLG_PLUG_MIDI_SLIDER2_VAL, 100U, 1500U, pos2);
						else    UI::UiUtils::SetSliderInfo(hwnd_, DLG_PLUG_MIDI_SLIDER2_VAL, pos2);
						ischange = true;
					}
				}
				if (ischange) UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void MidiCtrlSetupDialog::changeOnProxy_() {
			try {
				if (!hwnd_) return;

				if (HWND hi; (hi = ::GetDlgItem(hwnd_, DLG_PLUG_MIDI_PROXY_COMBO))) {
					int32_t idx = static_cast<int32_t>(::SendMessageW(hi, CB_GETCURSEL, 0, 0));
					if (idx == CB_ERR) return;
					config_.proxy_count = idx;
					::CheckDlgButton(hwnd_, DLG_PLUG_MIDI_ISPROXY, CHECKBTN(idx > 0));
					if (idx <= 0)
						cb_.ToLog(log_string().to_log_string(__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_MIDI_PROXY_OFF)
							).str()
						);
					else
						cb_.ToLog(log_string().to_log_format(__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_MIDI_PROXY_PORTS),
							idx).str()
						);
				} else {
					config_.proxy_count = 0U;
					::CheckDlgButton(hwnd_, DLG_PLUG_MIDI_ISPROXY, CHECKBTN(false));
					cb_.ToLog(log_string().to_log_string(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_UNDEFINED)
						).str()
					);
				}
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void MidiCtrlSetupDialog::changeOnOut_() {
			try {
				if (!hwnd_) return;

				if (HWND hi; (hi = ::GetDlgItem(hwnd_, DLG_PLUG_MIDI_OUT_COMBO))) {
					int32_t idx = static_cast<int32_t>(::SendMessageW(hi, CB_GETCURSEL, 0, 0));
					if (idx == CB_ERR) return;
					config_.out_count = idx;
					::CheckDlgButton(hwnd_, DLG_PLUG_MIDI_ISOUT, CHECKBTN(idx > 0));
					if (idx <= 0)
						cb_.ToLog(log_string().to_log_string(__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_MIDI_OUT_OFF)
							).str()
						);
					else
						cb_.ToLog(log_string().to_log_format(__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_MIDI_OUT_PORTS),
							idx).str()
						);
				} else {
					config_.out_count = 0U;
					::CheckDlgButton(hwnd_, DLG_PLUG_MIDI_ISOUT, CHECKBTN(false));
					cb_.ToLog(log_string().to_log_string(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_UNDEFINED)
						).str()
					);
				}
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void MidiCtrlSetupDialog::changeOnListViewCheck_() {
			try {
				if (!hwnd_) return;

				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_MIDI_LIST))) return;

				bool sel = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_MIDI_LIST_IN);
				const uint16_t x = ListView_GetItemCount(hi);

				if (x) {
					if (sel) config_.midi_in_devices.clear();
					else     config_.midi_out_devices.clear();

					for (uint16_t i = 0U; i < x; i++) {
						if (ListView_GetCheckState(hi, i)) {
							std::wstring buf(201, L'\0');
							ListView_GetItemText(hi, i, 0, buf.data(), 200);
							if (buf.at(0) != L'\0') {
								std::wstring dev(buf.c_str());
								if (sel) config_.midi_in_devices.push_back(dev);
								else     config_.midi_out_devices.push_back(dev);
							}
						}
					}
					UI::UiUtils::SaveDialogEnabled(hwnd_);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region Builds events
		void MidiCtrlSetupDialog::buildDeviceListBox_(const bool sel, bool isinit) {
			bool oldload = isload_.load();
			try {
				if (!hwnd_) return;

				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_MIDI_LIST))) return;
				isload_.store(false);

				if (isinit)
					(void) ListView_SetExtendedListViewStyle(hi,
						ListView_GetExtendedListViewStyle(hi)
						| LVS_EX_FULLROWSELECT
						| LVS_EX_AUTOSIZECOLUMNS
						| LVS_EX_CHECKBOXES
						| LVS_EX_SINGLEROW
						| LVS_EX_DOUBLEBUFFER
						| LVS_EX_FLATSB
					);
				else (void) ListView_DeleteAllItems(hi);

				::CheckRadioButton(hwnd_, DLG_PLUG_MIDI_LIST_IN, DLG_PLUG_MIDI_LIST_OUT, CHECKRADIO(sel, DLG_PLUG_MIDI_LIST_IN, DLG_PLUG_MIDI_LIST_OUT));
				auto& list = sel ? MIDI::MidiDevices::Get().GetMidiInDeviceList() : MIDI::MidiDevices::Get().GetMidiOutDeviceList();

				if (!sel && !config_.out_system_port && !config_.midi_out_devices.empty())
					config_.midi_out_devices.clear();

				for (auto& s : list) {
					LVITEMW lvi{};
					lvi.mask = LVIF_TEXT | LVIF_STATE;
					lvi.pszText = (LPWSTR)s.c_str();

					int idx;
					if ((idx = ListView_InsertItem(hi, &lvi)) == -1) break;
					if (find_device__(sel ? config_.midi_in_devices : config_.midi_out_devices, s))
						ListView_SetCheckState(hi, idx, true);
				}
				::EnableWindow(hi, (sel || (!sel && config_.out_system_port)));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			isload_.store(oldload);
		}
		void MidiCtrlSetupDialog::buildProxyComboBox_(const uint32_t n) {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_MIDI_PROXY_COMBO))) return;

				ComboBox_ResetContent(hi);

				for (uint32_t i = 0, z = (n >= 5) ? (n + 1) : 5; i < z; i++)
					ComboBox_AddString(hi, std::to_wstring(i).c_str());
				ComboBox_SelectString(hi, 0, std::to_wstring(n).c_str());
				::CheckDlgButton(hwnd_, DLG_PLUG_MIDI_ISPROXY, CHECKBTN(n > 0U));
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void MidiCtrlSetupDialog::buildOutComboBox_(const uint32_t n) {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_MIDI_OUT_COMBO))) return;

				ComboBox_ResetContent(hi);

				for (uint32_t i = 0; i < 4; i++)
					ComboBox_AddString(hi, std::to_wstring(i).c_str());
				ComboBox_SelectString(hi, 0, std::to_wstring(n).c_str());
				::CheckDlgButton(hwnd_, DLG_PLUG_MIDI_ISOUT, CHECKBTN(n > 0U));
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

	}
}