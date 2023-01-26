/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	COMMON::MIDI

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once


namespace Common {
	namespace MIDI {

		class FLAG_EXPORT MackieHelper
		{
		public:
			static int32_t GetSmartHomeTargetID(Common::MIDI::Mackie::Target&);
			static int32_t GetTargetID(Common::MIDI::Mackie::Target&);
			static std::wstring_view GetTarget(Common::MIDI::Mackie::Target&);
			static std::wstring_view GetTarget(uint8_t&);
			static std::wstring_view GetTarget(uint32_t);

			static std::wstring_view GetClickType(Common::MIDI::Mackie::ClickType&);
			static std::wstring_view GetClickType(uint8_t&);
			static std::wstring_view GetClickType(uint32_t);

			static std::wstring_view GetTranslateMMKey(Common::MIDI::Mackie::Target&);
			static std::wstring_view GetTranslateMMKey(uint8_t&);
			static std::wstring_view GetTranslateMMKey(uint32_t);
		};
	}
}