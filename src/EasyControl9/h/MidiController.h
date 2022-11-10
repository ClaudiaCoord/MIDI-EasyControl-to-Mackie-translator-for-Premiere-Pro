/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "..\pch.h"

#ifndef _USRDLL
#	define FLAG_API  
#	define FLAG_EXPORT __declspec(dllimport)
#else
#	define FLAG_API
#	define FLAG_EXPORT __declspec(dllexport)
#endif

#define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)

/*
struct FLAG_EXPORT MidiUnit;
struct FLAG_EXPORT MidiDevice;
struct FLAG_EXPORT MidiControllerIn;
struct FLAG_EXPORT MidiControllerOut;
struct FLAG_EXPORT MidiControllerProxy;

template class FLAG_EXPORT std::shared_ptr<MidiDevice>;
template class FLAG_EXPORT std::unique_ptr<MidiControllerIn>;
template class FLAG_EXPORT std::unique_ptr<MidiControllerOut>;
template class FLAG_EXPORT std::unique_ptr<MidiControllerProxy>;
*/

#include "..\MidiControllerAPI.h"
