/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "..\..\MIDIMTDLL\MIDIMTApi.h"
#include "..\..\MIDIMTMIX\MIDIMTMIXApi.h"
#include "..\..\EASYCTRL9\EASYCTRL9Api.h"
#include "..\..\MEDIAKEYS\MEDIAKEYSApi.h"
#include "..\..\SMARTHOME\SMARTHOMEApi.h"
#include "..\..\MIDIMTLIGHT\MIDIMTLIGHTApi.h"

#include "..\..\Common\rc\resource_midimt.h"
#include "..\..\Common\rc\resource_mackie.h"
#include "..\..\Common\rc\resource_version.h"
#include "..\rc\resource.h"

#include <windowsx.h>
#include <shellapi.h>
#include <commctrl.h>
#include <Shlobj.h>
#include <Shlobj_core.h>
#include <objbase.h>
#include <initguid.h>
#include <uiribbon.h>
#include <uiribbonpropertyhelpers.h>

#include <queue>

#include "ext\lodepng.h"
#include "handle_ptr.h"
#include "CbEvent.h"
#include "CheckRun.h"
#include "LangInterface.h"
#include "GuiUtils.h"
#include "ListEdit.h"
#include "TrayMenu.h"
#include "TrayNotify.h"
#include "RToolBarBase.h"

#include "mixer\Sprite.h"
#include "mixer\Sprites.h"
#include "mixer\DialogThemeColors.h"
#include "mixer\AudioMixerPanel.h"
#include "mixer\AudioMixerPanels.h"

#include "DialogStart.h"
#include "DialogMonitor.h"
#include "RToolBarDialogConfig.h"
#include "DialogConfig.h"
#include "DialogAbout.h"

