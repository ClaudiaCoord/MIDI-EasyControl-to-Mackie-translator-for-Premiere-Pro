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

		class ListMixerContainer {
		public:
			MIDI::MixerUnit unit;
			ListMixerContainer();
			ListMixerContainer(ListMixerContainer*);
			ListMixerContainer(MIDI::MixerUnit);
			ListMixerContainer(MIDI::MidiUnit&);
			ListMixerContainer(MIDI::MidiUnit&, DWORD&);
			ListMixerContainer(MIDI::Mackie::MIDIDATA&, DWORD&);
			~ListMixerContainer();
			MIDI::MidiUnit GetMidiUnit();
		};

		class LISTVIEWSORT {
			int  column;
			bool ascending[6]{};
		public:
			LISTVIEWSORT();
			const bool CheckColumn(int);
			bool operator==(const LISTVIEWSORT&);
			int  Column() const;
			bool Ascending();
			void Set(int);
			void Set(int, bool);
			void Reset();
		};
		class LISTVIEWPASTE {
			int  item, column;
			ListMixerContainer* cont;

			void sendnotify_(HWND, EditorNotify);
		public:
			LISTVIEWPASTE();
			const bool operator==(const LISTVIEWPASTE& x);
			const bool IsValueEmpty();
			const bool IsValuesEmpty();
			const bool IsItemEmpty();
			int  Item() const;
			int  Column() const;
			ListMixerContainer* Values() const;
			void SetItem(HWND, int, int);
			void SetValue(HWND, ListMixerContainer*);
			void Reset(HWND);
		};

		class ListEdit {
			handle_ptr<HWND> hwndLv__;
			std::function<void(std::wstring)> ErrorFn;
			HIMAGELIST icons__;

			std::shared_ptr<LISTVIEWSORT> lvsort;
			std::shared_ptr<LISTVIEWPASTE> lvpaste;

			void dispose_();

			int  listview_setrow_(ListMixerContainer*, int = -1);
			ListMixerContainer* listview_getrow_(int);
			bool listview_editlabel_(int, int);
			void listview_digitallabel_(int, int, RECT&, std::wstring&);
			void listview_clearlist_(HWND);
			void listview_deleteitem_(HWND, int);
			void listview_setimage_(LVITEMW&, ListMixerContainer*);
			void listview_updateimage_(HWND, int32_t);

			static int CALLBACK listview_sortex_(LPARAM, LPARAM, LPARAM);
			static LRESULT CALLBACK listview_sub_editproc_(HWND hwnd, UINT m, WPARAM w, LPARAM l, UINT_PTR sc, DWORD_PTR data);

		public:
			ListEdit();
			~ListEdit();

			std::atomic<bool> EditAsDigit{ false };

			void ListViewErrorCb(const std::function<void(std::wstring)>);

			void ListViewInit(HWND);
			void ListViewEnd();
			void ListViewFilterEmbed(bool);
			void ListViewLoad(std::shared_ptr<MIDI::MidiDevice>&);
			int  ListViewInsertItem(ListMixerContainer*);
			bool ListViewGetList(std::shared_ptr<MIDI::MidiDevice>&);
			ListMixerContainer* ListViewGetSelectedRow(LPNMHDR);

			bool ListViewMenu(uint32_t);
			bool ListViewMenu(LPNMHDR);
			bool ListViewEdit(LPNMHDR);
			bool ListViewSort(LPNMHDR);
			bool ListViewSetItem(LPNMHDR);
			bool ListViewSort(uint32_t, bool);
			void ListViewFiltersReset();
			LONG ListViewFilter(bool);
			LONG ListViewFilter(MIDI::MixerUnit&, bool = false);
			int32_t ListViewCount();
			size_t ListViewColumns();
		};
	}
}