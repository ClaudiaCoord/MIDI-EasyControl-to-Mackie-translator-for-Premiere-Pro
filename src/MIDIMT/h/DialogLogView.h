
#pragma once

namespace Common {
	namespace MIDIMT {

		class DialogLogView : public IO::PluginUi, public CbEvent {
		private:
			hwnd_ptr<hwnd_deleter> hwed_{};
			std::unique_ptr<UI::ScintillaBox> editor_{};

			void dispose_();
			void init_();
			void build_MenuPluginList_();

			void event_Log_(CbEventData*);
			void event_Monitor_(CbEventData*);
			void event_Text_(CbEventData*, CbHWNDType);
			void event_Config_(std::wstring, std::wstring);
			void event_Scint_(LPNMHDR);
			void event_Stat_(uint16_t);

			void zoomin_();
			void zoomout_();
			void gostart_();
			void goend_();
			void clear_();

			static LRESULT CALLBACK event_edit_(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

		public:

			DialogLogView();
			~DialogLogView() = default;

			IO::PluginUi* GetUi();

			const bool IsRunOnce();
			void SetFocus();

			HWND BuildDialog(HWND) override final;
			LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM) override final;
		};
	}
}