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

		class RToolBarDialogConfig : public RToolBarBase {
		private:
			std::atomic<bool> ispaste_{ false };
			std::atomic<bool> iscopied_{ false };
			std::atomic<bool> isselected_{ false };
			std::atomic<bool> iseditchange_{ false };
			std::atomic<bool> iseditdigits_{ false };
			std::atomic<bool> isautocommit_{ true };
			std::atomic<bool> isfiltertype_{ true };
			std::atomic<bool> isfilterembed_{ false };
			std::atomic<bool> isreadmidienable_{ false };

			std::vector<std::unique_ptr<RecentFile>> recent__;
			safearray_ptr create_safearray_();
			void build_recent_();

			virtual void update_(UINT32, REFPROPERTYKEY, const PROPVARIANT*, PROPVARIANT*) override;
			virtual void execute_(UINT32, const PROPERTYKEY*, const PROPVARIANT*, IUISimplePropertySet*) override;

		public:
			RToolBarDialogConfig(HWND, std::wstring);
			~RToolBarDialogConfig();

			void SetMode();
			void SetEditorNotify(EditorNotify);
			void IsReadMidiCheck(bool);
			const bool IsReadMidiCheck();
			const bool IsEditDigitCheck() const;
			const bool IsFilterEmbedCheck() const;
			const bool IsFilterTypeCheck() const;
			const bool IsFilterAutoCommitCheck() const ;

			MIDI::MixerUnit GetFilters();
			void SetFilters(MIDI::MixerUnit&);

			void EnableRefresh();

			void AddRecent(std::wstring&);
			std::wstring GetRecent(uint32_t);
		};
	}
}