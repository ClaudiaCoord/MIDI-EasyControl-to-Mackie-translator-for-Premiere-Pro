/*
    MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
    + Audio session volume/mute mixer.
    + MultiMedia Key translator.
    (c) CC 2023-2024, MIT

    MIDIMT ALL LANGUAGE

    See README.md for more details.
    NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "dll_lang_global.h"

BOOL APIENTRY DllMain(HMODULE, DWORD r, LPVOID) {
    switch (r) {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH: break;
    }
    return TRUE;
}

