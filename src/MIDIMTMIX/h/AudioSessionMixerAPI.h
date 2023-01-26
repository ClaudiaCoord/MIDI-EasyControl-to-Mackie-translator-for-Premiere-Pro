/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>

#include <atomic>
#include <algorithm>
#include <cctype>
#include <memory>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <exception>
#include <list>
#include <vector>
#include <iostream>
#include <functional>
#include <filesystem>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#include <atlbase.h>
#include <atlstr.h>

#define FLAG_EXPORT __declspec(dllimport)

class Awaiter;
class AwaiterEvent;
class AudioSessionEvents;
class AudioSessionNotify;
class AudioSessionItem;

#pragma warning( push )
#pragma warning( disable : 4251 )

template class FLAG_EXPORT::std::basic_string<wchar_t>;
template class FLAG_EXPORT::std::allocator<AudioSessionItem>;
template class FLAG_EXPORT::std::vector<AudioSessionItem*>;

#include "h\runtime_werror.h"
#include "h\Awaiter.h"
#include "h\AudioSessionHelper.h"
#include "h\AudioVolumeValue.h"
#include "h\AudioSessionItem.h"
#include "h\AudioSessionList.h"
#include "h\AudioSessionMixer.h"

#pragma warning( pop )

