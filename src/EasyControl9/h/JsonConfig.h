/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "..\pch.h"

class FLAG_EXPORT JsonConfig
{
public:
	static const char LogTag[];
	static const char LogLoad[];
	static const char LogSave[];
	static const char LogSaveError[];
	static const char LogLoadError[];
	static const char LogNameEmpty[];

	JsonConfig() {}
	bool Read(MidiDevice& md, std::string filepath);
	bool Write(MidiDevice *md, std::string filepath);
	void Print(MidiDevice *md);
};

