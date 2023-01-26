/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <timeapi.h>

#include <atomic>
#include <algorithm>
#include <cctype>
#include <memory>
#include <string>
#include <string_view>
#include <sstream>
#include <map>
#include <vector>
#include <exception>
#include <vector>
#include <iostream>
#include <functional>
#include <filesystem>
#include <fstream>
#include <future>
#include <thread>
#include <forward_list>
#include <utility>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#include <atlbase.h>
#include <atlstr.h>

typedef std::function<void(const std::wstring&)> logFnType;

#define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)
#define CHECK_LPWSTRING(a) (bool)((a != nullptr) && (wcslen(a) > 0))
#define _BOOL_TOLOG(A,B) " " << A << "=" << (B) << ","
#define BOOL_TOLOG(A) _BOOL_TOLOG(#A, A ? "yes" : "no")

#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__)
static_assert(sizeof(void*) == 8, "Error: is not 64 bit platform");
#define PX64__
#else
static_assert(sizeof(void*) == 4, "Error: is not 32 bit platform");
#define PX32__
#endif

#include "midi\MidiMackieTargetId.h"

