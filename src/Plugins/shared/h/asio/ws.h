/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* All DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#if !defined(ASIO_STANDALONE)
#define ASIO_STANDALONE 1
#endif

#include "asio.h"

#if defined(_M_X64) || defined(_M_IX86)
#include <intrin.h>
static inline void spin_loop_pause() { _mm_pause(); }
#endif
#if !defined(FLAG_DEPRECATED)
#define FLAG_DEPRECATED  /* __declspec(deprecated) */
#endif
#if !defined(FLAG_EXPORT)
#define FLAG_EXPORT __declspec(dllexport)
#endif

#include "WS\WSASIO.h"
#include "WS\WSAKEY.h"
#include "WS\WSSTATUS.h"
#include "WS\WSUTILITY.h"
#include "WS\WSSERVER.h"