/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ VM Chai script support (https://github.com/ChaiScript/ChaiScript/)
	(c) CC 2023-2024, MIT

	MMT Script Tester (VM Chai script tester)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "MIDIMTApi.h"
#define CHAISCRIPT_EXTRAS_MATH_SKIP_ADVANCED 1
#include "../../Plugins/shared/h/chaiscript/chaiscript.hpp"
#include "terminal/tcolor.h"
#include "cmdline/argparse.h"

#define FLAG_EXPORT __declspec(dllimport)

#define TRACE_CALL()
#define TRACE_CALLX(A)
#define TRACE_CALLD(A)

#include "VmScriptUserAction.h"
#include "VmScriptBootstrap.h"
#include "VmScriptUserUnit.h"
#include "VmScriptUserColor.h"
#include "VmScriptUserMacro.h"
#include "VmScriptsTask.h"
#include "VmScript.h"
#include "VmScripts.h"

