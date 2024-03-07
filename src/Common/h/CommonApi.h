/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
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
#include <format>
#include <array>
#include <shared_mutex>
#include <any>
#include <regex>
#include <type_traits>
#include <unordered_set>
#include <queue>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#include <atlbase.h>
#include <atlstr.h>

#include <objidl.h>
#include <gdiplus.h>

#include <commctrl.h>
#include <comdef.h>

typedef std::function<void(const std::wstring&)> callFromlog_t;

#define EXPORT comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)
#define CHECK_LPWSTRING(a) (bool)((a != nullptr) && (wcslen(a) > 0))
#define BOOL_TOLOG_(A,B) " " << A << "=" << (B) << ","
#define BOOL_TOLOG(A) BOOL_TOLOG_(#A, A ? "yes" : "no")

#if !defined (max_)
	#define max_(a,b) (((a) > (b)) ? (a) : (b))
#endif
#if !defined (min_)
	#define min_(a,b) (((a) < (b)) ? (a) : (b))
#endif

#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__)
static_assert(sizeof(void*) == 8, "Error: is not 64 bit platform");
#define PX64__
#else
static_assert(sizeof(void*) == 4, "Error: is not 32 bit platform");
#define PX32__
#endif

#include "midi\MidiMackieTargetId.h"

