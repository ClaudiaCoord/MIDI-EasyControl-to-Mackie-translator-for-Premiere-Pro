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

	class FLAG_EXPORT json_recent {
	private:
		std::vector<std::wstring> recents__;

		bool read_(std::wstring);
		bool write_(std::wstring);

	public:

		json_recent();

		void Load();
		void Add(std::wstring&);
		std::vector<std::wstring>& GetRecents();

		std::wstring Dump();
	};
}
