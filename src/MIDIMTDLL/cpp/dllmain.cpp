/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "CommonApi.h"
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "version.lib")

#ifdef __cplusplus
extern "C" { void __cdecl CloseOnExit(); }
#endif

BOOL APIENTRY DllMain(HMODULE, DWORD r, LPVOID)
{
    switch (r) {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH: break;
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH: {
            CloseOnExit();
            break;
        }
    }
    return TRUE;
}

void __cdecl CloseOnExit() {
#pragma EXPORT
}
