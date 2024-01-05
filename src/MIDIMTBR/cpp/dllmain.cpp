/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "CommonApi.h"
//	Ole32.lib (IsEqualGUID)
#pragma comment(lib, "MIDIMTLIB.lib")

#ifdef __cplusplus
extern "C" {
    void __cdecl CloseOnExit();
}
#endif

BOOL APIENTRY DllMain(HMODULE, DWORD r, LPVOID) {
    switch (r) {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:  
        case DLL_PROCESS_DETACH: CloseOnExit(); break;
    }
    return TRUE;
}

#ifdef __cplusplus
extern "C" {
    void __cdecl CloseOnExit() {
        #pragma EXPORT
    }
}
#endif
