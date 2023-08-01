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

		class DialogConfig
		{
			CbEvent mcb__;

			handle_ptr<HWND> hwnd__;
			handle_ptr<ListMixerContainer*> last__;
			std::unique_ptr<ListEdit> lv__;
			std::unique_ptr<RToolBarDialogConfig> tb__;
			GuiImageStateButton<HICON> img_status__;
			GuiImageStateButton<HICON> btn_folder__;
			GuiImageStateButton<HICON> btn_runapp__;
			GuiImageStateButton<HICON> btn_remove__;
			GuiImageStateButton<HICON> btn_info__;
			GuiImageStateButton<HICON> btn_mute__;
			ConfigStatus ConfigDevice = ConfigStatus::None;
			std::wstring confpath__;

			void clear_();
			void dispose_();
			void InitListView();

			void helpcategory_addtarget_(HWND, uint32_t, const std::wstring_view);
			void helpcategory_addmmkey_(HWND, uint32_t);
			void set_volumeslider_(HWND, int32_t);
			void set_muteimage_(int32_t);
			void load_file_(std::wstring);
			void print_itemscount_(LONG);

		public:

			DialogConfig();
			~DialogConfig();
			const bool IsRunOnce();
			void SetFocus();

			void ListViewMenu(uint32_t);
			void ListViewMenu(LPNMHDR);
			void ListViewSort(LPNMHDR);
			void ListViewEdit(LPNMHDR);
			void ListViewClick(LPNMHDR);
			void ListViewDbClick(LPNMHDR);
			void ListViewFilter(LPNMHDR);

			void ToolBarSort(uint32_t);
			void ToolBarRecentOpen(uint32_t);
			void ToolBarEditDigitMode();
			void ToolBarFilterEmbed();
			void ToolBarFilterType(uint32_t);
			void ToolBarSetMode();
			void ToolBarSave();
			void ToolBarImport();
			void ToolBarExport();
			void ToolBarMonitor();
			void ToolBarFilters(uint32_t);
			void ToolBarFilterAutoCommit();
			void ToolBarEditorNotify(EditorNotify);

			void ChangeOnAppBox();
			void ChangeOnBtnAppFolder();
			void ChangeOnBtnAppRunning();
			void ChangeOnBtnAppDelete();
			void ChangeOnBtnMute();
			void ChangeOnSlider();

			void EventLog(CbEventData*);
			void EventMonitor(CbEventData*);

			void InitDialog(HWND, COPYDATASTRUCT* = nullptr);
			void EndDialog();
			void OpenDragAndDrop(std::wstring);
			void HelpCategorySelected(uint32_t);
		};

	}
}