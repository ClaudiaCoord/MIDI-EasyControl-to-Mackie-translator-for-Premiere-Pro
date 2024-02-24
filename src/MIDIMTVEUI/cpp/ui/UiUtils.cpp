/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ UI/Viewer/Editor interface.
	(c) CC 2023-2024, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include <Shlobj.h>

#include "../../Common/rc/resource_midimt.h"

namespace Common {
	namespace UI {

		using namespace std::string_view_literals;

		class UIValues {
		public:
			static const wchar_t* blanks[];

			static constexpr std::wstring_view help_fmt = L"https://claudiacoord.github.io/MIDI-MT/docs/{0}/{1}.html"sv;

			#include "..\..\Common\h\languages\help_page_index.h"
		};
		const wchar_t* UIValues::blanks[] = {L"", L"  ", L"    "};

		class AsyncExecDeleter {
		private:
			std::function<void()>* f_ = nullptr;
		public:
			AsyncExecDeleter(LPARAM l) : f_(reinterpret_cast<std::function<void()>*>(l)) {
			}
			AsyncExecDeleter(std::function<void()>* f) : f_(f) {
			}
			~AsyncExecDeleter() {
				std::function<void()>* f = f_;
				f_ = nullptr;
				if (f) delete f;
			}
			std::function<void()>* Get() {
				return f_;
			}
			const bool empty() const {
				return !f_;
			}
		};

		static std::wstring_view select_LightPage__(const uint16_t id) {
			switch (id) {
				case DLG_PLUG_LIGHT_DMX_ISENABLE:
				case DLG_PLUG_LIGHT_DMX_COMBO:
				case DLG_PLUG_LIGHT_DMX_COMNAME:
				case DLG_PLUG_LIGHT_DMX_COMPORT:
				case DLG_PLUG_LIGHT_DMX_COMBAUDRATE:
				case DLG_PLUG_LIGHT_DMX_COMSTOPBITS:
				case DLG_PLUG_LIGHT_DMX_COMTIMEOUT:
				case DLG_PLUG_LIGHT_ARTNET_ISENABLE:
				case DLG_PLUG_LIGHT_ARTNET_COMBO:
				case DLG_PLUG_LIGHT_ARTNET_UNIVERSE:
				case DLG_PLUG_LIGHT_ARTNET_PORT:
				case DLG_PLUG_LIGHT_ARTNET_IP:
				case DLG_PLUG_LIGHT_ARTNET_MASK:
				case DLG_PLUG_LIGHT_ARTNET_BCAST:
				case DLG_PLUG_LIGHT_POLL: return UIValues::page_settings_module_lights;
				default: return L""sv;
			}
		}
		static std::wstring_view select_MqttPage__(const uint16_t id) {
			switch (id) {
				case DLG_PLUG_MQTT_CA:
				case DLG_PLUG_MQTT_PSK:
				case DLG_PLUG_MQTT_PORT:
				case DLG_PLUG_MQTT_PASS:
				case DLG_PLUG_MQTT_LOGIN:
				case DLG_PLUG_MQTT_ISSSL:
				case DLG_PLUG_MQTT_IPADDR:
				case DLG_PLUG_MQTT_CAOPEN:
				case DLG_PLUG_MQTT_PREFIX:
				case DLG_PLUG_MQTT_ISENABLE:
				case DLG_PLUG_MQTT_LOGLEVEL:
				case DLG_PLUG_MQTT_ISSELFSIGN: return UIValues::page_settings_module_smart_house;
				default: return L""sv;
			}
		}
		static std::wstring_view select_MidiPage__(const uint16_t id) {
			switch (id) {
				case DLG_PLUG_MIDI_LIST:
				case DLG_PLUG_MIDI_ISOUT:
				case DLG_PLUG_MIDI_ISUNITS:
				case DLG_PLUG_MIDI_ISPROXY:
				case DLG_PLUG_MIDI_SLIDER1:
				case DLG_PLUG_MIDI_SLIDER2:
				case DLG_PLUG_MIDI_LIST_IN:
				case DLG_PLUG_MIDI_LIST_OUT:
				case DLG_PLUG_MIDI_ISENABLE:
				case DLG_PLUG_MIDI_ISCONFIG:
				case DLG_PLUG_MIDI_UNITCOUNT:
				case DLG_PLUG_MIDI_OUT_COMBO:
				case DLG_PLUG_MIDI_SLIDER1_VAL:
				case DLG_PLUG_MIDI_SLIDER2_VAL:
				case DLG_PLUG_MIDI_PROXY_COMBO:
				case DLG_PLUG_MIDI_ISJOGFILTER:
				case DLG_PLUG_MIDI_ISOUTSYSPORT: return UIValues::page_settings_module_midi;
				case DLG_PLUG_MIDI_VMDRV_VER:
				case DLG_PLUG_MIDI_VMDRV_CHECK:
				case DLG_PLUG_MIDI_INSTALL_VMDRV: return UIValues::page_dependencies;
				default: return L""sv;
			}
		}
		static std::wstring_view select_RemotePage__(const uint16_t id) {
			switch (id) {
				case DLG_PLUG_REMOTE_ISENABLE:
				case DLG_PLUG_REMOTE_ISIPV6:
				case DLG_PLUG_REMOTE_ISFASTSOCK:
				case DLG_PLUG_REMOTE_ISREUSE:
				case DLG_PLUG_REMOTE_SYSLINK:
				case DLG_PLUG_REMOTE_IFACE_COMBO:
				case DLG_PLUG_REMOTE_LOGLEVEL_COMBO:
				case DLG_PLUG_REMOTE_PORT:
				case DLG_PLUG_REMOTE_COPY_ADDR:
				case DLG_PLUG_REMOTE_TMC:
				case DLG_PLUG_REMOTE_TMI:
				case DLG_PLUG_REMOTE_UA: return UIValues::page_settings_module_remote;
				default: return L""sv;
			}
		}
		static std::wstring_view select_GamePadPage__(const uint16_t id) {
			switch (id) {
				case DLG_PLUG_GAMEPAD_ISENABLE:
				case DLG_PLUG_GAMEPAD_DEVICES:
				case DLG_PLUG_GAMEPAD_TOTAL:
				case DLG_PLUG_GAMEPAD_CONNECT:
				case DLG_PLUG_GAMEPAD_SCENE:
				case DLG_PLUG_GAMEPAD_STEPINT:
				case DLG_PLUG_GAMEPAD_SLIDER1:
				case DLG_PLUG_GAMEPAD_RTYPE1:
				case DLG_PLUG_GAMEPAD_RTYPE2:
				case DLG_PLUG_GAMEPAD_RTYPE_COMBO:
				case DLG_PLUG_GAMEPAD_POLL:
				case DLG_PLUG_GAMEPAD_ASBTN:
				case DLG_PLUG_GAMEPAD_ISJOG: return UIValues::page_settings_module_gamepad;
				default: return L""sv;
			}
		}
		static std::wstring_view select_VMScriptPage__(const uint16_t id) {
			switch (id) {
				case DLG_PLUG_VMSCRIPT_ISENABLE:
				case DLG_PLUG_VMSCRIPT_WATCH:
				case DLG_PLUG_VMSCRIPT_DIR:
				case DLG_PLUG_VMSCRIPT_DIR_OPEN: return UIValues::page_settings_module_vm_script;
				case DLG_PLUG_VMSCRIPT_LIST:
				case DLG_PLUG_VMSCRIPT_TOTAL:  return UIValues::page_vm_script_mmtscripttester;
				default: return L""sv;
			}
		}

		void UiUtils::SetSliderInfo(HWND h, const uint32_t id, const uint32_t pos) {
			::SetDlgItemTextW(h, id, std::wstring(std::to_wstring(pos)).c_str());
		}
		void UiUtils::SetSliderValues(HWND h, const uint32_t ids, const uint32_t idt, const uint32_t minv, uint32_t const maxv, uint32_t pos) {
			try {
				HWND hi;
				if (!(hi = GetDlgItem(h, ids))) return;
				(void) ::SendMessageW(hi, TBM_SETRANGE, static_cast<WPARAM>(FALSE), static_cast<LPARAM>(MAKELONG(minv, maxv)));
				(void) ::SendMessageW(hi, TBM_SETPOS, static_cast<WPARAM>(TRUE), static_cast<LPARAM>(pos));
				UiUtils::SetSliderInfo(h, idt, pos);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		uint32_t UiUtils::GetSliderValue(HWND h, const uint32_t ids) {
			try {
				if (HWND hi; (hi = ::GetDlgItem(h, ids)))
					return static_cast<uint32_t>(::SendMessageW(hi, TBM_GETPOS, 0, 0));
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return (UINT_MAX - 1);
		}

		std::wstring UiUtils::GetControlText(HWND h, const uint32_t id) {
			if (wchar_t buf[MAX_PATH + 1]{}; ::GetDlgItemTextW(h, static_cast<int>(id), buf, static_cast<int>(std::size(buf))))
				return std::wstring(buf);
			return std::wstring();
		}
		bool UiUtils::GetControlChecked(HWND h, const uint32_t id) {
			return (::IsDlgButtonChecked(h, static_cast<int>(id)) == BST_CHECKED);
		}
		void UiUtils::SetControlEnable(HWND h, const uint32_t id, const bool val) {
			if (HWND hi; (hi = ::GetDlgItem(h, id)) != nullptr)
				::EnableWindow(hi, val);
		}
		const bool UiUtils::CheckControlEnable(HWND h, const uint32_t id) {
			
			if (HWND hi; (hi = ::GetDlgItem(h, static_cast<int>(id))) != nullptr)
				return ::IsWindowEnabled(hi);
			return false;
		}
		void UiUtils::SaveDialogEnabled(HWND h, bool b) {
			UiUtils::SetControlEnable(h, DLG_PLUG_SAVE, b);
		}

		std::wstring UiUtils::GetBlank(uint32_t val) {
			uint8_t i = (val > 9) ? ((val > 99) ? 0 : 1) : 2;
			return UIValues::blanks[i];
		}

		void UiUtils::ShowHelpPage(const std::wstring& lang, uint32_t dlgid, HELPINFO* h) {
			if (!h) return;
			UiUtils::ShowHelpPage(lang, dlgid, h->iCtrlId);
		}
		void UiUtils::ShowHelpPage(const std::wstring& lang, const uint16_t dlgid, const uint16_t eleid) {
			try {
				std::wstring page{};

				switch (dlgid) {
					case DLG_START_WINDOW: {
						switch (eleid) {
							case DLG_START_MIXER_ENABLE:
							case DLG_START_MIXER_DUPLICATE:
							case DLG_START_MIXER_OLD_VALUE:
							case DLG_START_MIXER_FAST_VALUE:
							case DLG_START_MIXER_RIGHT_CLICK: page = UIValues::page_audio_mixer; break;
							case DLG_START_PLACE_PLUGIN: page = UIValues::page_settings; break;
							case DLG_GO_STOP:
							case DLG_GO_START: page = UIValues::page_launch; break;
							case DLG_GO_ABOUT: page = UIValues::page_dependencies; break;
							default: {
								page = select_LightPage__(eleid);
								if (!page.empty()) break;
								page = select_MqttPage__(eleid);
								if (!page.empty()) break;
								page = select_MidiPage__(eleid);
								if (!page.empty()) break;
								page = select_RemotePage__(eleid);
								if (!page.empty()) break;
								page = select_GamePadPage__(eleid);
								if (!page.empty()) break;
								page = select_VMScriptPage__(eleid);
								if (!page.empty()) break;
								page = UIValues::page_index; break;
							}
						}
						break;
					}
					case DLG_PLUG_MQTT_WINDOW: {
						page = select_MqttPage__(eleid);
						if (!page.empty()) break;
						page = UIValues::page_settings_module_smart_house;
						break;
					}
					case DLG_PLUG_MIDI_WINDOW: {
						page = select_MidiPage__(eleid);
						if (!page.empty()) break;
						page = UIValues::page_settings_module_midi;
						break;
					}
					case DLG_PLUG_MMKEY_WINDOW: {
						page = UIValues::page_settings_module_multimedia_keys;
						break;
					}
					case DLG_PLUG_LIGHT_WINDOW: {
						page = select_LightPage__(eleid);
						if (!page.empty()) break;
						page = UIValues::page_settings_module_lights;
						break;
					}
					case DLG_PLUG_REMOTE_WINDOW: {
						page = select_RemotePage__(eleid);
						if (!page.empty()) break;
						page = UIValues::page_settings_module_remote;
						break;
					}
					case DLG_PLUG_GAMEPAD_WINDOW: {
						page = select_GamePadPage__(eleid);
						if (!page.empty()) break;
						page = UIValues::page_settings_module_gamepad;
						break;
					}
					case DLG_PLUG_VMSCRIPT_WINDOW: {
						page = select_VMScriptPage__(eleid);
						if (!page.empty()) break;
						page = UIValues::page_settings_module_vm_script;
						break;
					}
					case DLG_EDIT_WINDOW:
					case DLG_EDIT_INFO_WINDOW: page = UIValues::page_settings_configuration_files; break;
					case DLG_ABOUT_WINDOW: page = UIValues::page_broadcast_into_different_languages; break;
					case DLG_COLOR_WINDOW: page = UIValues::page_audio_mixer; break;
					case DLG_LOGVIEW_WINDOW: page = UIValues::page_settings; break;
					case DLG_MONITOR_WINDOW: page = UIValues::page_installation; break;
					default: page = UIValues::page_index; break;
				}
				std::wstring url = log_string::format(UIValues::help_fmt.data(), lang, page);
				if (!url.empty())
					::ShellExecuteW(0, 0, url.c_str(), 0, 0, SW_SHOW);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		std::wstring UiUtils::GetDragAndDrop(HDROP h) {
			std::wstring cnf{};
			if (!h) return cnf;
			try {
				uint32_t i, cnt = ::DragQueryFileW(h, -1, 0, 0);

				for (i = 0; i < cnt; ++i) {
					wchar_t p[MAX_PATH + 1]{};

					if (::DragQueryFileW(h, i, p, MAX_PATH)) {
						std::wstring s = std::wstring(p);
						if (s.ends_with(L".cnf")) {
							cnf = s;
							break;
						}
					}
				}
				::DragFinish(h);
			} catch (...) {}
			return cnf;
		}
		std::wstring UiUtils::OpenFileDialog(HWND h, COMDLG_FILTERSPEC* filter, size_t filter_size) {

			std::wstring s{};
			try {
				if (!h) return s;

				PWSTR pws = nullptr;
				IShellItem* item = nullptr;
				IFileOpenDialog* ptr = nullptr;

				try {
					do {
						HRESULT r = CoCreateInstance(
							CLSID_FileOpenDialog,
							NULL, CLSCTX_ALL,
							IID_IFileOpenDialog,
							reinterpret_cast<void**>(&ptr));

						if (r != S_OK) break;

						#pragma warning( push )
						#pragma warning( disable : 4267 )
						r = ptr->SetFileTypes(static_cast<UINT>(filter_size), filter);
						#pragma warning( pop )

						if (r != S_OK) break;
						r = ptr->Show(h);
						if (r != S_OK) break;
						r = ptr->GetResult(&item);
						if (r != S_OK) break;
						r = item->GetDisplayName(SIGDN_FILESYSPATH, &pws);
						if (r != S_OK) break;

						s = Utils::to_string(pws);

					} while (0);
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				if (item != nullptr) item->Release();
				if (ptr != nullptr) ptr->Release();
				if (pws != nullptr) CoTaskMemFree(pws);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return s;
		}
		std::wstring UiUtils::SaveFileDialog(HWND h, COMDLG_FILTERSPEC* filter, size_t filter_size) {

			std::wstring s{};
			try {
				if (!h) return s;

				PWSTR pws{ nullptr };
				IShellItem* item{ nullptr };
				IFileSaveDialog* ptr{ nullptr };

				try {
					COMDLG_FILTERSPEC filter[] = {
						{
							common_error_code::Get().get_error(common_error_id::err_MIDIMT_CONFFILTER).c_str(),
							L"*.cnf"
						}
					};
					do {
						HRESULT r = CoCreateInstance(
							CLSID_FileSaveDialog,
							NULL, CLSCTX_ALL,
							IID_IFileSaveDialog,
							reinterpret_cast<void**>(&ptr));

						if (r != S_OK) break;

						#pragma warning( push )
						#pragma warning( disable : 4267 )
						r = ptr->SetFileTypes(static_cast<UINT>(filter_size), filter);
						#pragma warning( pop )

						if (r != S_OK) break;
						r = ptr->Show(h);
						if (r != S_OK) break;
						r = ptr->GetResult(&item);
						if (r != S_OK) break;
						r = item->GetDisplayName(SIGDN_FILESYSPATH, &pws);
						if (r != S_OK) break;

						s = Utils::to_string(pws);

					} while (0);
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				if (item != nullptr) item->Release();
				if (ptr != nullptr) ptr->Release();
				if (pws != nullptr) CoTaskMemFree(pws);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return s;
		}

		const bool UiUtils::IsUIThread() {
			(void) ::GetLastError();
			if (!::IsGUIThread(1)) return false;
			if (::GetLastError() == ERROR_NOT_ENOUGH_MEMORY) return false;
			return true;
		}
		void UiUtils::Post(HWND h, const std::function<void()>& f) {
			try {
				if (!h) return;
				auto func = new std::function<void()>(f);
				::PostMessageW(h, WM_APP + 2, 0, (LPARAM)func);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void UiUtils::PostExec(LPARAM l) {
			try {
				if (!l) return;
				AsyncExecDeleter d = AsyncExecDeleter(l);
				if (!d.empty()) (*d.Get())();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
	}
}
