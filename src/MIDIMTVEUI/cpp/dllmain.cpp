/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Viewer/Editor interface.
	(c) CC 2023-2024, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "CommonApi.h"
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "MIDIMTLIB.lib")
#pragma comment(lib, "Scintilla.lib")
#pragma comment(lib, "Lexilla.lib")

#ifdef __cplusplus
extern "C" {
    void __cdecl   CloseOnExit();
    bool __stdcall Scintilla_Register(HINSTANCE);
    void __stdcall Scintilla_UnRegister();
}
#endif

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            Scintilla_Register(hModule);
            break;
        }
        case DLL_PROCESS_DETACH: {
            Scintilla_UnRegister();
            break;
        }
        case DLL_THREAD_DETACH: {
            CloseOnExit();
            break;
        }
        case DLL_THREAD_ATTACH: break;
    }
    return TRUE;
}

void __cdecl CloseOnExit() {
#pragma EXPORT
}
