/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#ifndef _USRDLL
#include <windows.h>
#include <memory>
#include <string>
#include <vector>
#define FLAG_API  
#define FLAG_EXPORT __declspec(dllimport) 
#endif

#include "h\Mackie.h"
#include "h\MackieHelper.h"
#include "h\MidiData.h"

typedef void (*f_Fn_event)(MidiUnit*);
typedef void (*f_Fn_proxy)(DWORD);
typedef void (*f_Fn_status)(const std::string);
typedef void (*f_Fn_monitor)(uint8_t scene, uint8_t id, uint8_t v, uint32_t t);
#define CONFIG_NAME "MidiController.cnf"

/* Export DLL API */

FLAG_EXPORT bool FLAG_API OpenTMidiControllerDefault();
FLAG_EXPORT bool FLAG_API OpenTMidiController(f_Fn_status fstatus, std::string filepath = CONFIG_NAME);
FLAG_EXPORT bool FLAG_API ReOpenTMidiController(f_Fn_status fstatus, std::string filepath = CONFIG_NAME);
FLAG_EXPORT bool FLAG_API CloseTMidiController();

FLAG_EXPORT bool FLAG_API TMidiStart();
FLAG_EXPORT bool FLAG_API TMidiStart(std::string & filepath);
FLAG_EXPORT bool FLAG_API TMidiStop();
FLAG_EXPORT bool FLAG_API TMidiLoad();
FLAG_EXPORT bool FLAG_API TMidiLoad(std::string & filepath);
FLAG_EXPORT bool FLAG_API TMidiSave();
FLAG_EXPORT bool FLAG_API TMidiSave(std::string & filepath);

FLAG_EXPORT bool FLAG_API TMidiSetButtonOnInterval(uint32_t msec);
FLAG_EXPORT bool FLAG_API TMidiSetButtonOnLongInterval(uint32_t msec);
FLAG_EXPORT bool FLAG_API TMidiSetCallbackStatus(f_Fn_status f);
FLAG_EXPORT bool FLAG_API TMidiSetCallbackProxy(f_Fn_proxy f);
FLAG_EXPORT void FLAG_API TMidiRemoveCallbackStatus(f_Fn_status f);
FLAG_EXPORT void FLAG_API TMidiSetAutoStart(bool b);
FLAG_EXPORT void FLAG_API TMidiSetInMonitor(bool b);
FLAG_EXPORT void FLAG_API TMidiSetCallbackInMonitor(f_Fn_monitor f);
FLAG_EXPORT void FLAG_API TMidiSetOutManualPort(bool b);

FLAG_EXPORT bool FLAG_API IsTMidiStarted();
FLAG_EXPORT bool FLAG_API IsTMidiEnable();
FLAG_EXPORT bool FLAG_API IsTMidiConfig();
FLAG_EXPORT bool FLAG_API IsTMidiProxy();
FLAG_EXPORT bool FLAG_API IsTMidiAutoStart();

FLAG_EXPORT void  FLAG_API TMidiSetProxyCount(uint32_t i);
FLAG_EXPORT uint32_t  FLAG_API TMidiGetProxyCount();

FLAG_EXPORT bool FLAG_API IsTMidiInEnable();
FLAG_EXPORT bool FLAG_API IsTMidiOutEnable();
FLAG_EXPORT bool FLAG_API IsTMidiInConnect();
FLAG_EXPORT bool FLAG_API IsTMidiOutConnect();
FLAG_EXPORT bool FLAG_API IsTMidiInMonitor();
FLAG_EXPORT bool FLAG_API IsTMidiOutManualPort();


FLAG_EXPORT bool FLAG_API TMidiSendToPort(DWORD d);
FLAG_EXPORT bool FLAG_API TMidiSendToPort(MidiUnit* unit);

FLAG_EXPORT std::vector<std::string> FLAG_API TMidiDeviceList();
FLAG_EXPORT std::shared_ptr<MidiDevice> FLAG_API TMidiGetConfig();
FLAG_EXPORT std::string FLAG_API TMidiInDeviceName();
FLAG_EXPORT std::string FLAG_API TMidiOutDeviceName();

#ifdef __cplusplus
extern "C"
{
#endif
		void __cdecl CloseOnExit();
#ifdef __cplusplus
}
#endif
