/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2023, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "framework.h"
#include "MIDIMT.h"
#include "JsonConfig.h"

typedef struct tag_listviewedithdr {
	int item;
	int column;
	bool IsEscape;
	WNDPROC defProc;
} LISTVIEWEDITHDR;

class LISTVIEWSORT {
	int  column;
	bool ascending[5]{};
public:
	LISTVIEWSORT() : column(-1) { }
	bool operator==(const LISTVIEWSORT& x) {
		return (column != -1);
	}
	int  Column() { return (column == -1) ? 0 : column; }
	bool Ascending() { return (column == -1) ? false : ascending[column]; }
	void Set(int col) {
		column = ((col >= 0) && (col < 5)) ? col : -1;
		if (column != -1)
			ascending[col] = !ascending[col];
	}
};

class LISTVIEWPASTE {
	int  item, column;
	std::array<uint32_t, 5> val;
public:
	LISTVIEWPASTE() : item(-1), column(-1) { val.fill(255U); }
	bool operator==(const LISTVIEWPASTE& x) { return IsItemEmpty(); }
	bool IsValuesEmpty() { return (val.at(0) == 255U) || (val.at(1) == 255U) || (val.at(2) == 255U); }
	bool IsItemEmpty() { return (item == -1) || (column == -1); }
	int  Item()    { return item; }
	int  Column()  { return column; }
	std::array<uint32_t, 5> Values() { return val; }
	void SetItem(int itm, int col) {
		item = itm, column = col;
	}
	void SetValue(std::array<uint32_t, 5> const& arr) {
		std::copy(arr.begin(), arr.end(), val.begin());
	}
	void Reset() {
		val.fill(255U);
		item = column = -1;
	}
};

static const wchar_t* blanks[] = { L"", L"  ", L"    " };
static const wchar_t* columns[] = { L"id", L"scene", L"type", L"target", L"long-target" };
static const uint16_t iconsId[] = { IDB_ICON_SETUP3, IDB_ICON_SETUP2, IDB_ICON_SETUP1 };
static const uint16_t msgsId[] = { IDS_SETUP_MSG2, IDS_SETUP_MSG1, IDS_SETUP_MSG3 };

std::shared_ptr<LISTVIEWSORT> lvsort = nullptr;
std::shared_ptr<LISTVIEWPASTE> lvpaste = nullptr;

ListEdit::ListEdit(HINSTANCE h) {
	__hinst = h;
	ListEdit::ctrl = this;
}
ListEdit::~ListEdit() {
	Dispose();
}
void ListEdit::Dispose() {
	ListEdit::ctrl = nullptr;
	__hwndLv = nullptr;
	try {
		if (lvsort != nullptr) {
			lvsort.reset();
			lvsort = nullptr;
		}
		if (lvpaste != nullptr) {
			lvpaste.reset();
			lvpaste = nullptr;
		}
	}
	catch (...) {}
}
void ListEdit::ListViewErrorCb(const std::function<void(std::wstring)> fun) {
	if (fun != nullptr)
		ErrorFn = fun;
}

void ListEdit::ListViewInit(HWND h) {
	__hwndLv = h;
	ListEdit::ctrl = this;

	try {
		ListView_SetExtendedListViewStyle(__hwndLv,
			ListView_GetExtendedListViewStyle(__hwndLv) | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		lvsort = std::make_shared<LISTVIEWSORT>();
		lvpaste = std::make_shared<LISTVIEWPASTE>();

		LVGROUP lvg{};
		lvg.cbSize = sizeof(LVGROUP);
		lvg.mask = LVGF_HEADER | LVGF_GROUPID;
		LVCOLUMN lvc{};
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

		for (size_t i = 0; i < std::size(columns); i++) {

			lvc.iSubItem = static_cast<int>(i);
			lvc.pszText = (LPWSTR)columns[i];
			
			switch (i) {
			case 1: {
				lvc.cx = 45;
				lvc.fmt = LVCFMT_CENTER;
				break;
			}
			case 0:
			case 2: {
				lvc.cx = 35;
				lvc.fmt = LVCFMT_CENTER;
				break;
			}
			default: {
				lvc.cx = 78;
				lvc.fmt = LVCFMT_LEFT;
				break;
			}}
			if (ListView_InsertColumn(__hwndLv, i, &lvc) == -1)
				return;
			
			lvg.pszHeader = lvc.pszText;
			lvg.iGroupId = static_cast<int>(i);

			if (ListView_InsertGroup(__hwndLv, -1, &lvg) == -1)
				return;
		}
	}
	catch (const std::exception& ex) {
		ListEdit::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
}
void ListEdit::ListViewLoad(std::shared_ptr<MidiDevice>& dev) {
	if (__hwndLv == nullptr) return;

	try {
		if ((dev == nullptr) || (dev.get()->units == nullptr) || (dev.get()->Count() == 0))
			return;
		
		LVITEM lvi{};
		lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_DI_SETITEM;

		for (size_t n = 0; n < dev.get()->Count(); n++) {
			MidiUnit& mu = dev.get()->units[n];
			if (mu.id == 255) continue;
			std::array<uint32_t, 5> arr { mu.id, mu.scene, mu.type, mu.target, mu.longtarget };
			ListViewSetRow(arr);
		}
		lvsort.get()->Set(0);
		ListView_SortItemsEx(__hwndLv, ListViewSortEx, (LPARAM)lvsort.get());
	}
	catch (const std::exception& ex) {
		ListEdit::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
}
bool ListEdit::ListViewSort(LPNMHDR lpmh) {
	if ((lpmh == nullptr) || (lvsort == nullptr)) return false;
	NM_LISTVIEW *pnm = (NM_LISTVIEW*)lpmh;
	lvsort.get()->Set(pnm->iSubItem);
	ListView_SortItemsEx(pnm->hdr.hwndFrom, ListViewSortEx, (LPARAM)lvsort.get());
	return true;
}
bool ListEdit::ListViewMenu(LPNMHDR lpmh) {
	if ((lpmh == nullptr) || (lvpaste == nullptr)) return false;
	NM_LISTVIEW* pnm = (NM_LISTVIEW*)lpmh;
	lvpaste.get()->SetItem(pnm->iItem, pnm->iSubItem);

	HMENU hmenu = ::LoadMenu(__hinst, MAKEINTRESOURCE(IDR_LV_MENU));
	if (hmenu != nullptr) {
		HMENU hpmenu = ::GetSubMenu(hmenu, 0);
		if (hpmenu != nullptr) {

			bool b = IsTMidiStarted();
			uint32_t cnt = ::GetMenuItemCount(hpmenu);
			for (uint32_t i = 0; i < cnt; i++) {
				if (::GetMenuItemID(hpmenu, i) == IDM_LV_PASTE) {
					(void)EnableMenuItem(hpmenu, i, ((lvpaste.get()->IsValuesEmpty()) ? MF_GRAYED : MF_ENABLED) | MF_BYPOSITION);
					break;
				}
			}
			POINT pt = pnm->ptAction;
			::ClientToScreen(pnm->hdr.hwndFrom, &pt);
			::TrackPopupMenuEx(
				hpmenu,
				TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				pt.x,
				pt.y,
				GetParent(__hwndLv),
				NULL);
			::DestroyMenu(hmenu);
		}
	}
	return true;
}
bool ListEdit::ListViewMenu(uint32_t id) {
	if ((lvpaste == nullptr) || (lvpaste.get()->IsItemEmpty() && (id != IDM_LV_NEW)))
		return false;

	try {
		switch (id) {
			case IDM_LV_COPY: {
				std::array<uint32_t, 5> arr{ 255U, 255U, 255U, 255U, 255U };
				ListViewGetRow(lvpaste.get()->Item(), arr);
				if ((arr[0] >= 255) || (arr[1] >= 255U) || (arr[2] > 255U)) {
					lvpaste.get()->Reset();
					break;
				}
				lvpaste.get()->SetValue(arr);
				break;
			}
			case IDM_LV_NEW:
			case IDM_LV_PASTE: {
				std::array<uint32_t, 5> arr{};
				if (id == IDM_LV_NEW) arr.fill(255U);
				else if (lvpaste.get()->IsValuesEmpty()) break;
				else arr = lvpaste.get()->Values();

				(void)ListViewInsertItem(arr);
				break;
			}
			case IDM_LV_DELETE: {
				ListView_DeleteItem(__hwndLv, lvpaste.get()->Item());
				lvpaste.get()->Reset();
				break;
			}
			default: break;
		}
	}
	catch (const std::exception& ex) {
		ListEdit::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
	return true;
}
bool ListEdit::ListViewSetItem(LPNMHDR lpmh) {
	if (lpmh == nullptr) return false;

	NMLVDISPINFO* pdi = reinterpret_cast<NMLVDISPINFO*>(lpmh);
	if (pdi == nullptr) return false;

	ListView_SetItemText(lpmh->hwndFrom, pdi->item.iItem, pdi->item.iSubItem, pdi->item.pszText);
	return true;
}
bool ListEdit::ListViewEdit(LPNMHDR lpmh) {
	if (lpmh == nullptr) return false;

	LPNMITEMACTIVATE pia = reinterpret_cast<LPNMITEMACTIVATE>(lpmh);
	if ((pia == nullptr) || (pia->iItem < 0))
		return false;

	if (ListView_GetItemState(lpmh->hwndFrom, pia->iItem, LVIS_FOCUSED) & LVIS_FOCUSED)
		ListViewEditLabel(pia->iItem, pia->iSubItem);
	else
		ListView_SetItemState(lpmh->hwndFrom, pia->iItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	return true;
}

bool ListEdit::ListViewInsertItem(std::array<uint32_t, 5> const& arr) {
	try {
		int pos = ListViewSetRow(arr);
		lvpaste.get()->Reset();
		if (pos == -1) return false;
		ListView_SetItemState(__hwndLv, pos, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		return true;
	}
	catch (const std::exception& ex) {
		ListEdit::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
	return false;
}
bool ListEdit::ListViewGetList(std::shared_ptr<MidiDevice>& dev) {
	if (__hwndLv == nullptr) return false;
	try {
		int count = ListView_GetItemCount(__hwndLv);
		if (count == 0) return false;

		dev.get()->Init(count);

		int pos = -1, next = 0;
		while ((pos = ListView_GetNextItem(__hwndLv, pos, LVNI_ALL)) != -1) {

			std::array<uint32_t, 5> arr { 255U, 255U, 255U, 255U, 255U };
			ListViewGetRow(pos, arr);
			if ((arr[0] >= 255) || (arr[1] >= 255U) || (arr[2] > 255U) || (arr[3] > 255U))
				continue;

			MidiUnit & unit = dev.get()->units[next++];
			unit.id = arr[0];
			unit.scene = arr[1];
			unit.type = static_cast<MidiUnitType>(arr[2]);
			unit.target = static_cast<Mackie::Target>(arr[3]);
			unit.longtarget = static_cast<Mackie::Target>(arr[4]);
		}
		return true;
	}
	catch (const std::exception& ex) {
		ListEdit::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
	return false;
}
std::array<uint32_t, 5> ListEdit::ListViewGetSelectedRow(LPNMHDR lpmh) {
	std::array<uint32_t, 5> arr{ 255U, 255U, 255U, 255U, 255U };
	do {
		if (lpmh == nullptr) break;
		LPNMITEMACTIVATE pia = reinterpret_cast<LPNMITEMACTIVATE>(lpmh);
		if ((pia == nullptr) || (pia->iItem < 0)) break;
		if (ListView_GetItemState(lpmh->hwndFrom, pia->iItem, LVIS_FOCUSED) & LVIS_FOCUSED)
			ListViewGetRow(pia->iItem, arr);
	} while (0);
	return arr;
}
int ListEdit::ListViewCount() {
	return ListView_GetItemCount(__hwndLv);
}

/* Private */

bool ListEdit::ListViewEditLabel(int item, int column)
{
	RECT rt{};
	ListView_GetSubItemRect(__hwndLv, item, column, LVIR_LABEL, &rt);
	InflateRect(&rt, 2, 2);
	LV_COLUMN lvcol{};
	lvcol.mask = LVCF_FMT;
	ListView_GetColumn(__hwndLv, column, &lvcol);
	DWORD dwAlign;

	if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwAlign = ES_LEFT;
	else if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwAlign = ES_RIGHT;
	else
		dwAlign = ES_CENTER;

	TCHAR txt[260]{};
	ListView_GetItemText(__hwndLv, item, column, txt, 260);
	std::wstring ws(txt);
	HWND edit = CreateWindow(
		WC_EDIT,
		ws.c_str(),
		WS_BORDER | WS_CHILD | WS_VISIBLE | dwAlign,
		rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top,
		__hwndLv, NULL, GetModuleHandle(NULL), NULL);
	if (edit == NULL)
		return false;

	SetWindowFont(edit, GetWindowFont(__hwndLv), TRUE);
	SetFocus(edit);
	Edit_SetSel(edit, 0, -1);

	LISTVIEWEDITHDR* phdr = new LISTVIEWEDITHDR;
	phdr->item = item;
	phdr->column = column;
	phdr->IsEscape = FALSE;
	phdr->defProc = (WNDPROC)GetWindowLongPtr(edit, GWLP_WNDPROC);
	SetWindowLongPtr(edit, GWLP_USERDATA, (LONG_PTR)phdr);

	(void)SetWindowLongPtr(edit, GWLP_WNDPROC, (LONG_PTR)EditListViewProc);
	return true;
}
int  ListEdit::ListViewSetRow(std::array<uint32_t, 5> const& arr, int id) {
	try {
		int item = (id != -1) ? id : 0;
		LVITEM lvi{};
		lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_DI_SETITEM;

		for (size_t i = 0; i < std::size(columns); i++) {
			std::wstring ws = std::to_wstring(arr[i]);
			lvi.iItem = item;
			lvi.iSubItem = static_cast<int>(i);
			lvi.pszText = (LPWSTR)ws.c_str();
			if ((i == 0) && (id == -1)) {
				item = ListView_InsertItem(__hwndLv, &lvi);
				if (item == -1)
					break;

			}
			else (void)ListView_SetItem(__hwndLv, &lvi);
		}
		return item;
	}
	catch (const std::exception& ex) {
		ListEdit::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
	return -1;
}
void ListEdit::ListViewGetRow(int item, std::array<uint32_t, 5>& arr) {
	try {
		for (size_t i = 0; i < std::size(columns); i++) {
			wchar_t buf[10]{};
			ListView_GetItemText(__hwndLv, item, (int)i, buf, sizeof(buf));
			arr[i] = std::stoul(buf);
		}
	}
	catch (const std::exception& ex) {
		ListEdit::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
}

void ListEdit::ErrorCb(const std::string &s) {
	if (ListEdit::ctrl == nullptr) return;
	ListEdit::ctrl->ErrorFn(std::wstring(s.begin(), s.end()));
}

LRESULT CALLBACK ListEdit::EditListViewProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (ListEdit::ctrl == nullptr) return 0L;
	LISTVIEWEDITHDR* phdr = (LISTVIEWEDITHDR*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (message) {
		case WM_GETDLGCODE: return DLGC_WANTALLKEYS;
		case WM_KEYDOWN: {
			if (wParam == VK_ESCAPE || wParam == VK_RETURN || wParam == VK_TAB) {
				phdr->IsEscape = BOOL(wParam == VK_ESCAPE);
				SetFocus(GetParent(hwnd));
				return 0L;
			}
			break;
		}
		case WM_KILLFOCUS: {
			wchar_t pch[260]{};
			Edit_GetText(hwnd, pch, 260);

			if (!phdr->IsEscape) {
				LV_DISPINFO di = { 0 };
				di.hdr.hwndFrom = ListEdit::ctrl->__hwndLv;
				di.hdr.idFrom = GetDlgCtrlID(ListEdit::ctrl->__hwndLv);
				di.hdr.code = (UINT)LVN_ENDLABELEDIT;
				di.item.mask = LVIF_TEXT;
				di.item.iItem = phdr->item;
				di.item.iSubItem = phdr->column;
				di.item.pszText = NULL;
				di.item.pszText = pch;
				di.item.cchTextMax = 260;
				SendMessage(GetParent(ListEdit::ctrl->__hwndLv), WM_NOTIFY,
					(WPARAM)GetDlgCtrlID(ListEdit::ctrl->__hwndLv), (LPARAM)&di);
			}
			DestroyWindow(hwnd);

			delete phdr;
			return 0L;
		}
	}
	return CallWindowProc(phdr->defProc, hwnd, message, wParam, lParam);
}
int CALLBACK ListEdit::ListViewSortEx(LPARAM lp1, LPARAM lp2, LPARAM ldata) {
	if (ListEdit::ctrl == nullptr) return 0;
	LISTVIEWSORT *data = (LISTVIEWSORT*)ldata;
	if ((data == nullptr) || !data) return 0;

	try {
		wchar_t buf1[20]{}, buf2[20]{};
		ListView_GetItemText(ListEdit::ctrl->__hwndLv, lp1, data->Column(), buf1, _countof(buf1));
		ListView_GetItemText(ListEdit::ctrl->__hwndLv, lp2, data->Column(), buf2, _countof(buf2));

		int p1 = _wtoi(buf1),
			p2 = _wtoi(buf2);
		return data->Ascending() ? (p1 - p2) : (p2 - p1);
	}
	catch (const std::exception& ex) {
		ListEdit::ErrorCb(std::string(ex.what()));
	}
	catch (...) {}
	return 0;
}

ListEdit * ListEdit::ctrl = nullptr;

