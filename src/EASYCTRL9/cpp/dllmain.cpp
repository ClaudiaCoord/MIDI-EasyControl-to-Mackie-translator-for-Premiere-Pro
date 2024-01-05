/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	EASYCTRL9 - All MIDI drivers

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "CommonApi.h"
#pragma comment(lib, "MIDIMTLIB.lib")
#pragma comment(lib, "winmm.lib")

#ifndef _LOAD_MIDIDRV_DYNAMIC
#   pragma comment(lib, "tevirtualmidi.lib")
#endif

extern "C" void __cdecl CloseOnExit();

BOOL APIENTRY DllMain(HMODULE, DWORD r, LPVOID){
    switch (r) {
        case DLL_THREAD_ATTACH:
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_DETACH:  break;
        case DLL_PROCESS_DETACH: {
            CloseOnExit();
            break;
        }
        default: break;
    }
    return TRUE;
}

extern "C" void __cdecl CloseOnExit() {
    #pragma EXPORT
}
