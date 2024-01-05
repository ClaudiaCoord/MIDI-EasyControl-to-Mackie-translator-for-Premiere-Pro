/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "CommonApi.h"

#define FLAG_EXPORT __declspec(dllimport)

#include "h\bridge_out_event.h"
#include "h\bridge_in_event.h"
#include "h\bridge_sys.h"
#include "h\Plugins.h"
#include "h\IOBridge.h"

#if defined (_PLUGIN_DLL)
#ifdef __cplusplus
Common::IO::plugin_t __cdecl FLAG_EXPORT OpenPlugin();
#endif
#endif

#undef FLAG_EXPORT


