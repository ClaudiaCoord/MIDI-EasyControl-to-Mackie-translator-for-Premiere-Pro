/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
    + DMX protocol, support USB Open DMX and USB RS485 dongle.
    + Art-Net protocol, support UDP local network broadcast.
    (c) CC 2023, MIT

    MIDIMTLIGHT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "CommonApi.h"
#pragma comment(lib, "MIDIMTLIB.lib")
#pragma comment(lib, "iphlpapi.lib")

#ifdef __cplusplus
extern "C" { void __cdecl CloseOnExit(); }
#endif

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        CloseOnExit();
        break;
    }
    return TRUE;
}

void __cdecl CloseOnExit() {
#pragma EXPORT
}
