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

#include "..\MIDIMTDLL\MIDIMTApi.h"
#include "..\MIDIMTMIX\MIDIMTMIXApi.h"
#include "..\EASYCTRL9\EASYCTRL9Api.h"
#include "..\MEDIAKEYS\MEDIAKEYSApi.h"

#include "..\rc\targetver.h"
#include "..\rc\resource.h"
#include "..\rc\resource_algo.h"

#include <windowsx.h>
#include <shellapi.h>
#include <commctrl.h>

#include <queue>

#include "ext\lodepng.h"
#include "handle_ptr.h"
#include "CbEvent.h"
#include "LangInterface.h"
#include "GuiUtils.h"
#include "ListEdit.h"
#include "TrayMenu.h"
#include "TrayNotify.h"

#include "mixer\Sprite.h"
#include "mixer\Sprites.h"
#include "mixer\DialogThemeColors.h"
#include "mixer\AudioMixerPanel.h"
#include "mixer\AudioMixerPanels.h"

#include "DialogStart.h"
#include "DialogMonitor.h"
#include "DialogConfig.h"
#include "DialogAbout.h"

