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

		#pragma region Columns::
		const wchar_t* Columns::Captions[] { L"...", L"id", L"scene", L"type", L"target", L"long-target"};
		const int Columns::ColumnSize[] { 26, 40, 40, 35, 69, 69 };
		const int Columns::ColumnFormat[] { LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_LEFT };
		const int Columns::Status = 0;
		const int Columns::Key = 1;
		const int Columns::Scene = 2;
		const int Columns::Type = 3;
		const int Columns::Target = 4;
		const int Columns::LongTarget = 5;
		const int Columns::ViewGroup = 10;
		const int Columns::HiddenGroup = 20;
		#pragma endregion

		#pragma region ListMixerContainer
		ListMixerContainer::ListMixerContainer() {
			unit.key = 255U;
			unit.scene = 255U;
			unit.type = MIDI::MidiUnitType::UNITNONE;
			unit.target = MIDI::Mackie::Target::NOTARGET;
			unit.longtarget = MIDI::Mackie::Target::NOTARGET;
		}
		ListMixerContainer::ListMixerContainer(ListMixerContainer* c) {
			if (c) unit.Copy(c->unit);
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
		#pragma endregion

		#pragma region LISTVIEWEDITHDR
		typedef struct tag_listviewedithdr {
			int item;
			int column;
			bool IsEscape;
		} LISTVIEWEDITHDR;
		#pragma endregion

		#pragma region LISTVIEWSORT
		LISTVIEWSORT::LISTVIEWSORT() : column(-1) {
		}
		const bool LISTVIEWSORT::CheckColumn(int col) {
			return (col >= 0) && (col <= Columns::LongTarget);
		}
		bool LISTVIEWSORT::operator==(const LISTVIEWSORT& x) {
			return (column != -1);
		}
		int  LISTVIEWSORT::Column() const { return (column == -1) ? 0 : column; }
		bool LISTVIEWSORT::Ascending() { return (column == -1) ? false : ascending[column]; }
		void LISTVIEWSORT::Set(int col) {
			column = CheckColumn(col) ? col : -1;
			if (column != -1)
				ascending[col] = !ascending[col];
		}
		void LISTVIEWSORT::Set(int col, bool asc) {
			column = CheckColumn(col) ? col : -1;
			if (column != -1)
				ascending[col] = asc;
		}
		void LISTVIEWSORT::Reset() {
			column = -1;
		}
		#pragma endregion

		#pragma region LISTVIEWPASTE
		LISTVIEWPASTE::LISTVIEWPASTE() : item(-1), column(-1), cont(nullptr) {
		}
		const bool LISTVIEWPASTE::operator==(const LISTVIEWPASTE& x) { return IsItemEmpty(); }
		const bool LISTVIEWPASTE::IsValueEmpty() {
			return (cont == nullptr);
		}
		const bool LISTVIEWPASTE::IsValuesEmpty() {
			return ((cont == nullptr) || (cont->unit.key >= 255) || (cont->unit.scene >= 255U) || (cont->unit.type == MIDI::MidiUnitType::UNITNONE));
		}
		const bool LISTVIEWPASTE::IsItemEmpty() { return (item == -1) || (column == -1); }
		int LISTVIEWPASTE::Item() const { return item; }
		int LISTVIEWPASTE::Column() const { return column; }
		ListMixerContainer* LISTVIEWPASTE::Values() const { return cont; }
		void LISTVIEWPASTE::SetItem(HWND hwnd, int i, int c) {
			item = i, column = c;
			sendnotify_(hwnd, EditorNotify::ItemFound);
		}
		void LISTVIEWPASTE::SetValue(HWND hwnd, ListMixerContainer* c) {
			cont = c;
			sendnotify_(hwnd, (c) ? EditorNotify::ValueFound : EditorNotify::ValueEmpty);
		}
		void LISTVIEWPASTE::Reset(HWND hwnd) {
			cont = nullptr;
			item = column = -1;
			if (hwnd) sendnotify_(hwnd, EditorNotify::ItemEmpty);
		}
		void LISTVIEWPASTE::sendnotify_(HWND hwnd, EditorNotify id) {
			if (hwnd)
				(void) ::PostMessageW(hwnd, WM_COMMAND, MAKELONG(IDM_LV_PASTE_NOTIFY, static_cast<uint16_t>(id)), 0);
		}
		#pragma endregion

		#pragma region Static local
		static std::tuple<bool, HMENU, std::vector<HMENU>> buildContextMenu(bool ispaste) {
			try {
				std::vector<HMENU> v;
				do {
					HMENU hm, hmp;
					if ((hm = LangInterface::Get().GetMenu(MAKEINTRESOURCE(IDR_LV_MENU))) == nullptr) break;
					v.push_back(hm);
					if ((hmp = ::GetSubMenu(hm, 0)) == nullptr) break;

					bool isbrun = common_config::Get().Local.IsMidiBridgeRun();

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
									case MIDI::Mackie::Target::MQTTKEY:   mid = IDR_COL5_BTN_SM_MENU; break;
									case MIDI::Mackie::Target::NOTARGET:
									default: mid = IDR_COL4_BTN_TARGET_MENU; break;
								}
								break;
							}
							case MIDI::MidiUnitType::UNITNONE:  mid = IDR_COL4_ALL_TARGET_MENU; break;
							default: /* SLISERS */ {
								switch (target) {
									case MIDI::Mackie::Target::VOLUMEMIX: mid = IDR_COL5_SLIDER_MIX_MENU; break;
									case MIDI::Mackie::Target::MQTTKEY:   mid = IDR_COL5_SLIDER_SM_MENU; break;
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
		#pragma endregion

		/* Class */

		ListEdit::ListEdit() : icons__(nullptr), ErrorFn([](std::wstring){}) {
			icons__ = ::ImageList_Create(16, 16, ILC_MASK | ILC_ORIGINALSIZE, 1, 1);

			for (int32_t i = IDI_ICON_LV1; i <= IDI_ICON_LV7; i++) {
				HICON ico = LangInterface::Get().GetIcon16x16(MAKEINTRESOURCE(i));
				if (ico != nullptr) {
					ImageList_AddIcon(icons__, ico);
					::DestroyIcon(ico);
				}
			}
		}
		ListEdit::~ListEdit() {
			dispose_();
			HIMAGELIST icons = icons__;
			icons__ = nullptr;
			if (icons != nullptr)
				ImageList_Destroy(icons);
		}
		void ListEdit::dispose_() {
			if (!hwndLv__) return;
			HWND hwnd = hwndLv__.release();
			try {
				listview_clearlist_(hwnd);
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
			dispose_();
		}
		void ListEdit::ListViewInit(HWND hwnd) {
			hwndLv__.reset(hwnd);

			try {
				lvsort = std::make_shared<LISTVIEWSORT>();
				lvpaste = std::make_shared<LISTVIEWPASTE>();

				(void) ListView_SetExtendedListViewStyle(hwnd,
					ListView_GetExtendedListViewStyle(hwnd) | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);

				(void) ListView_SetInsertMarkColor(hwnd, RGB(50,00,00));
				(void) ListView_SetImageList(hwnd, icons__, LVSIL_SMALL);

				LVGROUP lvg{};
				lvg.cbSize = sizeof(LVGROUP);

				lvg.iGroupId = Columns::HiddenGroup;
				lvg.mask = LVGS_HIDDEN | LVGS_NOHEADER | LVGS_COLLAPSED;
				lvg.stateMask = LVGS_HIDDEN | LVGS_NOHEADER | LVGS_COLLAPSED;
				lvg.state = LVGS_HIDDEN | LVGS_NOHEADER | LVGS_COLLAPSED;
				ListView_InsertGroup(hwnd, -1, &lvg);

				lvg.iGroupId = Columns::ViewGroup;
				lvg.mask = LVGF_HEADER | LVGF_GROUPID;
				lvg.stateMask = 0;
				lvg.state = 0;
				ListView_InsertGroup(hwnd, -1, &lvg);

				LVCOLUMNW lvc{};
				lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

				for (size_t i = 0; i < std::size(Columns::Captions); i++) {

					lvc.cx = Columns::ColumnSize[i];
					lvc.fmt = Columns::ColumnFormat[i];
					lvc.iSubItem = static_cast<int>(i);
					lvc.pszText = (LPWSTR)Columns::Captions[i];

					if (ListView_InsertColumn(hwnd, i, &lvc) == -1) return;
				}
				ListView_EnableGroupView(hwnd, true);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void ListEdit::ListViewFilterEmbed(bool b) {
			if (!hwndLv__) return;
			try {
				HWND head = ListView_GetHeader(hwndLv__);
				if (!head) return;
				LONG_PTR styles = GetWindowLongPtr(head, GWL_STYLE);
				SetWindowLongPtr(head, GWL_STYLE, (b) ? (styles | HDS_FILTERBAR) : (styles & ~HDS_FILTERBAR));

				for (size_t i = 0; i < std::size(Columns::Captions); i++) {

					if (b) {
						HDITEMW	hditem{};
						hditem.mask = HDI_FILTER;
						hditem.type = (i == 0) ? HDFT_HASNOVALUE : HDFT_ISNUMBER;
						(void)Header_SetItem(head, i, &hditem);

						hditem.mask = HDI_WIDTH;
						hditem.cxy = Columns::ColumnSize[i];
						(void)Header_SetItem(head, i, &hditem);

						hditem.mask = HDI_HEIGHT;
						hditem.cxy = 12;
						(void)Header_SetItem(head, i, &hditem);

						hditem.mask = HDI_FORMAT;
						hditem.fmt = Columns::ColumnFormat[i];
						(void)Header_SetItem(head, i, &hditem);
					}

					LVCOLUMNW lvc{};
					lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
					lvc.cx = Columns::ColumnSize[i];
					lvc.fmt = Columns::ColumnFormat[i];
					lvc.iSubItem = static_cast<int>(i);
					lvc.pszText = (LPWSTR)Columns::Captions[i];

					ListView_SetColumn(hwndLv__, i, &lvc);
				}
				if (b) (void)::SendMessageW(hwndLv__, HDM_SETFILTERCHANGETIMEOUT, 0, 1500);

				lvsort->Set(Columns::Key, true);
				ListView_SortItemsEx(hwndLv__, &listview_sortex_, (LPARAM)this);
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
					listview_setrow_(new ListMixerContainer(a));
				}
				lvsort->Set(Columns::Key, true);
				ListView_SortItemsEx(hwnd, &listview_sortex_, (LPARAM)this);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void ListEdit::ListViewFiltersReset() {
			if (!hwndLv__) return;
			try {
				HWND head = ListView_GetHeader(hwndLv__);
				if (!head) return;
				(void) Header_ClearAllFilters(head);

				LONG value{ 0L };
				HDITEMW	hditem{};
				hditem.mask = HDI_FILTER;
				hditem.type = HDFT_ISNUMBER;
				hditem.pvFilter = &value;

				for (size_t i = 1; i < std::size(Columns::Captions); i++)
					(void)Header_SetItem(head, i, &hditem);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		LONG ListEdit::ListViewFilter(bool filtermode) {
			if (!hwndLv__) return MAKELONG(-1, 0);
			HWND hwnd = hwndLv__.get();

			try {
				HWND head = ListView_GetHeader(hwndLv__);
				if (!head) return MAKELONG(-1, 0);

				LONG value{ 0L };
				HDITEMW	hditem{};
				hditem.mask = HDI_FILTER;
				hditem.type = HDFT_ISNUMBER;
				hditem.pvFilter = &value;

				MIDI::MixerUnit mu{};
				for (uint32_t i = 1; i <= 5; i++) {
					value = 0L;
					if (!Header_GetItem(head, i, &hditem)) continue;
					uint8_t val = static_cast<uint8_t>(value);
					switch (i) {
						case 1: mu.key = val; break;
						case 2: mu.scene = val; break;
						case 3: mu.type = static_cast<MIDI::MidiUnitType>((uint16_t)val); break;
						case 4: mu.target = static_cast<MIDI::Mackie::Target>((uint16_t)val); break;
						case 5: mu.longtarget = static_cast<MIDI::Mackie::Target>((uint16_t)val); break;
					}
				}
				return ListViewFilter(mu, filtermode);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return MAKELONG(-1, 0);
		}
		LONG ListEdit::ListViewFilter(MIDI::MixerUnit& unit, bool filtermode) {
			if (!hwndLv__) return MAKELONG(-1, 0);
			HWND hwnd = hwndLv__.get();

			try {
				int16_t count = 0, all = 0, pos = -1;
				if ((all = ListView_GetItemCount(hwnd)) == 0) return MAKELONG(0, 0);

				bool isclean = (unit.key == 0) && (unit.scene == 0) && (unit.target == 0) && (unit.longtarget == 0);
				while ((pos = ListView_GetNextItem(hwnd, pos, LVNI_ALL)) != -1) {
					try {
						ListMixerContainer* c = listview_getrow_(pos);
						if (c == nullptr) continue;

						bool visible = isclean ? true :
							(filtermode ? unit.EqualsOR(c->unit) : unit.EqualsAND(c->unit));

						LVITEMW lvi{};
						lvi.mask = LVIF_GROUPID;
						lvi.iItem = pos;
						lvi.iGroupId = visible ? Columns::ViewGroup : Columns::HiddenGroup;
						(void) ListView_SetItem(hwnd, &lvi);

						count = visible ? (count + 1) : count;

					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
				}
				return MAKELONG(all, count);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return MAKELONG(-1, 0);
		}
		bool ListEdit::ListViewSort(LPNMHDR lpmh) {
			if ((lpmh == nullptr) || (lvsort == nullptr)) return false;
			NM_LISTVIEW* pnm = (NM_LISTVIEW*)lpmh;
			lvsort.get()->Set(pnm->iSubItem);
			ListView_SortItemsEx(pnm->hdr.hwndFrom, &listview_sortex_, (LPARAM)this);
			return true;
		}
		bool ListEdit::ListViewSort(uint32_t col, bool b) {
			if (!hwndLv__ || (lvsort == nullptr) || (col > 5)) return false;
			lvsort.get()->Set(col, b);
			ListView_SortItemsEx(hwndLv__.get(), &listview_sortex_, (LPARAM)this);
			return true;
		}
		bool ListEdit::ListViewMenu(LPNMHDR lpmh) {
			if (!hwndLv__ || (lpmh == nullptr) || (lvpaste == nullptr)) return false;
			HWND hwnd = hwndLv__.get();

			try {
				HWND dlg = ::GetParent(hwnd);
				NMLISTVIEW* pnm = (NM_LISTVIEW*)lpmh;
				lvpaste.get()->SetItem(dlg, pnm->iItem, pnm->iSubItem);

				auto data = buildContextMenu(!lvpaste.get()->IsValuesEmpty());
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
				HWND dlg = ::GetParent(hwnd);

				switch (id) {
					case IDM_LV_COPY: {
						ListMixerContainer* cont = listview_getrow_(lvpaste->Item());
						if ((cont == nullptr) || (cont->unit.key >= 255) || (cont->unit.scene >= 255U) || (cont->unit.type == MIDI::MidiUnitType::UNITNONE)) {
							lvpaste->Reset(dlg);
							break;
						}
						lvpaste->SetValue(dlg, cont);
						break;
					}
					case IDM_LV_NEW: {
						ListMixerContainer* cont = new ListMixerContainer();
						int32_t item = ListViewInsertItem(cont);
						if (item != -1) {
							lvpaste->Reset(dlg);
							lvpaste->SetItem(dlg, item, 0);
							(void) ListView_EnsureVisible(hwnd, item, true);
						}
						break;
					}
					case IDM_LV_PASTE: {
						if (lvpaste->IsValuesEmpty()) {
							lvpaste->Reset(dlg);
							break;
						}
						ListMixerContainer* cont = new ListMixerContainer(lvpaste->Values());
						int32_t item = ListViewInsertItem(cont);
						if (item != -1) {
							lvpaste->SetItem(dlg, item, 0);
							lvpaste->SetValue(dlg, cont);
							(void)ListView_EnsureVisible(hwnd, item, true);
						}
						break;
					}
					case IDM_LV_DELETE: {
						listview_deleteitem_(hwnd, lvpaste->Item());
						lvpaste->Reset(dlg);
						break;
					}
					case IDM_LV_SET_MQTT:
					case IDM_LV_SET_MMKEY:
					case IDM_LV_SET_MIXER: 
					case IDM_LV_SET_LIGHTKEY: {
						uint32_t val;
						switch (id) {
							case IDM_LV_SET_MQTT:		val = static_cast<uint32_t>(MIDI::Mackie::Target::MQTTKEY);   break;
							case IDM_LV_SET_MMKEY:		val = static_cast<uint32_t>(MIDI::Mackie::Target::MEDIAKEY);  break;
							case IDM_LV_SET_MIXER:		val = static_cast<uint32_t>(MIDI::Mackie::Target::VOLUMEMIX); break;
							case IDM_LV_SET_LIGHTKEY:	val = static_cast<uint32_t>(MIDI::Mackie::Target::LIGHTKEY);  break;
							default: return false;
						}
						std::wstring ws = std::to_wstring(val);
						ListView_SetItemText(hwnd, lvpaste->Item(), Columns::Target, (LPWSTR)ws.c_str());
						listview_updateimage_(hwnd, lvpaste->Item());
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
				listview_editlabel_(pia->iItem, pia->iSubItem);
			else
				ListView_SetItemState(lpmh->hwndFrom, pia->iItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			return true;
		}

		int ListEdit::ListViewInsertItem(ListMixerContainer *cont) {
			if (!hwndLv__) return -1;
			HWND hwnd = hwndLv__.get();

			try {
				int32_t pos = listview_setrow_(cont);
				if (pos == -1)
					lvpaste->Reset(::GetParent(hwnd));
				else
					ListView_SetItemState(hwnd, pos, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				return pos;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return -1;
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
					try {
						ListMixerContainer* cont = listview_getrow_(pos);
						if ((cont == nullptr) ||
							(cont->unit.key >= 255) ||
							(cont->unit.scene >= 255U) ||
							(cont->unit.type == MIDI::MidiUnitType::UNITNONE)) continue;

						dev->units.push_back(std::move(cont->GetMidiUnit()));
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
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
				if (ListView_GetItemState(lpmh->hwndFrom, pia->iItem, LVIS_FOCUSED) & LVIS_FOCUSED) {
					if (lvpaste) lvpaste.get()->SetItem(::GetParent(lpmh->hwndFrom), pia->iItem, pia->iSubItem);
					return listview_getrow_(pia->iItem);
				}
			} while (0);
			return nullptr;
		}
		int ListEdit::ListViewCount() {
			if (!hwndLv__) return 0;
			return ListView_GetItemCount(hwndLv__.get());
		}

		/* Private */

		void ListEdit::listview_deleteitem_(HWND hwnd, int pos) {
			try {
				LVITEMW lvi{};
				lvi.mask = LVIF_PARAM;
				lvi.iItem = pos;

				if (ListView_GetItem(hwnd, &lvi)) {
					ListMixerContainer* cont = reinterpret_cast<ListMixerContainer*>(lvi.lParam);
					lvi.lParam = (LPARAM)nullptr;
					(void) ListView_SetItem(hwnd, &lvi);
					(void) ListView_DeleteItem(hwnd, pos);

					if (cont) delete cont;
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void ListEdit::listview_clearlist_(HWND hwnd) {
			try {
				int count = ListView_GetItemCount(hwnd);
				if (count == 0) return;
				lvpaste->Reset(nullptr);
				lvsort->Reset();

				int pos = -1;
				while ((pos = ListView_GetNextItem(hwnd, pos, LVNI_ALL)) != -1) {
					try {
						LVITEMW lvi{};
						lvi.mask = LVIF_PARAM;
						lvi.iItem = pos;

						if (ListView_GetItem(hwnd, &lvi)) {
							ListMixerContainer* cont = reinterpret_cast<ListMixerContainer*>(lvi.lParam);

							lvi.lParam = (LPARAM)nullptr;
							(void)ListView_SetItem(hwnd, &lvi);
							if (cont) delete cont;
						}
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
				}
				(void) ListView_DeleteAllItems(hwnd);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		int  ListEdit::listview_setrow_(ListMixerContainer* cont, int id) {
			if (!hwndLv__) return -1;
			HWND hwnd = hwndLv__.get();

			try {
				int item = (id == -1) ? 0 : id;

				for (size_t i = 0; i < std::size(Columns::Captions); i++) {
					LVITEMW lvi{};

					if (i == 0) {
						listview_setimage_(lvi, cont);
						lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_DI_SETITEM | LVIF_PARAM | LVIF_GROUPID;
						lvi.lParam = (LPARAM)cont;
						lvi.iGroupId = Columns::ViewGroup;
					} else {
						lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_DI_SETITEM;
					}

					std::wstring ws;
					switch (i) {
						case Columns::Status:		break;
						case Columns::Key:			ws = std::to_wstring(cont->unit.key); break;
						case Columns::Scene:		ws = std::to_wstring(cont->unit.scene); break;
						case Columns::Type:			ws = std::to_wstring(cont->unit.type); break;
						case Columns::Target:		ws = std::to_wstring(cont->unit.target); break;
						case Columns::LongTarget:	ws = std::to_wstring(cont->unit.longtarget); break;
						default: break;
					}
					lvi.iItem = item;
					lvi.iSubItem = static_cast<int>(i);
					lvi.pszText = (LPWSTR)ws.c_str();

					if ((i == 0) && (id == -1)) {
						if ((item = ListView_InsertItem(hwnd, &lvi)) == -1) break;
					} else (void) ListView_SetItem(hwnd, &lvi);
				}
				return item;
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return -1;
		}
		ListMixerContainer* ListEdit::listview_getrow_(int item) {
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
				if (!cont) return nullptr;

				for (size_t i = Columns::Key; i < std::size(Columns::Captions); i++) {
					wchar_t buf[10]{};
					ListView_GetItemText(hwnd, item, (int)i, buf, sizeof(buf));
					uint32_t u = std::stoul(buf);

					switch (i) {
						case Columns::Key:			cont->unit.key = u; break;
						case Columns::Scene:		cont->unit.scene = u; break;
						case Columns::Type:			cont->unit.type = static_cast<MIDI::MidiUnitType>(u); break;
						case Columns::Target:		cont->unit.target = static_cast<MIDI::Mackie::Target>(u); break;
						case Columns::LongTarget:	cont->unit.longtarget = static_cast<MIDI::Mackie::Target>(u); break;
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

		void ListEdit::listview_digitallabel_(int item, int column, RECT& rt, std::wstring& value) {
			try {
				HWND hwnd = hwndLv__.get();

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

				HWND edit = CreateWindowExW(0L, WC_EDIT, value.c_str(),
					WS_BORDER | WS_CHILD | WS_VISIBLE | dwAlign,
					rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top,
					hwnd, 0, LangInterface::Get().GetMainHinstance(), 0);
				if (!edit) return;

				SetWindowFont(edit, GetWindowFont(hwnd), TRUE);
				::SetFocus(edit);
				Edit_SetSel(edit, 0, -1);

				LISTVIEWEDITHDR* phdr = new LISTVIEWEDITHDR();
				phdr->item = item;
				phdr->column = column;
				phdr->IsEscape = FALSE;
				(void)SetWindowLongPtr(edit, GWLP_USERDATA, (LONG_PTR)phdr);
				::SetWindowSubclass(edit, (SUBCLASSPROC)&listview_sub_editproc_, 1, (DWORD_PTR)this);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		bool ListEdit::listview_editlabel_(int item, int column) {
			if ((!hwndLv__) || (column == Columns::Status)) return false;
			HWND hwnd = hwndLv__.get();

			try {
				bool ischage = false;
				RECT rt{};
				ListView_GetSubItemRect(hwnd, item, column, LVIR_LABEL, &rt);
				::InflateRect(&rt, 2, 2);

				bool isdigitedit = EditAsDigit.load();
				std::wstring wvalue;
				{
					wchar_t txt[50]{};
					ListView_GetItemText(hwnd, item, column, txt, 50);
					wvalue = std::wstring(txt);
				}
				if (!isdigitedit && (column == Columns::LongTarget)) {
					try {
						LVITEMW lvi{};
						lvi.mask = LVIF_PARAM;
						lvi.iItem = item;

						if (ListView_GetItem(hwnd, &lvi)) {
							ListMixerContainer* cont = reinterpret_cast<ListMixerContainer*>(lvi.lParam);
							if (cont) isdigitedit = cont->unit.target == MIDI::Mackie::Target::LIGHTKEY;
						}
					} catch (...) {}
				}
				switch (isdigitedit ? Columns::Key : column) {
					case Columns::Key:
					case Columns::Scene: {
						listview_digitallabel_(item, column, rt, wvalue);
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
				if (ischage)
					listview_updateimage_(hwnd, item);
				return true;

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}

		void ListEdit::listview_setimage_(LVITEMW& lvi, ListMixerContainer* cont) {
			if ((cont->unit.key == 255U) || (cont->unit.scene == 255U)) {
				lvi.iImage = 1;
			}
			else {
				switch (cont->unit.target) {
					using enum MIDI::Mackie::Target;
					case VOLUMEMIX: lvi.iImage = 3; break;
					case MEDIAKEY:  lvi.iImage = 4; break;
					case MQTTKEY:   lvi.iImage = 5; break;
					case LIGHTKEY:  lvi.iImage = 6; break;
					case NOTARGET:  lvi.iImage = 0; break;
					default:	    lvi.iImage = 2; break;
				}
			}
		}
		void ListEdit::listview_updateimage_(HWND hwnd, int32_t item) {
			try {
				ListMixerContainer* cont = listview_getrow_(item);
				if (cont) {
					LVITEMW lvi{};
					lvi.mask = LVIF_IMAGE;
					lvi.iItem = item;
					lvi.iSubItem = 0;
					listview_setimage_(lvi, cont);
					(void)ListView_SetItem(hwnd, &lvi);
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		int CALLBACK ListEdit::listview_sortex_(LPARAM lp1, LPARAM lp2, LPARAM data) {
			ListEdit* ctrl__ = reinterpret_cast<ListEdit*>(data);
			if ((ctrl__ == nullptr) || (!ctrl__->hwndLv__)) return 0L;

			LISTVIEWSORT* lvsort = ctrl__->lvsort.get();
			if ((!lvsort) || (lvsort->Column() == Columns::Status)) return 0L;

			HWND hwnd = ctrl__->hwndLv__.get();
			if (!hwnd) return 0L;

			try {
				wchar_t buf1[20]{}, buf2[20]{};
				ListView_GetItemText(hwnd, lp1, lvsort->Column(), buf1, _countof(buf1));
				ListView_GetItemText(hwnd, lp2, lvsort->Column(), buf2, _countof(buf2));

				int p1 = _wtoi(buf1),
					p2 = _wtoi(buf2);
				return lvsort->Ascending() ? (p1 - p2) : (p2 - p1);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return 0;
		}
		LRESULT CALLBACK ListEdit::listview_sub_editproc_(HWND hwnd, UINT m, WPARAM w, LPARAM l, UINT_PTR sc, DWORD_PTR data) {
			ListEdit* ctrl__ = reinterpret_cast<ListEdit*>(data);
			if (ctrl__ == nullptr) return 0L;
			LISTVIEWEDITHDR* phdr = (LISTVIEWEDITHDR*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

			try {
				switch (m) {
					case WM_GETDLGCODE: return DLGC_WANTALLKEYS;
					case WM_KEYDOWN: {
						if (w == VK_ESCAPE || w == VK_RETURN || w == VK_TAB) {
							phdr->IsEscape = BOOL(w == VK_ESCAPE);
							::SetFocus(::GetParent(hwnd));
							return 0L;
						}
						break;
					}
					case WM_KILLFOCUS: {
						if (!ctrl__->hwndLv__) return 0L;
						HWND hwndlv = ctrl__->hwndLv__.get();
						wchar_t pch[260]{};
						Edit_GetText(hwnd, pch, 260);

						if (!phdr->IsEscape) {
							LV_DISPINFO di = { 0 };
							di.hdr.hwndFrom = hwndlv;
							di.hdr.idFrom = ::GetDlgCtrlID(hwndlv);

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
							::SendMessageW(GetParent(hwndlv), WM_NOTIFY, (WPARAM)::GetDlgCtrlID(hwndlv), (LPARAM)&di);
							ctrl__->listview_updateimage_(hwndlv, phdr->item);
						}
						::RemoveWindowSubclass(hwnd, (SUBCLASSPROC)&listview_sub_editproc_, 1);
						::DestroyWindow(hwnd);
						delete phdr;
						return 0L;
					}
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return ::DefSubclassProc(hwnd, m, w, l);
		}
	}
}