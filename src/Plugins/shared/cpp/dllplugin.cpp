﻿/*
	MIDI-MT plugins part.
	(c) CC 2023-2024, MIT

	TEMPLATE MMTPLUGINx* DLL

	Output:      $(SolutionDir)_BUILD\$(Configuration)\$(PlatformShortName)\Plugins\
	BuildDir:    $(SolutionDir)_BUILD\$(Configuration)\$(PlatformShortName)\$(MSBuildProjectName)\
	Preprocessor: _PLUGIN_DLL;_PLUGIN64/_PLUGIN32;_PLUGINGUID=R"($(ProjectGuid))";

	Include in plugin class:
		typedef Common::PLUGINS::XXXPlugin ThisPluginClass;

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#if defined (_PLUGIN_DLL)

#include "global.h"
#pragma comment(lib, "MIDIMTLIB.lib")
#pragma comment(lib, "comctl32.lib")

extern "C" {
	static HMODULE dll_hinstance_{ nullptr };
	HMODULE GetCurrentDllHinstance();
	void CloseOnExit();
}

BOOL APIENTRY DllMain(HMODULE h, DWORD r, LPVOID) {
	dll_hinstance_ = h;

	switch (r) {
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_ATTACH: break;
		case DLL_PROCESS_DETACH: CloseOnExit(); break;
	}
	return TRUE;
}

#	ifdef __cplusplus

	#if defined (_PLUGIN64)
	#	pragma comment(linker, "/export:OpenPlugin=?OpenPlugin@@YA?AV?$shared_ptr@VPlugin@IO@Common@@@std@@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@2@PEAUHWND__@@@Z")
	#elif defined (_PLUGIN32)
	#	pragma comment(linker, "/export:OpenPlugin=?OpenPlugin@@YA?AV?$shared_ptr@VPlugin@IO@Common@@@std@@V?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@2@PAUHWND__@@@Z")
	#endif

	FLAG_EXPORT Common::IO::plugin_t OpenPlugin(std::wstring path, HWND h) {
		return std::make_shared<ThisPluginClass>(path, h);
	}
#	endif

extern "C" {
	void CloseOnExit() {
		#pragma EXPORT
	}
	HMODULE GetCurrentDllHinstance() {
		#pragma EXPORT
		return dll_hinstance_;
	}
}

#endif