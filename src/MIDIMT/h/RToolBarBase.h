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

		class safearray_ptr {
		private:
			SAFEARRAY* arr_{ nullptr };
		public:
			safearray_ptr(SAFEARRAY* = nullptr);
			~safearray_ptr();

			const bool empty() const;
			SAFEARRAY* get() const;
		};

		class RecentFile : public IUISimplePropertySet {
		private:
			bool last__{ false };
			ULONG rcnt__{ 0 };
			BSTR name__{ nullptr };
			BSTR path__{ nullptr };
		public:
			RecentFile(std::wstring&, const bool = false);
			~RecentFile();
			const bool empty() const;
			void dispose();
			std::wstring GetPath();

			ULONG STDMETHODCALLTYPE AddRef();
			ULONG STDMETHODCALLTYPE Release();
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void**);
			HRESULT STDMETHODCALLTYPE GetValue(__in REFPROPERTYKEY, __out PROPVARIANT*);
		};

		class RToolBarBase : public IUIApplication, IUICommandHandler
		{
		protected:
			handle_ptr<HWND> hwnd__{};
			IUIFramework* uifm{ nullptr };
			std::wstring name__{};
			ULONG rcnt__{ 0 };
			bool isinit{ false };

			void init_(std::wstring);
			void dispose_();
			void release_();
			void setmode_(uint32_t);
			void itemupdate_(uint32_t, UI_INVALIDATIONS);
			void itemsetboolvalue_(uint32_t, bool);
			bool itemgetboolvalue_(uint32_t);
			bool rsavesettings_(IUIRibbon*);
			bool rloadsettings_(IUIRibbon*);
			void itemsetintvalue_(uint32_t, uint32_t);
			uint32_t itemgetintvalue_(uint32_t);
			std::wstring buildsavepath_();

			virtual void update_(UINT32, REFPROPERTYKEY, const PROPVARIANT*, PROPVARIANT*);
			virtual void execute_(UINT32, const PROPERTYKEY*, const PROPVARIANT*, IUISimplePropertySet*);

		public:

			RToolBarBase(HWND, std::wstring);
			~RToolBarBase();

			const bool IsInit() const;

			void ItemEnable(uint32_t);
			void ItemUpdateValue(uint32_t);
			void ItemUpdateProperty(uint32_t);
			void ItemSetBoolValue(uint32_t, bool);
			bool ItemGetBoolValue(uint32_t);
			void ItemSetIntValue(uint32_t, uint32_t);
			uint32_t ItemGetIntValue(uint32_t);

			HRESULT STDMETHODCALLTYPE OnViewChanged(uint32_t, UI_VIEWTYPE, IUnknown*, UI_VIEWVERB, int32_t);
			HRESULT STDMETHODCALLTYPE OnCreateUICommand(uint32_t, UI_COMMANDTYPE, IUICommandHandler**);
			HRESULT STDMETHODCALLTYPE OnDestroyUICommand(uint32_t, UI_COMMANDTYPE, IUICommandHandler*);

			HRESULT STDMETHODCALLTYPE UpdateProperty(UINT32, REFPROPERTYKEY, const PROPVARIANT*, PROPVARIANT*);
			HRESULT STDMETHODCALLTYPE Execute(UINT32, UI_EXECUTIONVERB, const PROPERTYKEY*, const PROPVARIANT*, IUISimplePropertySet*);

			ULONG STDMETHODCALLTYPE AddRef();
			ULONG STDMETHODCALLTYPE Release();
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void**);

		};
	}
}
