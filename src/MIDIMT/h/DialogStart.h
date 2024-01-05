
#pragma once

namespace Common {
	namespace MIDIMT {

		class DialogStart : public IO::PluginUi, public CbEvent {
		private:
			int32_t last_selected_plugin_{ -1 };
			IO::plugin_t& open_plugin_;
			UI::ImageStateButton<HICON> img_status_{};

			void dispose_();
			void init_();

			void event_Log_(CbEventData*);
			void event_Monitor_(CbEventData*);
			void event_DragAndDrop_(std::wstring);
			void event_LanguageChange_(uint16_t idx);
			void event_PluginsReload_();
			void build_LangCombobox_();
			void build_PluginListView_(bool = false);

			void changeMovePlugin_();
			void changeStateActions_(bool);
			void changeConfigView_(std::shared_ptr<JSON::MMTConfig>&);

			void changeOnLang_();
			void changeOnLang_(uint16_t);
			void changeOnConfigFileOpen_();
			void changeOnSaveConfig_();
			void changeOnListViewClick_();

			void start_();
			void stop_();

		public:

			DialogStart();
			~DialogStart();

			IO::PluginUi* GetUi();

			const bool IsRunOnce();
			void SetFocus();

			HWND BuildDialog(HWND) override final;
			LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM) override final;
		};
	}
}

