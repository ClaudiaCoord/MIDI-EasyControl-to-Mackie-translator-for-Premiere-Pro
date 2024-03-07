/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"

namespace Common {
	namespace MIDIMT {

		static void setdecimal_(uint32_t n, PROPVARIANT* val) {
			::RtlZeroMemory(val, sizeof(PROPVARIANT));
			val->vt = VT_DECIMAL;
			val->decVal.Lo32 = n;
			val->decVal.Lo64 = n;
		}

		RToolBarDialogEdit::RToolBarDialogEdit(HWND hwnd, std::wstring rname) : RToolBarBase::RToolBarBase(hwnd, rname) {
			build_recent_();
		}
		RToolBarDialogEdit::~RToolBarDialogEdit() {
			recent__.clear();
			RToolBarBase::~RToolBarBase();
		}

		void RToolBarDialogEdit::SetMode() {
			setmode_(IDM_LV_EXT_MODE);
		}

		void RToolBarDialogEdit::update_(UINT32 cmd, REFPROPERTYKEY key, const PROPVARIANT* val, PROPVARIANT* nval) {
			try {
				switch (cmd) {
					case IDM_LV_FILTER_KEY:
					case IDM_LV_FILTER_SCENE:
					case IDM_LV_FILTER_TARGET:
					case IDM_LV_FILTER_TARGETLONG: {
						if (key.fmtid == UI_PKEY_MaxValue.fmtid)
							setdecimal_(255, nval);
						else if (key.fmtid == UI_PKEY_MinValue.fmtid)
							setdecimal_(0, nval);
						else if ((key.fmtid == UI_PKEY_DecimalValue.fmtid) &&
								 ((val->vt == VT_DECIMAL) && ((val->decVal.Lo32 > 255) || (val->decVal.Lo64 > 255))))
							setdecimal_(255, nval);
						return;
					}
					default: break;
				}
				if (key.fmtid == UI_PKEY_Enabled.fmtid) {
					switch (cmd) {
						case IDM_LV_COPY: {
							::UIInitPropertyFromBoolean(UI_PKEY_Enabled, isselected_.load(), nval);
							break;
						}
						case IDM_LV_PASTE: {
							::UIInitPropertyFromBoolean(UI_PKEY_Enabled, iscopied_.load(), nval);
							break;
						}
						case IDM_LV_DELETE:
						case IDM_LV_SET_MQTT:
						case IDM_LV_SET_MMKEY:
						case IDM_LV_SET_MIXER:
						case IDM_LV_SET_VMSCRIPT:
						case IDM_LV_SET_LIGHTKEY8B:
						case IDM_LV_SET_LIGHTKEY16B: {
							::UIInitPropertyFromBoolean(UI_PKEY_Enabled, ispaste_.load(), nval);
							break;
						}
						case IDM_DIALOG_SAVE: {
							::UIInitPropertyFromBoolean(UI_PKEY_Enabled, iseditchange_.load(), nval);
							break;
						}
						case IDM_LV_READ_MIDI_CODE: {
							::UIInitPropertyFromBoolean(UI_PKEY_Enabled, isreadmidienable_.load(), nval);
							break;
						}
						default: break;
					}
				}
				else if (key.fmtid == UI_PKEY_BooleanValue.fmtid) {
					switch (cmd) {
						case IDM_LV_FILTER_TYPEOR: {
							::UIInitPropertyFromBoolean(UI_PKEY_BooleanValue, isfiltertype_.load(), nval);
							break;
						}
						case IDM_LV_FILTER_TYPEAND: {
							::UIInitPropertyFromBoolean(UI_PKEY_BooleanValue, !isfiltertype_.load(), nval);
							break;
						}
						case IDM_LV_FILTER_AUTOCOMMIT: {
							::UIInitPropertyFromBoolean(UI_PKEY_BooleanValue, isautocommit_.load(), nval);
							break;
						}
						case IDM_LV_FILTER_EMBED: {
							::UIInitPropertyFromBoolean(UI_PKEY_BooleanValue, isfilterembed_.load(), nval);
							break;
						}
						case IDM_LV_EDIT_MODE: {
							::UIInitPropertyFromBoolean(UI_PKEY_BooleanValue, iseditdigits_.load(), nval);
							break;
						}
						default: break;
					}
				}
				else if (key.fmtid == UI_PKEY_Minimized.fmtid) {
					if ((val->vt == VT_BOOL) && (val->boolVal == VARIANT_TRUE))
						::UIInitPropertyFromBoolean(UI_PKEY_Minimized, false, nval);
				}
				else if (key.fmtid == UI_PKEY_Viewable.fmtid) {
					if ((val->vt == VT_BOOL) && (val->boolVal == VARIANT_FALSE))
						::UIInitPropertyFromBoolean(UI_PKEY_Viewable, true, nval);
				}
				else if (key.fmtid == UI_PKEY_RecentItems.fmtid) {
					if (cmd == IDM_DIALOG_LAST_OPEN) {
						try {
							safearray_ptr arr = create_safearray_();
							if (arr.empty()) return;
							::UIInitPropertyFromIUnknownArray(UI_PKEY_RecentItems, arr.get(), nval);
						} catch (...) {}
					}
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RToolBarDialogEdit::execute_(UINT32 cmd, const PROPERTYKEY* key, const PROPVARIANT* val, IUISimplePropertySet* s) {
		}

		void RToolBarDialogEdit::build_recent_() {
			try {
				if (!recent__.empty()) recent__.clear();

				common_config& cnf = common_config::Get();
				std::vector<std::wstring>& r = cnf.RecentConfig.GetRecents();
				if (r.empty()) return;

				bool pin = true;
				for (auto& s : r) {
					std::unique_ptr<RecentFile> rf = std::unique_ptr<RecentFile>(new RecentFile(s, pin));
					if (rf->empty()) continue;
					recent__.push_back(std::move(rf));
					pin = false;
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		safearray_ptr RToolBarDialogEdit::create_safearray_() {
			try {
				if (recent__.empty()) return safearray_ptr();

				LONG idx = 0;
				SAFEARRAY* arr = ::SafeArrayCreateVector(VT_UNKNOWN, 0, (ULONG)recent__.size());
				for (auto& a : recent__) {
					if (::SafeArrayPutElement(arr, &idx, static_cast<void*>(a.get())) != S_OK) break;
					idx++;
				}
				return safearray_ptr(arr);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return safearray_ptr();
		}

		#pragma region public getter
		const bool RToolBarDialogEdit::IsReadMidiCheck() {
			return itemgetboolvalue_(IDM_LV_READ_MIDI_CODE);
		}
		const bool RToolBarDialogEdit::IsEditDigitCheck() const {
			return iseditdigits_.load();
		}
		const bool RToolBarDialogEdit::IsFilterEmbedCheck() const {
			return isfilterembed_.load();
		}
		const bool RToolBarDialogEdit::IsFilterTypeCheck() const {
			return isfiltertype_.load();
		}
		const bool RToolBarDialogEdit::IsFilterAutoCommitCheck() const {
			return isautocommit_.load();
		}
		std::wstring RToolBarDialogEdit::GetRecent(uint32_t id) {
			try {
				id -= 1;
				if (recent__.size() <= id) return L"";
				std::unique_ptr<RecentFile>& rf = recent__[id];
				return rf->GetPath();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return L"";
		}
		MIDI::MidiUnit RToolBarDialogEdit::GetFilters() {
			MIDI::MidiUnit mu{};
			try {
				mu.key = static_cast<uint8_t>(itemgetintvalue_(IDM_LV_FILTER_KEY));
				mu.scene = static_cast<uint8_t>(itemgetintvalue_(IDM_LV_FILTER_SCENE));
				mu.target = static_cast<MIDI::Mackie::Target>(itemgetintvalue_(IDM_LV_FILTER_TARGET));
				mu.longtarget = static_cast<MIDI::Mackie::Target>(itemgetintvalue_(IDM_LV_FILTER_TARGETLONG));
				mu.id = static_cast<uint32_t>(isfiltertype_.load());
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return mu;
		}
		#pragma endregion

		#pragma region public setter
		void RToolBarDialogEdit::SetEditorNotify(EditorNotify id) {
			bool renew;
			switch (id) {
				case EditorNotify::ItemEmpty:			{ renew = true; ispaste_ = iscopied_ = isselected_ = false; break; }
				case EditorNotify::ItemFound:			{ renew = !ispaste_; ispaste_ = true; break; }
				case EditorNotify::ValueFound:			{ renew = !iscopied_; iscopied_ = true; break; }
				case EditorNotify::ValueEmpty:			{ renew = iscopied_; iscopied_ = false; break; }
				case EditorNotify::SelectedEnable:		{ renew = !isselected_; isselected_ = true; break; }
				case EditorNotify::SelectedDisable:		{ renew = isselected_; isselected_ = false; break; }
				case EditorNotify::EditChangeEnable:	{ renew = !iseditchange_; iseditchange_ = true; break; }
				case EditorNotify::EditChangeDisable:	{ renew = iseditchange_; iseditchange_ = false; break; }
				case EditorNotify::ReadMidiEnable:		{ renew = !isreadmidienable_; isreadmidienable_ = true; break; }
				case EditorNotify::ReadMidiDisable:		{ renew = isreadmidienable_; isreadmidienable_ = false; break; }
				case EditorNotify::FilterSetOr:			{ renew = true; isfiltertype_ = true; break; }
				case EditorNotify::FilterSetAnd:		{ renew = true; isfiltertype_ = false; break; }
				case EditorNotify::FilterEmbed:			{ renew = true; isfilterembed_ = !isfilterembed_; break; }
				case EditorNotify::AutoCommit:			{ renew = true; isautocommit_ = !isautocommit_; break; }
				case EditorNotify::EditDigits:			{ renew = true; iseditdigits_ = !iseditdigits_; break; }
				default: return;
			}

			if (!renew) return;

			switch (id) {
				case EditorNotify::ItemEmpty:
				case EditorNotify::ItemFound:
				case EditorNotify::ValueFound:
				case EditorNotify::ValueEmpty:
				case EditorNotify::SelectedEnable:
				case EditorNotify::SelectedDisable:
				case EditorNotify::EditChangeEnable:
				case EditorNotify::EditChangeDisable: {
					EnableRefresh();
					break;
				}
				case EditorNotify::ReadMidiEnable:
				case EditorNotify::ReadMidiDisable: {
					ItemEnable(IDM_LV_READ_MIDI_CODE);
					break;
				}
				case EditorNotify::FilterSetOr:
				case EditorNotify::FilterSetAnd: {
					ItemUpdateValue(IDM_LV_FILTER_TYPEOR);
					ItemUpdateValue(IDM_LV_FILTER_TYPEAND);
					break;
				}
				case EditorNotify::FilterEmbed: {
					ItemUpdateValue(IDM_LV_FILTER_EMBED);
					break;
				}
				case EditorNotify::AutoCommit: {
					ItemUpdateValue(IDM_LV_FILTER_AUTOCOMMIT);
					break;
				}
				case EditorNotify::EditDigits: {
					ItemUpdateValue(IDM_LV_EDIT_MODE);
					break;
				}
				default: break;
			}
		}
		void RToolBarDialogEdit::IsReadMidiCheck(bool b) {
			itemsetboolvalue_(IDM_LV_READ_MIDI_CODE, b);
		}
		void RToolBarDialogEdit::SetFilters(MIDI::MidiUnit& mu) {
			try {
				itemsetintvalue_(IDM_LV_FILTER_KEY, static_cast<uint32_t>(mu.key));
				itemsetintvalue_(IDM_LV_FILTER_SCENE, static_cast<uint32_t>(mu.scene));
				itemsetintvalue_(IDM_LV_FILTER_TARGET, static_cast<uint32_t>(mu.target));
				itemsetintvalue_(IDM_LV_FILTER_TARGETLONG, static_cast<uint32_t>(mu.longtarget));
				bool type = static_cast<bool>(mu.id);
				itemsetboolvalue_(IDM_LV_FILTER_TYPEOR,   type);
				itemsetboolvalue_(IDM_LV_FILTER_TYPEAND, !type);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RToolBarDialogEdit::AddRecent(std::wstring& s) {
			try {
				std::unique_ptr<RecentFile> rf = std::unique_ptr<RecentFile>(new RecentFile(s, true));
				if (rf->empty()) return;
				recent__.push_back(std::move(rf));
				common_config::Get().RecentConfig.Add(s);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		void RToolBarDialogEdit::EnableRefresh() {
			const uint32_t list[]{
				IDM_DIALOG_SAVE,
				IDM_LV_READ_MIDI_CODE,
				IDM_LV_COPY,
				IDM_LV_PASTE,
				IDM_LV_DELETE,
				IDM_LV_SET_MQTT,
				IDM_LV_SET_MMKEY,
				IDM_LV_SET_MIXER,
				IDM_LV_SET_VMSCRIPT,
				IDM_LV_SET_LIGHTKEY8B,
				IDM_LV_SET_LIGHTKEY16B
			};
			for (uint32_t i : list) ItemEnable(i);
		}
	}
}
