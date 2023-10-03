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

		using namespace std::string_view_literals;
		constexpr std::wstring_view default_help_fmt__ = L"https://claudiacoord.github.io/MIDI-MT/docs/{0}/{1}.html"sv;
		constexpr std::wstring_view default_page_index__ = L"index"sv;
		constexpr std::wstring_view default_page_mixer__ = L"Mixer"sv;
		constexpr std::wstring_view default_page_launch__ = L"Launch"sv;
		constexpr std::wstring_view default_page_settings__ = L"Settings"sv;
		constexpr std::wstring_view default_page_smarthome__ = L"Smart-House"sv;
		constexpr std::wstring_view default_page_installation__ = L"Installation"sv;
		constexpr std::wstring_view default_page_dependencies__ = L"Dependencies"sv;

		static const wchar_t* blanks[] = { L"", L"  ", L"    " };

		void Gui::SetControlEnable(HWND hwnd, int id, bool val) {
			HWND hwctrl;
			if ((hwctrl = ::GetDlgItem(hwnd, id)) != nullptr)
				::EnableWindow(hwctrl, val);
		}
		bool Gui::CheckControlEnable(HWND hwnd, int id) {
			HWND hwctrl;
			if ((hwctrl = ::GetDlgItem(hwnd, id)) != nullptr)
				return ::IsWindowEnabled(hwctrl);
			return false;
		}
		void Gui::IcondataButtonEnabled(HWND hwnd, ICONDATA<HICON>* data, bool val) {
			SetControlEnable(hwnd, data->GetCtrlId(), val);
		}
		void Gui::SaveConfigEnabled(HWND hwnd) {
			SetControlEnable(hwnd, IDC_DIALOG_SAVE, true);
		}

		void Gui::ShowHelpPage(uint32_t dlgid, HELPINFO* hi) {
			if (!hi) return;
			Gui::ShowHelpPage(dlgid, hi->iCtrlId);
		}
		void Gui::ShowHelpPage(uint32_t dlgid, uint32_t eleid) {
			try {
				std::wstring page;

				switch (dlgid) {
					case IDD_FORMSTART: {
						switch (eleid) {
							case IDC_MMKEY_ENABLE:
							case IDC_MIXER_ENABLE:
							case IDC_MIXER_DUPLICATE:
							case IDC_MIXER_OLD_VALUE:
							case IDC_MIXER_FAST_VALUE:
							case IDC_MIXER_RIGHT_CLICK: page = default_page_mixer__; break;
							case IDC_MQTT_CA:
							case IDC_MQTT_PSK:
							case IDC_MQTT_PASS:
							case IDC_MQTT_PORT:
							case IDC_MQTT_ISSSL:
							case IDC_MQTT_LOGIN:
							case IDC_MQTT_IPADDR:
							case IDC_MQTT_CAOPEN:
							case IDC_MQTT_PREFIX:
							case IDC_MQTT_LOGLEVEL:
							case IDC_MQTT_ISSELFSIGN: page = default_page_smarthome__; break;
							case IDC_GO_START: page = default_page_dependencies__; break;
							default: page = default_page_launch__; break;
						}
						break;
					}
					case IDD_FORMMONITOR:
					case IDD_FORMSETUP: page = default_page_settings__; break;
					default: page = default_page_index__; break;
				}
				std::wstring url = log_string::format(default_help_fmt__.data(), LangInterface::Get().GetLangId(), page);
				if (!url.empty())
					::ShellExecuteW(0, 0, url.c_str(), 0, 0, SW_SHOW);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		std::wstring Gui::GetDragAndDrop(HDROP hd) {
			std::wstring cnf;
			if (!hd) return cnf;
			try {
				uint32_t i, cnt = ::DragQueryFileA(hd, -1, 0, 0);

				for (i = 0; i < cnt; ++i) {
					wchar_t p[MAX_PATH + 1]{};

					if (::DragQueryFileW(hd, i, p, MAX_PATH)) {
						std::wstring s = std::wstring(p);
						if (s.ends_with(L".cnf")) {
							cnf = s;
							break;
						}
					}
				}
				::DragFinish(hd);
			} catch (...) {}
			return cnf;
		}
		std::wstring Gui::GetBlank(uint32_t val) {
			uint8_t i = (val > 9) ? ((val > 99) ? 0 : 1) : 2;
			return blanks[i];
		}
		std::wstring Gui::GetControlText(HWND hwnd, int id) {
			wchar_t buf[MAX_PATH]{};
			if (!::GetDlgItemTextW(hwnd, id, buf, (int)std::size(buf))) return L"";
			return std::wstring(buf);
		}
		bool Gui::GetControlChecked(HWND hwnd, int id) {
			return (::IsDlgButtonChecked(hwnd, id) == BST_CHECKED);
		}

		/* BitmapInfo */

		#if defined (EXT_DRAW_BITMAP)
		BitmapInfo::BitmapInfo(BITMAP& bmp) {
			set(bmp);
		}
		BitmapInfo::BitmapInfo(HBITMAP hbmp) {
			BITMAP bmp{};
			if (::GetObjectW(hbmp, sizeof(bmp), &bmp) > 0) set(bmp);
		}
		void BitmapInfo::set(BITMAP& bmp) {
			bmi__.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi__.bmiHeader.biWidth = bmp.bmWidth;
			bmi__.bmiHeader.biHeight = bmp.bmHeight;
			bmi__.bmiHeader.biPlanes = bmp.bmPlanes;
			bmi__.bmiHeader.biBitCount = bmp.bmBitsPixel;
			bmi__.bmiHeader.biCompression = BI_RGB;
		}
		BITMAPINFO* BitmapInfo::get() {
			return &bmi__;
		}
		int BitmapInfo::width() {
			return bmi__.bmiHeader.biWidth;
		}
		int BitmapInfo::height() {
			return bmi__.bmiHeader.biHeight;
		}
		#endif

		/* ICONDATA */

		template <typename T>
		ICONDATA<T>::ICONDATA() : icon(nullptr), iconId(0U), ctrlId(0U) {}
		template <typename T>
		ICONDATA<T>::ICONDATA(T h) : icon(h), iconId(0U), ctrlId(0U) {}
		template <typename T>
		ICONDATA<T>::ICONDATA(T h, uint16_t i, uint16_t c) : icon(h), iconId(i), ctrlId(c) {}
		template <typename T>
		T ICONDATA<T>::Get() { return icon; }
		template <typename T>
		uint16_t ICONDATA<T>::GetIconId() { return iconId; }
		template <typename T>
		uint16_t ICONDATA<T>::GetCtrlId() { return ctrlId; }
		template <typename T>
		void ICONDATA<T>::Reset() {
			T h = icon;
			icon = nullptr;
			if (h != nullptr) {
				if constexpr (std::is_same_v<HBITMAP, T>) ::DeleteObject(h);
				else if constexpr (std::is_same_v<HICON, T>) ::DestroyIcon(h);
			}
		}

		template class ICONDATA<HICON>;
		template class ICONDATA<HBITMAP>;

		/* GuiImageStateButton */

		template <class T>
		GuiImageStateButton<T>::GuiImageStateButton() : isimagebox__(false) {
		}
		template <class T>
		GuiImageStateButton<T>::~GuiImageStateButton(){
			Dispose();
		}
		template <class T>
		void GuiImageStateButton<T>::Dispose() {
			try {
				std::vector<ICONDATA<T>*> icos = icos__;
				icos__.clear();

				for (auto& a : icos)
					if (a != nullptr) {
						a->Reset();
						delete a;
					}
				hwnd__.reset();
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		template <class T>
		void GuiImageStateButton<T>::Init(HWND hwnd, uint16_t id) {
			try {
				HWND hwctrl = ::GetDlgItem(hwnd, id);
				hwnd__.reset(hwctrl);
				SetStatus();
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		template <class T>
		void GuiImageStateButton<T>::Init(uint16_t ids[], size_t size, bool isimagebox) {
			try {
				isimagebox__ = isimagebox;
				LangInterface& lang = LangInterface::Get();
				for (size_t i = 0; i < size; i++) {
					uint16_t n = ids[i];
					HICON h = lang.GetIcon24x24(MAKEINTRESOURCEW(n));
					if (h == nullptr) continue;

					if constexpr (std::is_same_v<HICON, T>) {
						icos__.push_back(new ICONDATA<T>(h, n, 0U));
					}
					else if constexpr (std::is_same_v<HBITMAP, T>) {
						ICONINFOEX ii{};
						ii.cbSize = sizeof(ii);
						if (::GetIconInfoExW(h, &ii) && (ii.hbmColor != nullptr)) {
							T img = (T) ::CopyImage(ii.hbmColor, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
							::DestroyIcon(h);
							icos__.push_back(new ICONDATA<T>(img, n, 0U));
						}
					}
				}
			} catch (...) {
				Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		template <class T>
		void GuiImageStateButton<T>::Reset() {
			hwnd__.reset();
		}
		template <class T>
		void GuiImageStateButton<T>::Release() {
			Dispose();
		}
		template <class T>
		void GuiImageStateButton<T>::SetEnable(bool val) {
			if (!hwnd__) return;
			::EnableWindow(hwnd__.get(), val);
		}
		template <class T>
		void GuiImageStateButton<T>::SetStatus(uint16_t idx) {
			if (!hwnd__ || (icos__.size() <= idx)) return;
			(void) ::SendMessageW(hwnd__.get(), (isimagebox__ ? STM_SETIMAGE : BM_SETIMAGE), IMAGE_ICON, (LPARAM)icos__[idx]->Get());
		}
		template <class T>
		T GuiImageStateButton<T>::GetIcon(uint16_t idx) {
			if (icos__.size() <= idx) return nullptr;
			return icos__[idx]->Get();
		}
		template <class T>
		bool  GuiImageStateButton<T>::IsEmpty() const {
			return icos__.empty();
		}

		template class GuiImageStateButton<HICON>;
		template class GuiImageStateButton<HBITMAP>;
	}
}