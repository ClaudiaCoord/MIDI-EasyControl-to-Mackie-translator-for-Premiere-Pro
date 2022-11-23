/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2023, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

class ListEdit
{
	HINSTANCE __hinst = nullptr;
	HWND __hwndLv = nullptr;
	std::function<void(std::wstring)> ErrorFn{};
	static ListEdit *ctrl;

	void Dispose();

	int  ListViewSetRow(std::array<uint32_t, 5> const& arr, int id = -1);
	void ListViewGetRow(int, std::array<uint32_t, 5>&);
	bool ListViewEditLabel(int item, int column);

	static void ErrorCb(const std::string&);
	static LRESULT CALLBACK EditListViewProc(HWND, UINT, WPARAM, LPARAM);
	static int CALLBACK ListViewSortEx(LPARAM, LPARAM, LPARAM);

public:
	ListEdit(HINSTANCE);
	~ListEdit();

	void ListViewErrorCb(const std::function<void(std::wstring)>);

	void ListViewInit(HWND);
	void ListViewLoad(std::shared_ptr<MidiDevice>& dev);
	bool ListViewInsertItem(std::array<uint32_t, 5> const& arr);
	bool ListViewGetList(std::shared_ptr<MidiDevice> & dev);
	std::array<uint32_t, 5> ListViewGetSelectedRow(LPNMHDR lpmh);

	bool ListViewMenu(uint32_t id);
	bool ListViewMenu(LPNMHDR lpmh);
	bool ListViewSort(LPNMHDR lpmh);
	bool ListViewSetItem(LPNMHDR lpmh);
	bool ListViewEdit(LPNMHDR lpmh);
	int  ListViewCount();
};

