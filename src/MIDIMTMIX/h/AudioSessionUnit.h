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

		class AudioSessionUnit
		{
		private:
			std::forward_list<std::size_t> applist_;
			MIDI::MidiUnit& unit_;
			const MIDI::MidiUnitType type_;
			const uint32_t id_;

		public:

			AudioSessionUnit(MIDI::MidiUnit&, bool = true);
			
			const MIDI::MidiUnitType	GetType() const;
			const uint32_t				GetId() const;
			const uint8_t				GetVolume();
			const bool					GetMute();
			ItemIdPair_t				GetData();
			const bool					AppFound(std::size_t);

			void						UpdateVolume(uint8_t, bool);

			log_string to_string();
		};

		typedef std::shared_ptr<AudioSessionUnit> ASUNIT_t;
		typedef std::vector<ASUNIT_t> AULIST_t;
	}
}

