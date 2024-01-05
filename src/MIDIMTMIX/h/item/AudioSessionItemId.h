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

		typedef std::pair<MIDI::MidiUnitType, uint32_t> ItemIdPair_t;
		typedef std::forward_list<ItemIdPair_t> ItemIdList_t;

		class FLAG_EXPORT AudioSessionItemId
		{
		private:
			ItemIdList_t keys;

			void copy_(AudioSessionItemId&, bool);

		public:

			operator bool();

			AudioSessionItemId() = default;

			MIDI::MidiUnitType found(uint32_t);
			MIDI::MidiUnitType found(ItemIdPair_t&);
			MIDI::MidiUnitType found(uint32_t, MIDI::MidiUnitType);

			const bool		 empty();
			const bool		 add(MIDI::MidiUnit&);
			const bool		 add(MIDI::MidiUnitType, uint32_t);
			const bool		 add(ItemIdPair_t);
			ItemIdList_t&	 get();

			void remove(uint32_t);

			void copy(AudioSessionItem&);
			void copy(AudioSessionItemId&);
			void copy(AudioSessionItemChange&);
			log_string to_string();

			static MIDI::MidiUnitType normalize_type(MIDI::MidiUnitType);
		};
	}
}

