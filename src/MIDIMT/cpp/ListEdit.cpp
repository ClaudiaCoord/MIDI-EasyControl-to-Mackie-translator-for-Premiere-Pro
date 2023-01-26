/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#include "MIDIMT.h"
#include <array>

namespace Common {
	namespace MIDIMT {

		const wchar_t* Columns::Captions[] = { L"...", L"id", L"scene", L"type", L"target", L"long-target"};
		const int Columns::Status = 0;
		const int Columns::Key = 1;
		const int Columns::Scene = 2;
		const int Columns::Type = 3;
		const int Columns::Target = 4;
		const int Columns::LongTarget = 5;


		ListMixerContainer::ListMixerContainer() {
			unit.key = 255U;
			unit.scene = 255U;
			unit.type = MIDI::MidiUnitType::UNITNONE;
			unit.target = MIDI::Mackie::Target::NOTARGET;
			unit.longtarget = MIDI::Mackie::Target::NOTARGET;
		}
		ListMixerContainer::ListMixerContainer(MIDI::Mackie::MIDIDATA& m, DWORD& t) {
			unit.key = m.key();
			unit.scene = m.scene();
			unit.type = (m.value() == 127U) ? MIDI::MidiUnitType::BTN : ((m.value() > 0) ? MIDI::MidiUnitType::SLIDER : MIDI::MidiUnitType::UNITNONE);
			unit.value.value = m.value();
			unit.value.time = t;
		}
		ListMixerContainer::ListMixerContainer(MIDI::MidiUnit& m, DWORD& t) {
			unit.Copy(m);
			unit.value.time = t;
		}
		ListMixerContainer::ListMixerContainer(MIDI::MidiUnit& m) {
			unit.Copy(m);
		}
		ListMixerContainer::~ListMixerContainer() {
		}
		MIDI::MidiUnit ListMixerContainer::GetMidiUnit() {
			MIDI::MidiUnit m{};
			m.Copy(unit);
			return m;
		}

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
			int  item__, column__;
			ListMixerContainer* cont__;
		public:
			LISTVIEWPASTE() : item__(-1), column__(-1), cont__(nullptr) { }
			bool operator==(const LISTVIEWPASTE& x) { return IsItemEmpty(); }
			bool IsValuesEmpty() {
				return ((cont__ == nullptr) || (cont__->unit.key >= 255) || (cont__->unit.scene >= 255U) || (cont__->unit.type == MIDI::MidiUnitType::UNITNONE));
			}
			bool IsItemEmpty() { return (item__ == -1) || (column__ == -1); }
			int  Item() { return item__; }
			int  Column() { return column__; }
			ListMixerContainer* Values() { return cont__; }
			void SetItem(int item, int col) {
				item__ = item, column__ = col;
			}
			void SetValue(ListMixerContainer* cont) {
				cont__ = cont;
			}
			void Reset() {
				cont__ = nullptr;
				item__ = column__ = -1;
			}
		};

		std::shared_ptr<LISTVIEWSORT> lvsort;
		std::shared_ptr<LISTVIEWPASTE> lvpaste;

		/* Static local */

		static std::tuple<bool, HMENU, std::vector<HMENU>> buildContextMenu() {
			try {
				std::vector<HMENU> v;
				do {
					HMENU hm, hmp;
					if ((hm = LangInterface::Get().GetMenu(MAKEINTRESOURCE(IDR_LV_MENU))) == nullptr) break;
					v.push_back(hm);
					if ((hmp = ::GetSubMenu(hm, 0)) == nullptr) break;

					bool isbrun = common_config::Get().Local.IsMidiBridgeRun(),
						 ispaste = !lvpaste.get()->IsValuesEmpty();

					uint32_t complette = 0U, cnt = ::GetMenuItemCount(hmp);
					for (uint32_t i = 0; i < cnt; i++) {
						uint32_t id = ::GetMenuItemID(hmp, i);
						switch (id) {
							case IDM_LV_PASTE: {
								(void) ::EnableMenuItem(hmp, i, (ispaste ? MF_ENABLED : MF_GRAYED) | MF_BYPOSITION);
								complette++;
								break;
							}
							case IDM_LV_READ_MIDI_CODE: {
								(void) ::EnableMenuItem(hmp, i, (isbrun ? MF_ENABLED : MF_GRAYED) | MF_BYPOSITION);
								complette++;
								break;
							}
							default: break;
						}
						if (complette >= 2) break;
					}
					return std::make_tuple(true, hmp, v);
				} while (0);
				for (auto& m : v) ::DestroyMenu(m);
				return std::make_tuple(false, nullptr, std::vector<HMENU>());
			}
			catch (...) {}
			return std::make_tuple(false, nullptr, std::vector<HMENU>());
		}
		static std::tuple<bool, HMENU, std::vector<HMENU>> buildCol3Menu(MIDI::MidiUnitType type) {
			try {
				std::vector<HMENU> v;
				do {
					HMENU hm, hmp;
					if ((hm = LangInterface::Get().GetMenu(MAKEINTRESOURCE(IDR_COL3_TYPE_MENU))) == nullptr) break;
					v.push_back(hm);
					if ((hmp = ::GetSubMenu(hm, 0)) == nullptr) break;

					(void) ::CheckMenuItem(hmp, (static_cast<uint32_t>(type) + IDM_COL3_BASE), MF_CHECKED | MF_BYCOMMAND);
					return std::make_tuple(true, hmp, v);
				} while (0);
				for (auto& m : v) ::DestroyMenu(m);
				return std::make_tuple(false, nullptr, std::vector<HMENU>());
			}
			catch (...) {}
			return std::make_tuple(false, nullptr, std::vector<HMENU>());
		}
		static std::tuple<bool, HMENU, std::vector<HMENU>> buildCol4_5Menu(int column, MIDI::MidiUnitType type, MIDI::Mackie::Target target, MIDI::Mackie::Target longtarget) {
			try {
				uint32_t cmd = 0, mid = 0;
				switch (column)
				{
					case Columns::Target: {
						switch (type) {
							case MIDI::MidiUnitType::BTN:
							case MIDI::MidiUnitType::BTNTOGGLE: /* BUTTONS */ mid = IDR_COL4_BTN_TARGET_MENU; break;
							case MIDI::MidiUnitType::UNITNONE:  /* NONE */    mid = IDR_COL4_ALL_TARGET_MENU; break;
							default:							/* SLISERS */ mid = IDR_COL4_SLIDER_TARGET_MENU; break;
						}
						cmd = static_cast<uint32_t>(target) + IDM_COL4_BASE;
						break;
					}
					case Columns::LongTarget: {
						switch (type) {
							case MIDI::MidiUnitType::BTN:
							case MIDI::MidiUnitType::BTNTOGGLE: /* BUTTONS */ {
								switch (target) {
									case MIDI::Mackie::Target::MEDIAKEY:  mid = IDR_COL5_BTN_MMKEY_MENU; break;
									case MIDI::Mackie::Target::VOLUMEMIX: mid = IDR_COL5_BTN_MIX_MENU; break;
									case MIDI::Mackie::Target::NOTARGET:
									default: mid = IDR_COL4_BTN_TARGET_MENU; break;
								}
								break;
							}
							case MIDI::MidiUnitType::UNITNONE:  mid = IDR_COL4_ALL_TARGET_MENU; break;
							default: /* SLISERS */ {
								switch (target) {
									case MIDI::Mackie::Target::VOLUMEMIX: mid = IDR_COL5_SLIDER_MIX_MENU; break;
									case MIDI::Mackie::Target::MEDIAKEY:
									case MIDI::Mackie::Target::NOTARGET:
									default: mid = IDR_COL4_SLIDER_TARGET_MENU; break;
								}
								break;
							}
						}
						cmd = static_cast<uint32_t>(longtarget) + IDM_COL4_BASE;
						break;
					}
					default: break;
				}

				std::vector<HMENU> v;
				do {
					HMENU hm, hmp;
					if (mid == 0) break;
					if ((hm = LangInterface::Get().GetMenu(MAKEINTRESOURCE(mid))) == nullptr) break;
					v.push_back(hm);
					if ((hmp = ::GetSubMenu(hm, 0)) == nullptr) break;

					if (mid == IDR_COL4_ALL_TARGET_MENU) {

						MENUITEMINFOW mi{};
						mi.cbSize = sizeof(mi);
						mi.fMask = MIIM_SUBMENU;

						if ((hm = LangInterface::Get().GetMenu(MAKEINTRESOURCE(IDR_COL4_BTN_TARGET_MENU))) == nullptr) break;
						v.push_back(hm);
						if ((mi.hSubMenu = ::GetSubMenu(hm, 0)) == nullptr) break;
						if (!SetMenuItemInfoW(hmp, IDR_COL5_MENU_REPLACE_FIELD2, false, &mi)) break;

						if ((hm = LangInterface::Get().GetMenu(MAKEINTRESOURCE(IDR_COL4_SLIDER_TARGET_MENU))) == nullptr) break;
						v.push_back(hm);
						if ((mi.hSubMenu = ::GetSubMenu(hm, 0)) == nullptr) break;
						if (!SetMenuItemInfoW(hmp, IDR_COL5_MENU_REPLACE_FIELD1, false, &mi)) break;
					}
					if (cmd != 0)
						(void) ::CheckMenuItem(hmp, cmd, MF_CHECKED | MF_BYCOMMAND);
					return std::make_tuple(true, hmp, v);

				} while (0);
				for (auto& m : v) ::DestroyMenu(m);
				return std::make_tuple(false, nullptr, std::vector<HMENU>());
			}
			catch (...) {}
			return std::make_tuple(false, nullptr, std::vector<HMENU>());
		}

		/* Class */

		ListEdit::ListEdit() : icons__(nullptr), ErrorFn([](std::wstring){}) {
			ListEdit::ctrl__ = this;
			icons__ = ImageList_Create(16, 16, ILC_MASK | ILC_ORIGINALSIZE, 1, 1);

			for (int32_t i = IDI_ICON_LV1; i <= IDI_ICON_LV5; i++) {
				HICON ico = LangInterface::Get().GetIcon16x16(MAKEINTRESOURCE(i));
				if (ico != nullptr) {
					ImageList_AddIcon(icons__, ico);
					::DestroyIcon(ico);
				}
			}
		}
		ListEdit::~ListEdit() {
			Dispose();
			HIMAGELIST icons = icons__;
			icons__ = nullptr;
			if (icons != nullptr)
				ImageList_Destroy(icons);
		}
		void ListEdit::Dispose() {
			if (!hwndLv__) return;
			HWND hwnd = hwndLv__.release();
			ListEdit::ctrl__ = nullptr;
			try {
				ListViewClearList(hwnd);
				if (lvsort)  lvsort.reset();
				if (lvpaste) lvpaste.reset();
			} catch (...) {}
		}
		size_t ListEdit::ListViewColumns() {
			return std::size(Columns::Captions);
		}
		void ListEdit::ListViewErrorCb(const std::function<void(std::wstring)> fun) {
			if (fun != nullptr)
				ErrorFn = fun;
		}

		void ListEdit::ListViewEnd() {
			Dispose();
		}
		void ListEdit::ListViewInit(HWND hwnd) {
			hwndLv__.reset(hwnd);
			ListEdit::ctrl__ = this;

			try {
				ListView_SetExtendedListViewStyle(hwnd,
					ListView_GetExtendedListViewStyle(hwnd) | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
				lvsort = std::make_shared<LISTVIEWSORT>();
				lvpaste = std::make_shared<LISTVIEWPASTE>();

				ListView_SetInsertMarkColor(hwnd, RGB(50,00,00));
				ListView_SetImageList(hwnd, icons__, LVSIL_SMALL);

				LVGROUP lvg{};
				lvg.cbSize = sizeof(LVGROUP);
				lvg.mask = LVGF_HEADER | LVGF_GROUPID;
				LVCOLUMN lvc{};
				lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

				for (size_t i = 0; i < std::size(Columns::Captions); i++) {

					lvc.iSubItem = static_cast<int>(i);
					lvc.pszText = (LPWSTR)Columns::Captions[i];

					switch (i) {
						case Columns::Status: {
							lvc.cx = 26;
							lvc.fmt = LVCFMT_CENTER;
							break;
						}
						case Columns::Key: {
							lvc.cx = 40;
							lvc.fmt = LVCFMT_CENTER;
							break;
						}
						case Columns::Scene: {
							lvc.cx = 40;
							lvc.fmt = LVCFMT_CENTER;
							break;
						}
						case Columns::Type: {
							lvc.cx = 35;
							lvc.fmt = LVCFMT_CENTER;
							break;
						}
						case Columns::Target:
						case Columns::LongTarget: {
							lvc.cx = 70;
							lvc.fmt = LVCFMT_LEFT;
							break;
						}
						default: {
							lvc.cx = 10;
							lvc.fmt = LVCFMT_LEFT;
							break;
						}
					}
					if (ListView_InsertColumn(hwnd, i, &lvc) == -1)
						return;

					lvg.pszHeader = lvc.pszText;
					lvg.iGroupId = static_cast<int>(i);

					if (ListView_InsertGroup(hwnd, -1, &lvg) == -1)
						return;
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void ListEdit::ListViewLoad(std::shared_ptr<MIDI::MidiDevice>& dev) {
			if (!hwndLv__) return;
			HWND hwnd = hwndLv__.get();

			try {
				if (dev->units.empty())
					return;

				LVITEM lvi{};
				lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_DI_SETITEM;

				for (auto& a : dev->units) {
					if (a.key == 255) continue;
					ListViewSetRow(new ListMixerContainer(a));
				}
				lvsort->Set(0);
				ListView_SortItemsEx(hwnd, ListViewSortEx, (LPARAM)lvsort.get());
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		bool ListEdit::ListViewSort(LPNMHDR lpmh) {
			if ((lpmh == nullptr) || (lvsort == nullptr)) return false;
			NM_LISTVIEW* pnm = (NM_LISTVIEW*)lpmh;
			lvsort.get()->Set(pnm->iSubItem);
			ListView_SortItemsEx(pnm->hdr.hwndFrom, ListViewSortEx, (LPARAM)lvsort.get());
			return true;
		}
		bool ListEdit::ListViewMenu(LPNMHDR lpmh) {
			if (!hwndLv__ || (lpmh == nullptr) || (lvpaste == nullptr)) return false;
			HWND hwnd = hwndLv__.get();

			try {
				NM_LISTVIEW* pnm = (NM_LISTVIEW*)lpmh;
				lvpaste.get()->SetItem(pnm->iItem, pnm->iSubItem);

				auto data = buildContextMenu();
				if (!std::get<0>(data)) return false;
				HMENU hpm = std::get<1>(data);

				POINT pt = pnm->ptAction;
				::ClientToScreen(pnm->hdr.hwndFrom, &pt);
				::TrackPopupMenuEx(
					hpm,
					TPM_LEFTALIGN | TPM_RIGHTBUTTON,
					pt.x,
					pt.y,
					GetParent(hwnd),
					0);

				for (auto& m : std::get<2>(data)) ::DestroyMenu(m);
				return true;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		bool ListEdit::ListViewMenu(uint32_t id) {
			if (!hwndLv__) return false;
			HWND hwnd = hwndLv__.get();

			if ((lvpaste == nullptr) || (lvpaste.get()->IsItemEmpty() && (id != IDM_LV_NEW)))
				return false;

			try {
				switch (id) {
					case IDM_LV_COPY: {
						ListMixerContainer* cont = ListViewGetRow(lvpaste->Item());
						if ((cont == nullptr) || (cont->unit.key >= 255) || (cont->unit.scene >= 255U) || (cont->unit.type == MIDI::MidiUnitType::UNITNONE)) {
							lvpaste->Reset();
							break;
						}
						lvpaste->SetValue(cont);
						break;
					}
					case IDM_LV_NEW:
					case IDM_LV_PASTE: {
						ListMixerContainer* cont = nullptr;
						if (id == IDM_LV_NEW) cont = new ListMixerContainer();
						else if (lvpaste->IsValuesEmpty()) break;
						else cont = lvpaste->Values();

						if (cont == nullptr) break;
						(void)ListViewInsertItem(cont);
						break;
					}
					case IDM_LV_DELETE: {
						ListView_DeleteItem(hwnd, lvpaste->Item());
						lvpaste->Reset();
						break;
					}
					case IDM_LV_SET_MMKEY:
					case IDM_LV_SET_MIXER: {
						uint32_t val = static_cast<uint32_t>((id == IDM_LV_SET_MMKEY) ? MIDI::Mackie::Target::MEDIAKEY : MIDI::Mackie::Target::VOLUMEMIX);
						std::wstring ws = std::to_wstring(val);
						ListView_SetItemText(hwnd, lvpaste->Item(), Columns::Target, (LPWSTR)ws.c_str());
						lvpaste->Reset();
						break;
					}
					default: break;
				}
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
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

		bool ListEdit::ListViewInsertItem(ListMixerContainer *cont) {
			if (!hwndLv__) return false;
			HWND hwnd = hwndLv__.get();

			try {
				int pos = ListViewSetRow(cont);
				lvpaste->Reset();
				if (pos == -1) return false;
				ListView_SetItemState(hwnd, pos, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				return true;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		bool ListEdit::ListViewGetList(std::shared_ptr<MIDI::MidiDevice>& dev) {
			if (!hwndLv__) return false;
			HWND hwnd = hwndLv__.get();

			try {
				int count = ListView_GetItemCount(hwnd);
				if (count == 0) return false;

				dev->Clear();

				int pos = -1;
				while ((pos = ListView_GetNextItem(hwnd, pos, LVNI_ALL)) != -1) {

					ListMixerContainer* cont = ListViewGetRow(pos);
					if ((cont == nullptr) || (cont->unit.key >= 255) || (cont->unit.scene >= 255U) || (cont->unit.type == MIDI::MidiUnitType::UNITNONE))
						continue;
					dev->units.push_back(std::move(cont->GetMidiUnit()));
				}
				return true;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		ListMixerContainer* ListEdit::ListViewGetSelectedRow(LPNMHDR lpmh) {
			do {
				if (lpmh == nullptr) break;
				LPNMITEMACTIVATE pia = reinterpret_cast<LPNMITEMACTIVATE>(lpmh);
				if ((pia == nullptr) || (pia->iItem < 0)) break;
				if (ListView_GetItemState(lpmh->hwndFrom, pia->iItem, LVIS_FOCUSED) & LVIS_FOCUSED)
					return ListViewGetRow(pia->iItem);
			} while (0);
			return nullptr;
		}
		int ListEdit::ListViewCount() {
			if (!hwndLv__) return 0;
			return ListView_GetItemCount(hwndLv__.get());
		}

		/* Private */

		void ListEdit::ListViewSetImage(LVITEMW& lvi, ListMixerContainer* cont) {
			if ((cont->unit.key == 255U) || (cont->unit.scene == 255U)) {
				lvi.iImage = 1;
			} else {
				switch (cont->unit.target) {
					case MIDI::Mackie::Target::VOLUMEMIX: lvi.iImage = 3; break;
					case MIDI::Mackie::Target::MEDIAKEY:  lvi.iImage = 4; break;
					case MIDI::Mackie::Target::NOTARGET:  lvi.iImage = 0; break;
					default:							  lvi.iImage = 2; break;
				}
			}
		}
		void ListEdit::ListViewClearList(HWND hwnd) {
			try {
				int count = ListView_GetItemCount(hwnd);
				if (count == 0) return;

				int pos = -1;
				while ((pos = ListView_GetNextItem(hwnd, pos, LVNI_ALL)) != -1) {

					LVITEMW lvi{};
					lvi.mask = LVIF_PARAM;
					lvi.iItem = pos;

					if (ListView_GetItem(hwnd, &lvi)) {
						ListMixerContainer* cont = reinterpret_cast<ListMixerContainer*>(lvi.lParam);
						if (cont != nullptr) delete cont;
					}
				}
				(void) ListView_DeleteAllItems(hwnd);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		int ListEdit::ListViewSetRow(ListMixerContainer* cont, int id) {
			if (!hwndLv__) return -1;
			HWND hwnd = hwndLv__.get();

			try {
				int item = (id != -1) ? id : 0;

				for (size_t i = 0; i < std::size(Columns::Captions); i++) {
					LVITEMW lvi{};

					if (i == 0) {
						lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_DI_SETITEM | LVIF_PARAM;
						lvi.lParam = (LPARAM)cont;
						ListViewSetImage(lvi, cont);
					} else {
						lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_DI_SETITEM;
					}

					std::wstring ws;
					switch (i) {
						case Columns::Status: break;
						case Columns::Key: ws = std::to_wstring(cont->unit.key); break;
						case Columns::Scene: ws = std::to_wstring(cont->unit.scene); break;
						case Columns::Type: ws = std::to_wstring(cont->unit.type); break;
						case Columns::Target: ws = std::to_wstring(cont->unit.target); break;
						case Columns::LongTarget: ws = std::to_wstring(cont->unit.longtarget); break;
						default: break;
					}
					lvi.iItem = item;
					lvi.iSubItem = static_cast<int>(i);
					lvi.pszText = (LPWSTR)ws.c_str();

					if ((i == 0) && (id == -1)) {
						item = ListView_InsertItem(hwnd, &lvi);
						if (item == -1)
							break;

					} else (void) ListView_SetItem(hwnd, &lvi);
				}
				return item;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return -1;
		}
		ListMixerContainer* ListEdit::ListViewGetRow(int item) {
			if (!hwndLv__) return nullptr;
			HWND hwnd = hwndLv__.get();

			try {
				ListMixerContainer* cont = nullptr;
				wchar_t buf[10]{};
				LVITEMW lvi{};
				lvi.mask = LVIF_PARAM;
				lvi.iItem = item;

				if (ListView_GetItem(hwnd, &lvi))
					cont = reinterpret_cast<ListMixerContainer*>(lvi.lParam);
				if (cont == nullptr) return nullptr;

				for (size_t i = Columns::Key; i < std::size(Columns::Captions); i++) {
					wchar_t buf[10]{};
					ListView_GetItemText(hwnd, item, (int)i, buf, sizeof(buf));
					uint32_t u = std::stoul(buf);

					switch (i) {
						case Columns::Key:	cont->unit.key = u; break;
						case Columns::Scene: cont->unit.scene = u; break;
						case Columns::Type: cont->unit.type = static_cast<MIDI::MidiUnitType>(u); break;
						case Columns::Target: cont->unit.target = static_cast<MIDI::Mackie::Target>(u); break;
						case Columns::LongTarget: cont->unit.longtarget = static_cast<MIDI::Mackie::Target>(u); break;
						default: break;
					}
				}
				return cont;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return nullptr;
		}
		bool ListEdit::ListViewEditLabel(int item, int column) {
			if ((!hwndLv__) || (column == Columns::Status)) return false;
			HWND hwnd = hwndLv__.get();

			try {
				bool ischage = false;
				RECT rt{};
				ListView_GetSubItemRect(hwnd, item, column, LVIR_LABEL, &rt);
				InflateRect(&rt, 2, 2);
				LV_COLUMN lvcol{};
				lvcol.mask = LVCF_FMT;
				ListView_GetColumn(hwnd, column, &lvcol);
				DWORD dwAlign;

				if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
					dwAlign = ES_LEFT;
				else if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
					dwAlign = ES_RIGHT;
				else
					dwAlign = ES_CENTER;

				std::wstring wvalue;
				{
					wchar_t txt[50]{};
					ListView_GetItemText(hwnd, item, column, txt, 50);
					wvalue = std::wstring(txt);
				}

				switch (column) {
					case Columns::Key:
					case Columns::Scene: {
						HWND edit = CreateWindow(
							WC_EDIT,
							wvalue.c_str(),
							WS_BORDER | WS_CHILD | WS_VISIBLE | dwAlign,
							rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top,
							hwnd, 0, GetModuleHandle(0), 0);
						if (edit == nullptr) break;

						SetWindowFont(edit, GetWindowFont(hwnd), TRUE);
						SetFocus(edit);
						Edit_SetSel(edit, 0, -1);

						LISTVIEWEDITHDR* phdr = new LISTVIEWEDITHDR;
						phdr->item = item;
						phdr->column = column;
						phdr->IsEscape = FALSE;
						phdr->defProc = (WNDPROC)GetWindowLongPtr(edit, GWLP_WNDPROC);
						(void)SetWindowLongPtr(edit, GWLP_USERDATA, (LONG_PTR)phdr);
						(void)SetWindowLongPtr(edit, GWLP_WNDPROC, (LONG_PTR)EditListViewProc);
						break;
					}
					case Columns::Type: {
						POINT pt{ rt.left, rt.bottom };
						::ClientToScreen(hwnd, &pt);

						MIDI::MidiUnitType type = static_cast<MIDI::MidiUnitType>(std::stoi(wvalue));
						auto data = buildCol3Menu(type);
						if (!std::get<0>(data)) break;
						HMENU hpm = std::get<1>(data);

						int32_t r = ::TrackPopupMenuEx(
							hpm,
							TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
							pt.x,
							pt.y,
							GetParent(hwnd),
							NULL);

						for (auto& m : std::get<2>(data)) ::DestroyMenu(m);

						if (r != 0) {
							r = (r - IDM_COL3_BASE);
							if (r >= 0) {
								std::wstring s = std::to_wstring(r);
								ListView_SetItemText(hwnd, item, column, (LPWSTR)s.c_str());
								ischage = true;
							}
						}
						break;
					}
					case Columns::Target:
					case Columns::LongTarget: {
						POINT pt{ rt.left, rt.bottom };
						::ClientToScreen(hwnd, &pt);

						MIDI::MidiUnitType type;
						{
							wchar_t txt[50]{};
							ListView_GetItemText(hwnd, item, Columns::Type, txt, 50);
							std::wstring ws = std::wstring(txt);
							type = static_cast<MIDI::MidiUnitType>(std::stoi(ws));
						}
						MIDI::Mackie::Target target;
						MIDI::Mackie::Target longtarget;
						{
							if (column == Columns::Target) {
								target = static_cast<MIDI::Mackie::Target>(std::stoi(wvalue));
								longtarget = MIDI::Mackie::Target::NOTARGET;
							}
							else if (column == Columns::LongTarget) {
								wchar_t txt[50]{};
								ListView_GetItemText(hwnd, item, Columns::Target, txt, 50);
								std::wstring ws = std::wstring(txt);
								target = static_cast<MIDI::Mackie::Target>(std::stoi(ws));
								longtarget = static_cast<MIDI::Mackie::Target>(std::stoi(wvalue));
							}
							else break;
						}
						auto data = buildCol4_5Menu(column, type, target, longtarget);
						if (!std::get<0>(data)) break;
						HMENU hpm = std::get<1>(data);

						uint32_t r = ::TrackPopupMenuEx(
							hpm,
							TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
							pt.x,
							pt.y,
							GetParent(hwnd),
							NULL);

						for (auto& m : std::get<2>(data)) ::DestroyMenu(m);

						if (r != 0) {
							r = (r - IDM_COL4_BASE);
							if (r >= 0) {
								std::wstring s = std::to_wstring(r);
								ListView_SetItemText(hwnd, item, column, (LPWSTR)s.c_str());
								ischage = true;
							}
						}
						break;
					}
				}
				if (ischage) {
					try {
						ListMixerContainer* cont = ListViewGetRow(item);
						if (cont != nullptr) {
							LVITEMW lvi{};
							lvi.mask = LVIF_IMAGE;
							lvi.iItem = item;
							lvi.iSubItem = 0;
							ListViewSetImage(lvi, cont);
							(void) ListView_SetItem(hwnd, &lvi);
						}
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
				}
				return true;

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}

		LRESULT CALLBACK ListEdit::EditListViewProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
			if (ListEdit::ctrl__ == nullptr) return 0L;
			LISTVIEWEDITHDR* phdr = (LISTVIEWEDITHDR*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

			try {

				switch (message) {
					case WM_GETDLGCODE: return DLGC_WANTALLKEYS;
					case WM_KEYDOWN: {
						if (wParam == VK_ESCAPE || wParam == VK_RETURN || wParam == VK_TAB) {
							phdr->IsEscape = BOOL(wParam == VK_ESCAPE);
							::SetFocus(GetParent(hwnd));
							return 0L;
						}
						break;
					}
					case WM_KILLFOCUS: {
						if (!ListEdit::ctrl__->hwndLv__) return 0L;
						HWND hwndlv = ListEdit::ctrl__->hwndLv__.get();
						wchar_t pch[260]{};
						Edit_GetText(hwnd, pch, 260);

						if (!phdr->IsEscape) {
							LV_DISPINFO di = { 0 };
							di.hdr.hwndFrom = hwndlv;
							di.hdr.idFrom = GetDlgCtrlID(hwndlv);

							#pragma warning( push )
							#pragma warning( disable : 26454 )
							di.hdr.code = static_cast<UINT>(LVN_ENDLABELEDIT);
							#pragma warning( pop )

							di.item.mask = LVIF_TEXT;
							di.item.iItem = phdr->item;
							di.item.iSubItem = phdr->column;
							di.item.pszText = NULL;
							di.item.pszText = pch;
							di.item.cchTextMax = 260;
							::SendMessageW(GetParent(hwndlv), WM_NOTIFY, (WPARAM)GetDlgCtrlID(hwndlv), (LPARAM)&di);
							try {
								ListMixerContainer* cont = ListEdit::ctrl__->ListViewGetRow(phdr->item);
								if (cont != nullptr) {
									LVITEMW lvi{};
									lvi.mask = LVIF_IMAGE;
									lvi.iItem = phdr->item;
									lvi.iSubItem = 0;
									ListEdit::ctrl__->ListViewSetImage(lvi, cont);
									(void)ListView_SetItem(hwndlv, &lvi);
								}
							}
							catch (...) {
								Utils::get_exception(std::current_exception(), __FUNCTIONW__);
							}
						}
						DestroyWindow(hwnd);
						delete phdr;
						return 0L;
					}
				}
			} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return CallWindowProc(phdr->defProc, hwnd, message, wParam, lParam);
		}
		int CALLBACK ListEdit::ListViewSortEx(LPARAM lp1, LPARAM lp2, LPARAM ldata) {
			if ((ListEdit::ctrl__ == nullptr) || (!ListEdit::ctrl__->hwndLv__)) return 0L;
			HWND hwnd = ListEdit::ctrl__->hwndLv__.get();

			LISTVIEWSORT* data = (LISTVIEWSORT*)ldata;
			if ((!data) || (data->Column() == Columns::Status)) return 0L;

			try {
				wchar_t buf1[20]{}, buf2[20]{};
				ListView_GetItemText(hwnd, lp1, data->Column(), buf1, _countof(buf1));
				ListView_GetItemText(hwnd, lp2, data->Column(), buf2, _countof(buf2));

				int p1 = _wtoi(buf1),
					p2 = _wtoi(buf2);
				return data->Ascending() ? (p1 - p2) : (p2 - p1);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return 0;
		}

		ListEdit* ListEdit::ctrl__ = nullptr;

	}
}