/*
	MIDI-MT is a high level application driver for USB MIDI control surface.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace PLUGINS {

		using namespace std::placeholders;

		VmScriptSetupDialog::VmScriptSetupDialog(IO::PluginCb& cb, std::shared_ptr<SCRIPT::VmScripts>& vms, HWND mhwnd)
			: cb_(cb), vmscripts_(vms), id_(Utils::random_hash(this)) {

			mhwnd_.reset(mhwnd);
		}
		VmScriptSetupDialog::~VmScriptSetupDialog() {
			dispose_();
		}

		void VmScriptSetupDialog::dispose_() {
			vmscripts_->event_list_remove(id_);
			isload_.store(false);
			hwnd_.reset();
		}
		void VmScriptSetupDialog::init_() {
			try {
				isload_.store(false);
				auto f = std::async(std::launch::async, [=]() ->bool {
					auto& mmt = common_config::Get().GetConfig();
					awaiter::lock(build_list_);
					config_.copy(mmt->vmscript);
					awaiter::unlock(build_list_);
					return true;
				});
				const bool _ = f.get();

				::CheckDlgButton(hwnd_,  DLG_PLUG_VMSCRIPT_ISENABLE,CHECKBTN(config_.enable));
				::CheckDlgButton(hwnd_,  DLG_PLUG_VMSCRIPT_WATCH,	CHECKBTN(config_.script_watch));
				::CheckDlgButton(hwnd_,  DLG_PLUG_VMSCRIPT_DEBUG,	CHECKBTN(config_.script_debug));
				::CheckDlgButton(hwnd_,  DLG_PLUG_VMSCRIPT_LIBMATH,	CHECKBTN(!config_.lib_match));
				::CheckDlgButton(hwnd_,  DLG_PLUG_VMSCRIPT_LIBSTR,	CHECKBTN(!config_.lib_string));
				::CheckDlgButton(hwnd_,  DLG_PLUG_VMSCRIPT_LIBWSTR,	CHECKBTN(!config_.lib_wstring));

				::SetDlgItemTextW(hwnd_, DLG_PLUG_VMSCRIPT_DIR, config_.script_directory.c_str());

				isload_.store(true);

				vmscripts_->event_list_add(
					std::bind(static_cast<void(VmScriptSetupDialog::*)(SCRIPT::scripts_list_t&)>(&VmScriptSetupDialog::build_script_list_), this, _1),
					id_
				);
				vmscripts_->event_list();
				::SetFocus(hwnd_);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		IO::PluginUi* VmScriptSetupDialog::GetPluginUi() {
			return static_cast<IO::PluginUi*>(this);
		}

		#pragma region Builds events
		void VmScriptSetupDialog::build_script_list_(SCRIPT::scripts_list_t& list) {
			try {
				if (!hwnd_) return;
				awaiter lock(std::ref(build_list_));
				if (!lock.lock_if()) return;

				std::vector<std::wstring> v{};

				for (auto& a : list)
					v.push_back((log_string()
						<< L"  " << Utils::to_string(a.name)
						<< (log_string() << L",    size: " << a.body.size() << L" bytes").str()
						<< L",    modify: " << Utils::NANOSECONDS_to_string(a.time)));

				script_list_ = v;

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}

			try {
				if (hwnd_)
					::PostMessageW(hwnd_, WM_COMMAND, DLG_PLUG_VMSCRIPT_LIST_CHANGE, 0U);
			} catch (...) {}
		}
		#pragma endregion

		#pragma region Change On events
		void VmScriptSetupDialog::changeOnEnable_() {
			try {
				if (!hwnd_) return;
				config_.enable = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_VMSCRIPT_ISENABLE);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScriptSetupDialog::changeOnSelectDirectory_() {
			try {
				if (!hwnd_) return;
			
				COMDLG_FILTERSPEC filter[] = {
					{
						common_error_code::Get().get_error(common_error_id::err_SCRIPT_DIRFILTER).c_str(),
						vmscripts_->get_script_extension().c_str()
					}
				};
				std::wstring s = UI::UiUtils::OpenFileDialog(hwnd_, filter);
				if (!s.empty()) {
					std::filesystem::path p(s);
					config_.script_directory = p.parent_path().wstring();
					::SetDlgItemTextW(hwnd_, DLG_PLUG_VMSCRIPT_DIR, config_.script_directory.c_str());
					UI::UiUtils::SaveDialogEnabled(hwnd_);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScriptSetupDialog::changeOnWatchDirectory_() {
			try {
				if (!hwnd_) return;
				config_.script_watch = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_VMSCRIPT_WATCH);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScriptSetupDialog::changeOnDebugging_() {
			try {
				if (!hwnd_) return;
				config_.script_debug = UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_VMSCRIPT_DEBUG);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScriptSetupDialog::changeOnLibMath_() {
			try {
				if (!hwnd_) return;
				config_.lib_match = !UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_VMSCRIPT_LIBMATH);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScriptSetupDialog::changeOnLibString_() {
			try {
				if (!hwnd_) return;
				config_.lib_string = !UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_VMSCRIPT_LIBSTR);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScriptSetupDialog::changeOnLibWstring_() {
			try {
				if (!hwnd_) return;
				config_.lib_wstring = !UI::UiUtils::GetControlChecked(hwnd_, DLG_PLUG_VMSCRIPT_LIBWSTR);
				UI::UiUtils::SaveDialogEnabled(hwnd_);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScriptSetupDialog::changeOnScriptList_() {
			try {
				if (!hwnd_ || !isload_.load(std::memory_order_acquire)) return;
				awaiter lock(std::ref(build_list_));
				if (!lock.lock_if()) return;

				HWND hi;
				if (!(hi = ::GetDlgItem(hwnd_, DLG_PLUG_VMSCRIPT_LIST))) return;

				(void)ListBox_ResetContent(hi);

				for (auto& a : script_list_)
					(void) ::SendMessageW(hi, LB_ADDSTRING, 0, (LPARAM)a.c_str());

				::SetDlgItemTextW(hwnd_, DLG_PLUG_VMSCRIPT_TOTAL, std::to_wstring(script_list_.size()).c_str());

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void VmScriptSetupDialog::changeOnSaveConfig_() {
			try {
				if (!hwnd_) return;

				auto& mmt = common_config::Get().GetConfig();
				mmt->vmscript.copy(config_);
				UI::UiUtils::SaveDialogEnabled(hwnd_, false);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region Override
		LRESULT VmScriptSetupDialog::CommandDialog(HWND h, UINT m, WPARAM w, LPARAM l) {
			try {
				switch (m) {
					case WM_INITDIALOG: {
						hwnd_.reset(h, static_cast<SUBCLASSPROC>(&PluginUi::DialogProc_), reinterpret_cast<DWORD_PTR>(this), 0);
						init_();
						return static_cast<INT_PTR>(1);
					}
					case WM_HELP: {
						if (!l || !mhwnd_) break;
						return ::SendMessageW(mhwnd_, m, DLG_PLUG_VMSCRIPT_WINDOW, l);
					}
					case WM_COMMAND: {
						if (!isload_) break;
						uint16_t c{ LOWORD(w) };
						switch (c) {
							case DLG_PLUG_SAVE: {
								changeOnSaveConfig_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_VMSCRIPT_ISENABLE: {
								changeOnEnable_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_VMSCRIPT_DIR_OPEN: {
								changeOnSelectDirectory_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_VMSCRIPT_WATCH: {
								changeOnWatchDirectory_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_VMSCRIPT_DEBUG: {
								changeOnDebugging_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_VMSCRIPT_LIBMATH: {
								changeOnLibMath_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_VMSCRIPT_LIBSTR: {
								changeOnLibString_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_VMSCRIPT_LIBWSTR: {
								changeOnLibWstring_();
								return static_cast<INT_PTR>(1);
							}
							case DLG_PLUG_VMSCRIPT_LIST_CHANGE: {
								changeOnScriptList_();
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

	}
}