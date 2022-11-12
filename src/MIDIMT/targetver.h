/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once
#include <SDKDDKVer.h>

#if defined(__W64)
#    define PLATFORM_NAME "X64\0"
#elif defined(__W32)
#    define PLATFORM_NAME "X32\0"
#else
#    define PLATFORM_NAME "UN\0"
#endif
