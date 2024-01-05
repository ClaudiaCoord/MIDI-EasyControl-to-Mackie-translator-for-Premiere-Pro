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

		/*
			DEFINE_GUID(IID_IUIFRAMEWORK, 0xf4f0385d, 0x6872, 0x43a8, 0xad, 0x09, 0x4c, 0x33, 0x9c, 0xb3, 0xf5, 0xc5);
			DEFINE_GUID(IID_IUIAPPLICATION, 0xd428903c, 0x729a, 0x491d, 0x91, 0x0d, 0x68, 0x2a, 0x08, 0xff, 0x25, 0x22);
		*/

		/* safearray_ptr */
		safearray_ptr::safearray_ptr(SAFEARRAY* a) : arr_(a) {
		}
		safearray_ptr::~safearray_ptr() {
			try {
				SAFEARRAY* a = arr_;
				arr_ = nullptr;
				if (a) ::SafeArrayDestroy(a);
			} catch (...) {}
		}
		const bool safearray_ptr::empty() const {
			return arr_ == nullptr;
		}
		SAFEARRAY* safearray_ptr::get() const {
			return arr_;
		}

		/* RecentFiles */
		RecentFile::RecentFile(std::wstring& path, const bool pin) {
			try {
				std::filesystem::path p(path);
				if (std::filesystem::exists(p)) {
					path__ = ::SysAllocString(p.wstring().data());
					name__ = ::SysAllocString(p.stem().wstring().data());
					last__ = pin;
				}
			} catch (...) {}
		}
		RecentFile::~RecentFile() {
			dispose();
		}
		void RecentFile::dispose() {
			try {
				BSTR s = path__;
				path__ = nullptr;
				if (s) ::SysFreeString(s);
				s = name__;
				name__ = nullptr;
				if (s) ::SysFreeString(s);
			} catch (...) {}
		}
		const bool RecentFile::empty() const {
			return path__ == nullptr;
		}
		std::wstring RecentFile::GetPath() {
			try {
				if (!path__) return L"";
				return std::wstring(path__);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return L"";
		}
		HRESULT STDMETHODCALLTYPE RecentFile::GetValue(__in REFPROPERTYKEY key, __out PROPVARIANT* pvar) {
			HRESULT hr;
			if (key == UI_PKEY_Label)
				hr = ::UIInitPropertyFromString(key, name__, pvar);
			else if (key == UI_PKEY_LabelDescription)
				hr = ::UIInitPropertyFromString(key, path__, pvar);
			else if (key == UI_PKEY_Pinned) {
				pvar->vt = VT_BOOL;
				pvar->boolVal = last__ ? VARIANT_TRUE : VARIANT_FALSE;
				hr = S_OK;
			}
			else
				hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
			return hr;
		}

		#pragma region COM Interface
		HRESULT STDMETHODCALLTYPE RecentFile::QueryInterface(REFIID riid, void** pobj) {
			if (!pobj) {
				return E_INVALIDARG;
			}
			if (riid == IID_IUnknown) {
				*pobj = static_cast<IUnknown*>(
					static_cast<IUnknown*>(this));
			}
			else if (riid == __uuidof(IUISimplePropertySet)) {
				*pobj = static_cast<IUISimplePropertySet*>(this);
			}
			else {
				*pobj = 0;
				return E_NOINTERFACE;
			}
			AddRef();
			return S_OK;
		}
		ULONG   STDMETHODCALLTYPE RecentFile::AddRef() {
			return InterlockedIncrement(&rcnt__);
		}
		ULONG   STDMETHODCALLTYPE RecentFile::Release() {
			return InterlockedDecrement(&rcnt__);
		}
		#pragma endregion

		/* RToolBarBase */
		RToolBarBase::RToolBarBase(HWND hwnd, std::wstring rname) : name__(rname) {
			hwnd__.reset(hwnd);
			init_(rname);
		}
		RToolBarBase::~RToolBarBase() {
			dispose_();
		}
		const bool	RToolBarBase::IsInit() const {
			return isinit;
		}
		void		RToolBarBase::ItemEnable(uint32_t id) {
			itemupdate_(id, UI_INVALIDATIONS_STATE);
		}
		void		RToolBarBase::ItemUpdateValue(uint32_t id) {
			itemupdate_(id, UI_INVALIDATIONS_VALUE);
		}
		void		RToolBarBase::ItemUpdateProperty(uint32_t id) {
			itemupdate_(id, UI_INVALIDATIONS_PROPERTY);
		}
		void		RToolBarBase::ItemSetBoolValue(uint32_t id, bool b) {
			itemsetboolvalue_(id, b);
		}
		bool		RToolBarBase::ItemGetBoolValue(uint32_t id) {
			return itemgetboolvalue_(id);
		}
		void		RToolBarBase::ItemSetIntValue(uint32_t id, uint32_t v) {
			itemsetintvalue_(id, v);
		}
		uint32_t	RToolBarBase::ItemGetIntValue(uint32_t id) {
			return itemgetintvalue_(id);
		}

		#pragma region COM Interface
		HRESULT STDMETHODCALLTYPE RToolBarBase::QueryInterface(REFIID riid, void** pobj) {
			if (!pobj) {
				return E_INVALIDARG;
			}
			if (riid == IID_IUnknown) {
				*pobj = static_cast<IUnknown*>(
					static_cast<IUIApplication*>(this));
			}
			else if (riid == __uuidof(IUICommandHandler)) {
				*pobj = static_cast<IUICommandHandler*>(this);
			}
			else if (riid == __uuidof(IUIApplication)) {
				*pobj = static_cast<IUIApplication*>(this);
			}
			else {
				*pobj = 0;
				return E_NOINTERFACE;
			}
			AddRef();
			return S_OK;
		}
		ULONG   STDMETHODCALLTYPE RToolBarBase::AddRef() {
			return InterlockedIncrement(&rcnt__);
		}
		ULONG   STDMETHODCALLTYPE RToolBarBase::Release() {
			return InterlockedDecrement(&rcnt__);
		}
		#pragma endregion

		#pragma region IUIApplication Interface
		HRESULT STDMETHODCALLTYPE RToolBarBase::OnViewChanged(uint32_t, UI_VIEWTYPE, IUnknown* pv, UI_VIEWVERB verb, int32_t) {
			HRESULT hr = E_NOTIMPL;

			switch (verb) {
				case UI_VIEWVERB_CREATE: {
					IUIRibbon* rb{ nullptr };
					if ((hr = pv->QueryInterface(IID_PPV_ARGS(&rb))) != S_OK) break;
					rloadsettings_(rb);
					rb->Release();
					break;
				}
				case UI_VIEWVERB_DESTROY: {
					IUIRibbon* rb{ nullptr };
					if ((hr = pv->QueryInterface(IID_PPV_ARGS(&rb))) != S_OK) break;
					rsavesettings_(rb);
					rb->Release();
					break;
				}
				case UI_VIEWVERB_SIZE: {
					(void) ::SetWindowPos(hwnd__.get(), 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
					hr = S_OK;
					break;
				}
				case UI_VIEWVERB_ERROR: {
					break;
				}
				default: break;
			}
			return hr;
		}
		HRESULT STDMETHODCALLTYPE RToolBarBase::OnCreateUICommand(uint32_t, UI_COMMANDTYPE, IUICommandHandler** cmdh) {
			if (cmdh) {
				*cmdh = static_cast<IUICommandHandler*>(this);
				AddRef();
				return S_OK;
			}
			return E_INVALIDARG;
		}
		HRESULT STDMETHODCALLTYPE RToolBarBase::OnDestroyUICommand(uint32_t, UI_COMMANDTYPE, IUICommandHandler*) {
			return E_NOTIMPL;
		}
		#pragma endregion

		#pragma region IUICommandHandler Interface
		HRESULT STDMETHODCALLTYPE RToolBarBase::UpdateProperty(UINT32 cmd, REFPROPERTYKEY key, const PROPVARIANT* val, PROPVARIANT* nval) {
			if (val && nval)
				update_(cmd, key, val, nval);
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE RToolBarBase::Execute(UINT32 cmd, UI_EXECUTIONVERB verb, const PROPERTYKEY* key, const PROPVARIANT* val, IUISimplePropertySet* s) {
			if (verb == UI_EXECUTIONVERB_EXECUTE) {
				try {
					uint32_t hw = 0;
					if (key && val) {
						switch (cmd) {
							case IDM_DIALOG_LAST_OPEN: {
								if (UI_PKEY_SelectedItem.fmtid == key->fmtid) {
									uint32_t idx = 0xffffffff;
									if (::UIPropertyToUInt32(*key, *val, &idx) == S_OK)
										hw = idx + 1;
								}
								break;
							}
							default: break;
						}
						execute_(cmd, key, val, s);
					}
					(void) ::PostMessageW(hwnd__.get(), WM_COMMAND, MAKELONG(cmd, hw), 0);
				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
			}
			return S_OK;
		}
		#pragma endregion

		#pragma region private
		void RToolBarBase::init_(std::wstring rname) {
			if (isinit) return;
			try {
				do {
					if (::CoCreateInstance(CLSID_UIRibbonFramework, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&uifm)) != S_OK) break;
					if (uifm->Initialize(hwnd__, this) != S_OK) break;
					if (uifm->LoadUI(LangInterface::Get().GetLangHinstance(), rname.c_str()) != S_OK) break;
					isinit = true;
					return;
				} while (0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}

			try {
				release_();
			} catch (...) {}
		}
		void RToolBarBase::dispose_() {
			if (hwnd__)	hwnd__.reset();
			if (!isinit) return;
			release_();
			isinit = false;
		}
		void RToolBarBase::release_() {
			try {
				IUIFramework* uf = uifm;
				uifm = nullptr;
				if (uf) {
					IUIRibbon* rb{ nullptr };
					if (uf->GetView(0, IID_PPV_ARGS(&rb)) == S_OK) {
						if (rb) rb->Release();
					}
					uf->Destroy();
					uf->Release();
					isinit = false;
				}
			} catch (...) {}
		}
		void RToolBarBase::setmode_(uint32_t id) {
			if (!isinit || (!uifm)) return;
			try {
				bool b = itemgetboolvalue_(id);
				int32_t mode = b ? (UI_MAKEAPPMODE((int)0) | UI_MAKEAPPMODE((int)1)) : UI_MAKEAPPMODE((int)0);
				(void)uifm->SetModes(mode);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RToolBarBase::update_(UINT32, REFPROPERTYKEY, const PROPVARIANT*, PROPVARIANT*) {}
		void RToolBarBase::execute_(UINT32, const PROPERTYKEY*, const PROPVARIANT*, IUISimplePropertySet*) {}

		void RToolBarBase::itemupdate_(uint32_t id, UI_INVALIDATIONS tag) {
			if (!isinit || (!uifm)) return;
			try {
				(void)uifm->InvalidateUICommand(id, tag, 0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		bool RToolBarBase::itemgetboolvalue_(uint32_t id) {
			if (!isinit || (!uifm)) return false;
			try {
				PROPVARIANT val{};
				if (uifm->GetUICommandProperty(id, UI_PKEY_BooleanValue, &val) != S_OK) return false;
				return (val.vt == VT_BOOL) ? (val.boolVal == VARIANT_TRUE) : false;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		void RToolBarBase::itemsetboolvalue_(uint32_t id, bool b) {
			if (!isinit || (!uifm)) return;
			try {
				PROPVARIANT val{};
				::UIInitPropertyFromBoolean(UI_PKEY_BooleanValue, b, &val);
				(void)uifm->SetUICommandProperty(id, UI_PKEY_BooleanValue, val);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RToolBarBase::itemsetintvalue_(uint32_t id, uint32_t v) {
			if (!isinit || (!uifm)) return;
			try {
				PROPVARIANT val{};
				val.vt = VT_DECIMAL;
				val.decVal.Lo32 = v;
				val.decVal.Lo64 = v;
				(void)uifm->SetUICommandProperty(id, UI_PKEY_DecimalValue, val);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		uint32_t RToolBarBase::itemgetintvalue_(uint32_t id) {
			if (!isinit || (!uifm)) return 0U;
			try {
				PROPVARIANT val{};
				val.vt = VT_DECIMAL;
				if (uifm->GetUICommandProperty(id, UI_PKEY_DecimalValue, &val) != S_OK) return 0U;
				return (val.decVal.Lo64 > 0U) ? static_cast<uint32_t>(val.decVal.Lo64) : val.decVal.Lo32;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return 0U;
		}

		std::wstring RToolBarBase::buildsavepath_() {
			do {
				wchar_t p[MAX_PATH]{};
				if (::SHGetFolderPathW(0, CSIDL_APPDATA, 0, 0, p) != S_OK) break;
				if (!::PathCombineW(p, p, L"MIDIMT")) break;
				if (!::CreateDirectoryW(p, 0) && (GetLastError() != ERROR_ALREADY_EXISTS)) break;
				if (!::PathCombineW(p, p, name__.c_str())) break;
				return std::wstring(p);
			} while (0);
			return L"";
		}
		bool RToolBarBase::rloadsettings_(IUIRibbon* r) {
			try {
				do {
					std::wstring ws = buildsavepath_();
					if (ws.empty()) break;

					IStream* ist{ nullptr };
					IStorage* str{ nullptr };

					if (::StgOpenStorageEx(ws.c_str(),
						STGM_READ | STGM_SHARE_DENY_WRITE,
						STGFMT_STORAGE,
						0, 0, 0,
						__uuidof(IStorage),
						(void**)&str) != S_OK) break;

					if (str->OpenStream(L"Ribbon", 0,
						STGM_READ | STGM_SHARE_EXCLUSIVE,
						0, &ist) != S_OK) break;

					LARGE_INTEGER liStart{ 0, 0 };
					ULARGE_INTEGER ulActual{ 0, 0 };
					ist->Seek(liStart, STREAM_SEEK_SET, &ulActual);
					HRESULT hr = r->LoadSettingsFromStream(ist);
					ist->Release();
					return hr == S_OK;
				} while (0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		bool RToolBarBase::rsavesettings_(IUIRibbon* r) {
			try {
				do {
					std::wstring ws = buildsavepath_();
					if (ws.empty()) break;

					IStream* ist{ nullptr };
					IStorage* str{ nullptr };

					if (::StgCreateStorageEx(ws.c_str(),
						STGM_CREATE | STGM_SHARE_EXCLUSIVE | STGM_READWRITE,
						STGFMT_STORAGE,
						0, 0, 0, __uuidof(IStorage), (void**)&str) != S_OK) break;

					if (str->CreateStream(L"Ribbon",
						STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
						0, 0, &ist) != S_OK) break;

					HRESULT hr = r->SaveSettingsToStream(ist);
					ist->Release();
					return hr == S_OK;
				} while (0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		#pragma endregion
	}
}
