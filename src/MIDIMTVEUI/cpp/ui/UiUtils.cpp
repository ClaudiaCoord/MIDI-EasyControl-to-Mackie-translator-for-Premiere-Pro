/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ UI/Viewer/Editor interface.
	(c) CC 2023-2024, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include "../../Common/rc/resource_midimt.h"

namespace Common {
	namespace UI {

		using namespace std::string_view_literals;

		class UIValues {
		public:
			static const wchar_t* blanks[];

			static constexpr std::wstring_view help_fmt = L"https://claudiacoord.github.io/MIDI-MT/docs/{0}/{1}.html"sv;
			static constexpr std::wstring_view page_index = L"index"sv;
			static constexpr std::wstring_view page_mixer = L"Mixer"sv;
			static constexpr std::wstring_view page_launch = L"Launch"sv;
			static constexpr std::wstring_view page_lights = L"Lighting-control"sv;
			static constexpr std::wstring_view page_settings = L"Settings"sv;
			static constexpr std::wstring_view page_smarthome = L"Smart-House"sv;
			static constexpr std::wstring_view page_languages = L"Broadcast-into-different-languages"sv;
			static constexpr std::wstring_view page_installation = L"Installation"sv;
			static constexpr std::wstring_view page_dependencies = L"Dependencies"sv;
		};
		const wchar_t* UIValues::blanks[] = {L"", L"  ", L"    "};

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
				case DLG_PLUG_LIGHT_POLL: return UIValues::page_lights;
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
				case DLG_PLUG_MQTT_ISSELFSIGN: return UIValues::page_smarthome;
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
				case DLG_PLUG_MIDI_ISOUTSYSPORT: return UIValues::page_settings;
				case DLG_PLUG_MIDI_VMDRV_VER:
				case DLG_PLUG_MIDI_VMDRV_CHECK:
				case DLG_PLUG_MIDI_INSTALL_VMDRV: return UIValues::page_dependencies;
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
		void UiUtils::SetControlEnable(HWND hwnd, const uint32_t id, const bool val) {
			if (HWND hi; (hi = ::GetDlgItem(hwnd, id)) != nullptr)
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

		void UiUtils::ShowHelpPage(uint32_t dlgid, HELPINFO* h) {
			if (!h) return;
			UiUtils::ShowHelpPage(dlgid, h->iCtrlId);
		}
		void UiUtils::ShowHelpPage(const uint16_t dlgid, const uint16_t eleid) {
			try {
				std::wstring page{};

				switch (dlgid) {
					case DLG_START_WINDOW: {
						switch (eleid) {
							case DLG_START_MIXER_ENABLE:
							case DLG_START_MIXER_DUPLICATE:
							case DLG_START_MIXER_OLD_VALUE:
							case DLG_START_MIXER_FAST_VALUE:
							case DLG_START_MIXER_RIGHT_CLICK: page = UIValues::page_mixer; break;
							case DLG_PLUG_MMKEY_ISENABLE: page = UIValues::page_settings; break;
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
								page = UIValues::page_index; break;
							}
						}
						break;
					}
					case DLG_PLUG_MQTT_WINDOW: {
						page = select_MqttPage__(eleid);
						if (!page.empty()) break;
						page = UIValues::page_smarthome;
						break;
					}
					case DLG_PLUG_MIDI_WINDOW: {
						page = select_MidiPage__(eleid);
						if (!page.empty()) break;
						page = UIValues::page_launch;
						break;
					}
					case DLG_PLUG_MMKEY_WINDOW: {
						page = UIValues::page_mixer;
						break;
					}
					case DLG_PLUG_LIGHT_WINDOW: {
						page = select_LightPage__(eleid);
						if (!page.empty()) break;
						page = UIValues::page_lights;
						break;
					}

					case DLG_EDIT_WINDOW:
					case DLG_EDIT_INFO_WINDOW: page = UIValues::page_settings; break;
					case DLG_ABOUT_WINDOW: page = UIValues::page_languages; break;
					case DLG_COLOR_WINDOW: page = UIValues::page_mixer; break;
					
					case DLG_LOGVIEW_WINDOW:
					case DLG_MONITOR_WINDOW: page = UIValues::page_installation; break;
					default: page = UIValues::page_index; break;
				}
				std::wstring url{}; //= log_string::format(UIValues::help_fmt.data(), LangInterface::Get().GetLangId(), page);
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
	}
}
