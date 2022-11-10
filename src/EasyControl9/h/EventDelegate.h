/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "..\pch.h"

class EventDelegate
{
    std::vector<std::function<void(MidiUnit*)>> funcs;
public:
    template<class T> EventDelegate& operator+=(T mFunc) { funcs.push_back(mFunc); return *this; }
    void operator()(MidiUnit* m) { for (auto& f : funcs) f(m); }
};

