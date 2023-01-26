/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once
namespace Common {
	namespace MIXER {

		class FLAG_EXPORT AudioSessionItemMidi
		{
		private:
			uint32_t Id[3] = { 0U, 0U, 0U };
			void set_(Common::MIDI::MidiUnitType, Common::MIDI::Mackie::Target, Common::MIDI::Mackie::Target, uint32_t);
		public:

			AudioSessionItemMidi();

			const bool IsKeys();
			const bool IsFound(uint32_t);
			void Set(Common::MIDI::MidiUnit&);
			void Set(Common::MIDI::MixerUnit&);
			void Set(uint32_t[]);
			uint32_t* Get();
			AudioAction Get(uint32_t);
			uint32_t Get(AudioAction);
			void Clear();
			void Copy(Common::MIXER::AudioSessionItemMidi&);
			void Copy(Common::MIXER::AudioSessionItem&);

			const std::wstring ToString();
		};
	}
}

