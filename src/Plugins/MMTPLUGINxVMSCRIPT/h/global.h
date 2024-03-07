/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "MIDIMTApi.h"
#include "MIDIMTVEUIApi.h"
#include "../../shared/h/dllmacro.h"
#include "../../shared/h/chaiscript/chaiscript.hpp"

#define FLAG_EXPORT __declspec(dllexport)

#include "VmScriptUserAction.h"
#include "VmScriptBootstrap.h"
#include "VmScriptUserUnit.h"
#include "VmScriptUserColor.h"
#include "VmScriptUserMacro.h"
#include "VmScriptDebug.h"
#include "VmScriptsTask.h"
#include "VmScript.h"
#include "VmScripts.h"
#include "SetupDialog.h"
#include "MMTPLUGINxVMSCRIPT.h"

