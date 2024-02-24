/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

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
			static std::wstring GetScriptTarget(const Common::MIDI::Mackie::Target&);
			static int32_t GetScriptTargetID(const Common::MIDI::Mackie::Target&);
			static std::wstring GetSmartHomeTarget(const Common::MIDI::Mackie::Target&);
			static int32_t GetSmartHomeTargetID(const Common::MIDI::Mackie::Target&);
			static int32_t GetTargetID(const Common::MIDI::Mackie::Target&);
			static std::wstring_view GetTarget(const Common::MIDI::Mackie::Target&);
			static std::wstring_view GetTarget(const uint8_t&);
			static std::wstring_view GetTarget(const uint32_t);

			static std::wstring_view GetClickType(const Common::MIDI::Mackie::ClickType&);
			static std::wstring_view GetClickType(const uint8_t&);
			static std::wstring_view GetClickType(const uint32_t);

			static std::wstring_view GetTranslateMMKey(const Common::MIDI::Mackie::Target&);
			static std::wstring_view GetTranslateMMKey(const uint8_t&);
			static std::wstring_view GetTranslateMMKey(const uint32_t);
		};
	}
}