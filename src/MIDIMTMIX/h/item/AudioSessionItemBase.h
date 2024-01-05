/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIXER {

		class FLAG_EXPORT AudioSessionItemBase
		{
		private:
			/* Session type */
			TypeItems Typeitem = TypeItems::TypeNone;

		public:

			/* Application values */
			AudioSessionItemApp App{};

			/* Volume values */
			AudioSessionItemValue Volume{};

			/* MIDI keys */
			AudioSessionItemId Id{};

			AudioSessionItemBase() = default;
			~AudioSessionItemBase() = default;

			/* Exports */

			const bool IsEmptyApp();
			const bool IsEmptyId();
			const bool IsEqualsApp(const std::size_t);
			const bool IsEqualsGuid(const GUID&);
			const MIDI::MidiUnitType FoundId(const uint32_t, const MIDI::MidiUnitType = MIDI::MidiUnitType::UNITNONE);

			const GUID GetGuid() const;
			const uint32_t GetPid() const;
			const TypeItems GetType() const;
			
			const std::wstring GetName() const;
			const std::wstring GetDesc() const;
			const std::wstring GetIcon() const;
			const std::wstring GetPath() const;

			void SetType(TypeItems);
		};
	}
}