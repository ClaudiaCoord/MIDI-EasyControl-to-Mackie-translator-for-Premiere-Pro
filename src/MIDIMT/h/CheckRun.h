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

		class CheckRun {
		private:
			const uint32_t msgid__;
			std::wstring confpath__;

			const bool parse_();
			const bool check_(bool);
			void setconfig_();
			void calledit_();
			void clear_();
			static BOOL CALLBACK broadcast_find_(HWND, LPARAM);

		public:

			CheckRun();
			~CheckRun();

			static CheckRun& Get();
			const uint32_t GetMsgId() const;
			const std::wstring& GetConfigPath() const;
			const bool Begin();

			void notify(HWND);
			static COPYDATASTRUCT* build(COPYDATASTRUCT*);
			static COPYDATASTRUCT* build(uint32_t, std::wstring);
		};
	}
}
