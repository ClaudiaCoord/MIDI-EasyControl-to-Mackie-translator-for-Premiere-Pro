/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Joystick support.
	+ GamePad support.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (GamePad/Joistick Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace PLUGINS {

		static const uint16_t elements_inputs_[] {
			DLG_PLUG_GAMEPAD_SCENE,
			DLG_PLUG_GAMEPAD_ASBTN,
			DLG_PLUG_GAMEPAD_ISJOG
		};
		static const uint16_t elements_controls_[] {
			DLG_PLUG_GAMEPAD_RTYPE_COMBO
		};

		GamePadSetupDialog::GamePadSetupDialog(IO::PluginCb& cb, GAMEPAD::JoisticksBase& jb, HWND mhwnd) : cb_(cb), jb_(jb) {
			mhwnd_.reset(mhwnd);
		}
		GamePadSetupDialog::~GamePadSetupDialog() {
			dispose_();
		}

		void GamePadSetupDialog::dispose_() {
			isload_.store(false);
			hwnd_.reset();
		}
		void GamePadSetupDialog::init_() {
			try {
				isload_.store(false);
				auto& mmt = common_config::Get().GetConfig();
				config_.Copy(mmt->gamepadconf);

				::CheckDlgButton(hwnd_,  DLG_PLUG_GAMEPAD_ISENABLE, CHECKBTN(config_.enable));
				::CheckDlgButton(hwnd_,  DLG_PLUG_GAMEPAD_ASBTN, CHECKBTN(config_.directasbutton));
				::CheckDlgButton(hwnd_,  DLG_PLUG_GAMEPAD_ISJOG, CHECKBTN(config_.horizontalmode));
				::SetDlgItemTextW(hwnd_, DLG_PLUG_GAMEPAD_SCENE, std::to_wstring((uint16_t)config_.scene).c_str());
				::SetDlgItemTextW(hwnd_, DLG_PLUG_GAMEPAD_STEPINT, std::to_wstring((uint16_t)config_.step).c_str());
				::SetDlgItemTextW(hwnd_, DLG_PLUG_GAMEPAD_TOTAL, std::to_wstring(jb_.TotalDevices()).c_str());
				::SetDlgItemTextW(hwnd_, DLG_PLUG_GAMEPAD_CONNECT, std::to_wstring(jb_.OnlineDevices()).c_str());

				UI::UiUtils::SetSliderValues(hwnd_, DLG_PLUG_GAMEPAD_SLIDER1, DLG_PLUG_GAMEPAD_POLL, 10U, 1000U, config_.polling);

				buildListView_();
				buildTypesComboBox_(static_cast<int32_t>(config_.type));
				changeOnRole_(config_.rcontrols ? DLG_PLUG_GAMEPAD_RTYPE2 : DLG_PLUG_GAMEPAD_RTYPE1, false);

				isload_.store(true);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		IO::PluginUi* GamePadSetupDialog::GetPluginUi() {
			return static_cast<IO::PluginUi*>(this);
		}

		#pragma region Override
		LRESULT GamePadSetupDialog::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
			try {
				switch (m) {
					case WM_INITDIALOG: {
						hwnd_.reset(h, static_cast<SUBCLASSPROC>(&PluginUi::DialogProc_), reinterpret_cast<DWORD_PTR>(this), 0);
						init_();
						return static_cast<INT_PTR>(1);
					}
					case WM_HSCROLL: {

						if (!isload_ || !l) break;

						switch (LOWORD(w)) {
							case SB_LINELEFT:
							case SB_LINERIGHT:
							case SB_THUMBPOSITION: {
								changeOnSlider_();
								return static_cast<INT_PTR>(1);
							}
						}
						break;
					}
					case WM_HELP: {
						if (!l || !mhwnd_) break;
						return ::SendMessageW(mhwnd_, m, DLG_PLUG_GAMEPAD_WINDOW, l);
					}
					case WM_COMMAND: {
						if (!isload_) break;
						uint16_t c{ LOWORD(w) };
						switch (c) {
							case DLG_PLUG_SAVE: {
								changeOnSaveConfig_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_GAMEPAD_ISENABLE: {
								changeOnEnable_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_GAMEPAD_ASBTN: {
								changeOnAsButton_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_GAMEPAD_ISJOG: {
								changeOnHorizontalMode_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_GAMEPAD_SCENE: {
								if (!isload_ || (HIWORD(w) != EN_CHANGE)) break;
								changeOnScene_();
								break;
							}
							case DLG_PLUG_GAMEPAD_STEPINT: {
								if (!isload_ || (HIWORD(w) != EN_CHANGE)) break;
								changeOnStep_();
								break;
							}
							case DLG_PLUG_GAMEPAD_RTYPE1:
							case DLG_PLUG_GAMEPAD_RTYPE2: {
								changeOnRole_(c);
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_GAMEPAD_RTYPE_COMBO: {
								if (HIWORD(w) == CBN_SELENDOK) /* CBN_SELCHANGE */
									changeOnType_();
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

		#pragma region Builds
		void GamePadSetupDialog::buildListView_() {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_GAMEPAD_DEVICES))) return;

				(void)ListBox_ResetContent(hi);

				for (auto& a : jb_.data)
					(void) ::SendMessageW(hi, LB_ADDSTRING, 0, (LPARAM)a.label().c_str());

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void GamePadSetupDialog::buildTypesComboBox_(int32_t sel) {
			try {
				if (!hwnd_) return;
				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_GAMEPAD_RTYPE_COMBO))) return;

				(void)ComboBox_ResetContent(hi);

				for (uint16_t i = 0; i <= static_cast<uint16_t>(GAMEPAD::JoystickControlsType::CTRL_LIGHTS_AND_MQTT_AND_MIX); i++)
					(void) ComboBox_AddString(hi, GAMEPAD::JoystickHelper::GetJoystickControlsType(i).data());

				if (sel >= 0)
					(void) ComboBox_SetCurSel(hi, sel);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region Change On events
		void GamePadSetupDialog::changeOnEnable_() {
			try {
				if (!hwnd_) return;
				config_.enable = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_GAMEPAD_ISENABLE);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void GamePadSetupDialog::changeOnAsButton_() {
			try {
				if (!hwnd_) return;
				config_.directasbutton = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_GAMEPAD_ASBTN);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void GamePadSetupDialog::changeOnHorizontalMode_() {
			try {
				if (!hwnd_) return;
				config_.horizontalmode = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_GAMEPAD_ISJOG);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void GamePadSetupDialog::changeOnType_() {
			try {
				if (!hwnd_) return;

				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_GAMEPAD_RTYPE_COMBO))) return;

				int32_t val = static_cast<int32_t>(::SendMessageW(hi, CB_GETCURSEL, 0, 0));
				if (val == CB_ERR) return;

				config_.type = static_cast<GAMEPAD::JoystickControlsType>(val);
				UI::UiUtils::SaveDialogEnabled(hwnd_);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void GamePadSetupDialog::changeOnScene_() {
			try {
				if (!hwnd_) return;

				auto s = UI::UiUtils::GetControlText(hwnd_, DLG_PLUG_GAMEPAD_SCENE);
				if (s.empty()) return;

				uint32_t p = std::stoul(s);
				if (p < 255) {
					config_.scene = static_cast<uint8_t>(p);
					UI::UiUtils::SaveDialogEnabled(hwnd_);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void GamePadSetupDialog::changeOnStep_() {
			try {
				if (!hwnd_) return;

				auto s = UI::UiUtils::GetControlText(hwnd_, DLG_PLUG_GAMEPAD_STEPINT);
				if (s.empty()) return;

				uint32_t p = std::stoul(s);
				if (p < 43) {
					p = (!p) ? 1U : p;
					config_.step = static_cast<uint8_t>(p);
					UI::UiUtils::SaveDialogEnabled(hwnd_);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void GamePadSetupDialog::changeOnSlider_() {
			try {
				if (!hwnd_) return;
				bool ischange{ false };

				DWORD p = UI::UiUtils::GetSliderValue(hwnd_, DLG_PLUG_GAMEPAD_SLIDER1);
				config_.polling = ((p > 10) && (p < 1000)) ? static_cast<uint16_t>(p) : config_.polling;
				UI::UiUtils::SaveDialogEnabled(hwnd_);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void GamePadSetupDialog::changeOnRole_(uint16_t k, bool ischange) {
			try {
				if (!hwnd_) return;

				(void) ::CheckRadioButton(hwnd_, DLG_PLUG_GAMEPAD_RTYPE1, DLG_PLUG_GAMEPAD_RTYPE2, k);

				bool b = (k == DLG_PLUG_GAMEPAD_RTYPE1);
				for (uint16_t id : elements_inputs_)
					(void) ::EnableWindow(::GetDlgItem(hwnd_, id), b);
				for (uint16_t id : elements_controls_)
					(void) ::EnableWindow(::GetDlgItem(hwnd_, id), !b);

				if (ischange) {
					config_.rcontrols = !b;
					UI::UiUtils::SaveDialogEnabled(hwnd_);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void GamePadSetupDialog::changeOnSaveConfig_() {
			try {
				if (!hwnd_) return;

				auto& mmt = common_config::Get().GetConfig();
				mmt->gamepadconf.Copy(config_);
				UI::UiUtils::SaveDialogEnabled(hwnd_, false);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion
	}
}