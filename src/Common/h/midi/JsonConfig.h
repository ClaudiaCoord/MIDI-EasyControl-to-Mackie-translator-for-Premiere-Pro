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

		class FLAG_EXPORT JsonConfig
		{
		public:

			JsonConfig() {}
			bool Read(MidiDevice* md, std::wstring filepath);
			bool Write(MidiDevice* md, std::wstring filepath);
			std::wstring Dump(MidiDevice* md);

			template<class T1>
			bool Read(T1 & t, std::wstring filepath) {
				return Read(t.get(), filepath);
			}
			template<class T1>
			bool Write(T1& t, std::wstring filepath) {
				return Write(t.get(), filepath);
			}
			template<class T1>
			bool Dump(T1& t) {
				return Dump(t.get());
			}
		};
	}
}
