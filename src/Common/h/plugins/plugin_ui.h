/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023, MIT

	MIDIMMT DLL + MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace IO {
		
		using namespace std::string_view_literals;

		class FLAG_EXPORT PluginUi {
		protected:
			std::atomic<bool> isload_{ false };
			handle_ptr<HINSTANCE> hinst_{};
			hwnd_ptr<dialog_deleter> hwnd_{};

			static INT_PTR CALLBACK BuildDialogProc_(HWND, UINT, WPARAM, LPARAM);
			static LRESULT CALLBACK DialogProc_(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

		public:

			PluginUi() = default;
			PluginUi(const PluginUi&) = default;
			virtual ~PluginUi() = default;

			const bool empty() const;

			virtual HWND BuildDialog(HWND);
			virtual HWND BuildDialog(HINSTANCE, HWND, LPWSTR);
			virtual void CloseDialog();
			virtual void CloseAnimateDialog();
			virtual void ChangeDialogPosition(RECT&);
			virtual LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM);
		};
	}
}

