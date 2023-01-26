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

		class Columns {
		public:
			static const wchar_t* Captions[];
			static const int Status;
			static const int Key;
			static const int Scene;
			static const int Type;
			static const int Target;
			static const int LongTarget;
		};

		class ListMixerContainer {
		public:
			MIDI::MixerUnit unit;
			ListMixerContainer();
			ListMixerContainer(MIDI::MixerUnit);
			ListMixerContainer(MIDI::MidiUnit&);
			ListMixerContainer(MIDI::MidiUnit&, DWORD&);
			ListMixerContainer(MIDI::Mackie::MIDIDATA&, DWORD&);
			~ListMixerContainer();
			MIDI::MidiUnit GetMidiUnit();
		};

		class ListEdit
		{
			handle_ptr<HWND> hwndLv__;
			std::function<void(std::wstring)> ErrorFn;
			static ListEdit* ctrl__;
			HIMAGELIST icons__;

			void Dispose();

			int  ListViewSetRow(ListMixerContainer*, int = -1);
			ListMixerContainer* ListViewGetRow(int);
			bool ListViewEditLabel(int item, int column);
			void ListViewClearList(HWND);
			void ListViewSetImage(LVITEMW&, ListMixerContainer*);

			static LRESULT CALLBACK EditListViewProc(HWND, UINT, WPARAM, LPARAM);
			static int CALLBACK ListViewSortEx(LPARAM, LPARAM, LPARAM);

		public:
			ListEdit();
			~ListEdit();

			void ListViewErrorCb(const std::function<void(std::wstring)>);

			void ListViewInit(HWND);
			void ListViewEnd();
			void ListViewLoad(std::shared_ptr<MIDI::MidiDevice>& dev);
			bool ListViewInsertItem(ListMixerContainer*);
			bool ListViewGetList(std::shared_ptr<MIDI::MidiDevice>& dev);
			ListMixerContainer* ListViewGetSelectedRow(LPNMHDR lpmh);

			bool ListViewMenu(uint32_t id);
			bool ListViewMenu(LPNMHDR lpmh);
			bool ListViewSort(LPNMHDR lpmh);
			bool ListViewSetItem(LPNMHDR lpmh);
			bool ListViewEdit(LPNMHDR lpmh);
			int  ListViewCount();
			size_t ListViewColumns();
		};
	}
}