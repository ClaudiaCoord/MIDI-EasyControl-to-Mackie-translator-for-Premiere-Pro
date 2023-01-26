/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MEDIAKEYS DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "CommonApi.h"
#include <mmeapi.h>

#define FLAG_EXPORT __declspec(dllimport)

#pragma warning( push )
#pragma warning( disable : 4251 )

#include "h\MMKBridge.h"

#pragma warning( pop )

#undef FLAG_EXPORT


