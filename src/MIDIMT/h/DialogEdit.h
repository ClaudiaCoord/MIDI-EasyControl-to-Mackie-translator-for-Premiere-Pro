/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIDIMT {

		enum ConfigStatus : uint16_t {
			None = 0,
			LoadFile,
			LoadDevice
		};

		class DialogEdit : public IO::PluginUi, public CbEvent {

			handle_ptr<ListMixerContainer*> last_{};
			std::unique_ptr<ListEdit> lv_{};
			std::unique_ptr<RToolBarDialogEdit> tb_{};
			UI::ImageStateButton<HICON> img_status_{};
			UI::ImageStateButton<HICON> btn_folder_{};
			UI::ImageStateButton<HICON> btn_runapp_{};
			UI::ImageStateButton<HICON> btn_remove_{};
			UI::ImageStateButton<HICON> btn_info_{};
			UI::ImageStateButton<HICON> btn_mute_{};
			ConfigStatus ConfigDevice = ConfigStatus::None;
			std::wstring confpath_{};

			void clear_();
			void dispose_();
			void init_();
			void init_config_(std::shared_ptr<JSON::MMTConfig>&);
			void init_lv_(HWND);

			void helpcategory_selected_(uint32_t);
			void helpcategory_addtarget_(HWND, uint32_t, const std::wstring_view);
			void helpcategory_addmmkey_(HWND, uint32_t);
			void set_volumeslider_(HWND, int32_t);
			void set_muteimage_(int32_t);
			void load_file_(std::wstring);
			void print_itemscount_(LONG);

			void event_Log_(CbEventData*);
			void event_Monitor_(CbEventData*);
			void event_DragAndDrop_(std::wstring);

			void lv_menu_(uint32_t);
			void lv_menu_(LPNMHDR);
			void lv_sort_(LPNMHDR);
			void lv_edit_(LPNMHDR);
			void lv_click_(LPNMHDR);
			void lv_dbclick_(LPNMHDR);
			void lv_filter_(LPNMHDR);

			void tb_sort_(uint32_t);
			void tb_recent_open_(uint32_t);
			void tb_edit_digitmode_();
			void tb_edit_notify_(EditorNotify);
			void tb_setmode_();
			void tb_save_();
			void tb_import_();
			void tb_export_();
			void tb_monitor_();
			void tb_filter_embed_();
			void tb_filter_type_(uint32_t);
			void tb_filter_auto_commit_();
			void tb_filters_(uint32_t);

			void changeOnAppBox_();
			void changeOnBtnAppFolder_();
			void changeOnBtnAppRunning_();
			void changeOnBtnAppDelete_();
			void changeOnBtnMute_();
			void changeOnSlider_();

		public:

			DialogEdit();
			~DialogEdit();

			IO::PluginUi* GetUi();
			void ExternalConfig(COPYDATASTRUCT*);

			const bool IsRunOnce();
			void SetFocus();

			HWND BuildDialog(HWND) override final;
			LRESULT CommandDialog(HWND, UINT, WPARAM, LPARAM) override final;

		};

	}
}