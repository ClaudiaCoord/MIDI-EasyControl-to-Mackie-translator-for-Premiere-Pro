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

		class FLAG_EXPORT MidiHelper
		{
		public:
			static const std::wstring GetSuffixMackieOut();
			static const std::wstring GetSuffixProxyOut();

			static std::wstring_view GetClassTypes(ClassTypes&);
			
			static std::wstring_view GetType(Common::MIDI::MidiUnitType&);
			static std::wstring_view GetType(uint8_t&);
			static std::wstring_view GetType(uint32_t);
			
			static std::wstring_view GetScene(Common::MIDI::MidiUnitScene&);
			static std::wstring_view GetScene(uint8_t&);
			static std::wstring_view GetScene(uint32_t);
		};
	}
}