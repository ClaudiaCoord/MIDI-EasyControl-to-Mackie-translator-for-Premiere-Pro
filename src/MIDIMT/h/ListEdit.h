/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIDIMT {

		enum class EditorNotify : int {
			ItemEmpty = 0,
			ItemFound,
			ValueFound,
			ValueEmpty,
			SelectedEnable,
			SelectedDisable,
			EditChangeEnable,
			EditChangeDisable,
			ReadMidiEnable,
			ReadMidiDisable,
			AutoCommit,
			EditDigits,
			FilterEmbed,
			FilterSetOr,
			FilterSetAnd,
		};

		class Columns {
		public:
			static const wchar_t* Captions[];
			static const int ColumnSize[];
			static const int ColumnFormat[];
			static const int Status;
			static const int Key;
			static const int Scene;
			static const int Type;
			static const int Target;
			static const int LongTarget;
			static const int ViewGroup;
			static const int HiddenGroup;
		};

		class ListUnitContainer {
		public:
			MIDI::MidiUnit unit;

			ListUnitContainer() = default;
			~ListUnitContainer() = default;
			ListUnitContainer(ListUnitContainer&&) = default;

			ListUnitContainer(const ListUnitContainer&);
			ListUnitContainer(ListUnitContainer*);
			ListUnitContainer(MIDI::MidiUnit&);
			ListUnitContainer(MIDI::MidiUnit&, DWORD);
			ListUnitContainer(MIDI::Mackie::MIDIDATA, DWORD);
		};

		class LISTVIEWSORT {
			int  column{ -1 };
			bool ascending[6]{};
		public:
			LISTVIEWSORT();
			const bool CheckColumn(int);
			bool operator==(const LISTVIEWSORT&);
			int  Column() const;
			bool Ascending() const;
			void Set(int);
			void Set(int, bool);
			void Reset();
		};
		class LISTVIEWPASTE {
			int  item{ -1 }, column{ -1 };
			ListUnitContainer* cont{ nullptr };

			void sendnotify_(HWND, EditorNotify);
		public:
			LISTVIEWPASTE();
			const bool operator==(const LISTVIEWPASTE&);
			const bool IsValueEmpty();
			const bool IsValuesEmpty();
			const bool IsItemEmpty() const;
			int  Item() const;
			int  Column() const;
			ListUnitContainer* Values() const;
			void SetItem(HWND, int, int);
			void SetValue(HWND, ListUnitContainer*);
			void Reset(HWND);
		};

		class ListEdit {
			hwnd_ptr<empty_deleter> hwndLv_{};
			std::function<void(const std::wstring&)> AddToLog = [](const std::wstring&) {};
			std::function<void()> UpdateSaveStatus = []() {};
			std::atomic<bool> is_show_duplicate_{ false };
			HIMAGELIST icons_{ nullptr };

			std::shared_ptr<LISTVIEWSORT> lvsort{};
			std::shared_ptr<LISTVIEWPASTE> lvpaste{};

			void dispose_();

			int  listview_setrow_(ListUnitContainer*, int = -1);
			ListUnitContainer* listview_getrow_(int);
			bool listview_editlabel_(int, int);
			void listview_digitallabel_(int, int, RECT&, std::wstring&);
			void listview_clearlist_(HWND);
			void listview_deleteitem_(HWND, int);
			void listview_setimage_(LVITEMW&, ListUnitContainer*);
			void listview_updateimage_(HWND, int32_t);
			std::future<std::vector<uint32_t>> listview_select_duplicate_(bool);

			static int CALLBACK listview_sortex_(LPARAM, LPARAM, LPARAM);
			static LRESULT CALLBACK listview_sub_editproc_(HWND hwnd, UINT m, WPARAM w, LPARAM l, UINT_PTR sc, DWORD_PTR data);

		public:
			ListEdit();
			~ListEdit();

			std::atomic<bool> EditAsDigit{ false };
			

			void ListViewAddToLogCb(const std::function<void(const std::wstring&)>);
			void ListViewUpdateStatusCb(const std::function<void()>);

			void ListViewInit(HWND);
			void ListViewEnd();
			void ListViewClear();
			void ListViewFilterEmbed(bool);
			void ListViewLoad(std::shared_ptr<JSON::MMTConfig>&);
			int  ListViewInsertItem(ListUnitContainer*);
			bool ListViewGetList(std::shared_ptr<JSON::MMTConfig>&);
			ListUnitContainer* ListViewGetSelectedRow(LPNMHDR);

			bool ListViewMenu(uint32_t);
			bool ListViewMenu(LPNMHDR);
			bool ListViewEdit(LPNMHDR);
			bool ListViewSort(LPNMHDR);
			bool ListViewSetItem(LPNMHDR);
			bool ListViewSort(uint32_t, bool);
			void ListViewFiltersReset();
			LONG ListViewFilter(bool);
			LONG ListViewFilter(MIDI::MidiUnit&, bool = false);
			LONG ListViewFilterDup();
			LONG ListViewDeleteDup();
			int32_t ListViewCount();
			size_t ListViewColumns();
		};
	}
}