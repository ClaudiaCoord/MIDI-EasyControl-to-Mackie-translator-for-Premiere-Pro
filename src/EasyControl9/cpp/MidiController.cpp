/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#include "..\pch.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "tevirtualmidi.lib")

#define ISPTRBAD(A) if ((A == nullptr) || (!A)) return false
#define ISPTRGOOD(A,B) if ((A != nullptr) && (A)) B

std::unique_ptr<MidiBridge> ctrl_bridge_ptr = nullptr;
f_Fn_status console = nullptr;

// API

FLAG_EXPORT bool FLAG_API TMidiStart(std::string & filepath) {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->Start(filepath);
}
FLAG_EXPORT bool FLAG_API TMidiStart() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->Start();
}
FLAG_EXPORT bool FLAG_API TMidiStop() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->Stop();
}
FLAG_EXPORT bool FLAG_API IsTMidiStarted() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->IsStarted();
}

FLAG_EXPORT bool FLAG_API TMidiLoad() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->Start();
}
FLAG_EXPORT bool FLAG_API TMidiLoad(std::string & fp) {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->Load(fp);
}
FLAG_EXPORT bool FLAG_API TMidiSave() {
    if ((ctrl_bridge_ptr == nullptr) || (!ctrl_bridge_ptr))
        return false;
    return ctrl_bridge_ptr->Save();
}
FLAG_EXPORT bool FLAG_API TMidiSave(std::string & fp) {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->Save(fp);
}

FLAG_EXPORT void FLAG_API TMidiRemoveCallbackStatus(f_Fn_status f) {
    ISPTRGOOD(ctrl_bridge_ptr, ctrl_bridge_ptr->RemoveCallbackStatus(f));
}
FLAG_EXPORT bool FLAG_API TMidiSetCallbackStatus(f_Fn_status f) {
    ISPTRBAD(ctrl_bridge_ptr);
    ctrl_bridge_ptr->SetCallbackStatus(f);
    return true;
}
FLAG_EXPORT bool FLAG_API TMidiSetCallbackProxy(f_Fn_proxy f) {
    ISPTRBAD(ctrl_bridge_ptr);
    ctrl_bridge_ptr->SetCallbackProxy(f);
    return true;
}
FLAG_EXPORT bool FLAG_API TMidiSetButtonOnInterval(uint32_t msec) {
    ISPTRBAD(ctrl_bridge_ptr);
    ctrl_bridge_ptr->SetButtonOnInterval(msec);
    return true;
}
FLAG_EXPORT bool FLAG_API TMidiSetButtonOnLongInterval(uint32_t msec) {
    ISPTRBAD(ctrl_bridge_ptr);
    ctrl_bridge_ptr->SetButtonOnLongInterval(msec);
    return true;
}
FLAG_EXPORT void FLAG_API TMidiSetAutoStart(bool b) {
    ISPTRGOOD(ctrl_bridge_ptr, ctrl_bridge_ptr->SetAutoStart(b));
}
FLAG_EXPORT void FLAG_API TMidiSetProxy(bool b) {
    ISPTRGOOD(ctrl_bridge_ptr, ctrl_bridge_ptr->SetProxy(b));
}
FLAG_EXPORT void FLAG_API TMidiSetOutManualPort(bool b) {
    ISPTRGOOD(ctrl_bridge_ptr, ctrl_bridge_ptr->SetOutManualPort(b));
}


FLAG_EXPORT bool FLAG_API IsTMidiOutManualPort() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->IsOutManualPort();
}
FLAG_EXPORT bool FLAG_API IsTMidiInMonitor() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->IsInMonitor();
}
FLAG_EXPORT void FLAG_API TMidiSetInMonitor(bool b) {
    ISPTRGOOD(ctrl_bridge_ptr, ctrl_bridge_ptr->SetInMonitor(b));
}
FLAG_EXPORT void FLAG_API TMidiSetCallbackInMonitor(f_Fn_monitor f) {
    ISPTRGOOD(ctrl_bridge_ptr, ctrl_bridge_ptr->SetCallbackInMonitor(f));
}


FLAG_EXPORT bool FLAG_API TMidiSendToPort(MidiUnit *unit) {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->SendToPort(unit);
}
FLAG_EXPORT bool FLAG_API TMidiSendToPort(DWORD d) {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->SendToPort(d);
}

FLAG_EXPORT bool FLAG_API IsTMidiEnable() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->IsEnable();
}
FLAG_EXPORT bool FLAG_API IsTMidiConfig() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->IsConfig();
}
FLAG_EXPORT bool FLAG_API IsTMidiAutoStart() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->IsAutoStart();
}
FLAG_EXPORT bool FLAG_API IsTMidiProxy() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->IsProxy();
}

FLAG_EXPORT bool FLAG_API IsTMidiInEnable() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->IsInEnable();
}
FLAG_EXPORT bool FLAG_API IsTMidiOutEnable() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->IsOutEnable();
}
FLAG_EXPORT bool FLAG_API IsTMidiInConnect() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->IsInConnect();
}
FLAG_EXPORT bool FLAG_API IsTMidiOutConnect() {
    ISPTRBAD(ctrl_bridge_ptr);
    return ctrl_bridge_ptr->IsOutConnect();
}

FLAG_EXPORT bool FLAG_API OpenTMidiControllerDefault() {
    return OpenTMidiController(nullptr, std::string(CONFIG_NAME));
}
FLAG_EXPORT bool FLAG_API OpenTMidiController(f_Fn_status fstatus, std::string filepath) {
    if ((ctrl_bridge_ptr != nullptr) && (ctrl_bridge_ptr)) return true;
    return ReOpenTMidiController(fstatus, filepath);
}
FLAG_EXPORT bool FLAG_API ReOpenTMidiController(f_Fn_status fstatus, std::string filepath) {

    try {
        if (fstatus != nullptr)
            console = fstatus;
        if (ctrl_bridge_ptr == nullptr)
            ctrl_bridge_ptr = std::make_unique<MidiBridge>(fstatus, filepath);
        else
            ctrl_bridge_ptr.reset(new MidiBridge(fstatus, filepath));
    }
    catch (std::runtime_error & err) {
        Utils::ErrorMessage(LogString() << LogString::LogErrorOpen << "[" << err.what() << "]", console);
        return false;
    }
    catch (...) {
        Utils::ErrorMessage(LogString() << LogString::LogErrorFault, console);
        return false;
    }
    return true;
}

FLAG_EXPORT bool FLAG_API CloseTMidiController() {
    try {
        ISPTRGOOD(ctrl_bridge_ptr, { ctrl_bridge_ptr.get()->Dispose(); return true; });
        return false;
    }
    catch (std::runtime_error& err) {
        Utils::ErrorMessage(LogString() << LogString::LogErrorClose << "[" << err.what() << "]", console);
        return false;
    }
    catch (...) {
        Utils::ErrorMessage(LogString() << LogString::LogErrorFault, console);
        return false;
    }
}

FLAG_EXPORT std::vector<std::string> FLAG_API TMidiDeviceList() {
    ISPTRGOOD(ctrl_bridge_ptr, return ctrl_bridge_ptr.get()->InDeviceList());
    return std::vector<std::string>();
}
FLAG_EXPORT std::shared_ptr<MidiDevice> FLAG_API TMidiGetConfig() {
    ISPTRGOOD(ctrl_bridge_ptr, return ctrl_bridge_ptr.get()->GetConfig());
    return std::make_shared<MidiDevice>();
}

FLAG_EXPORT std::string FLAG_API TMidiInDeviceName() {
    ISPTRGOOD(ctrl_bridge_ptr, return ctrl_bridge_ptr.get()->InDeviceName());
    return "";
}
FLAG_EXPORT std::string FLAG_API TMidiOutDeviceName() {
    ISPTRGOOD(ctrl_bridge_ptr, return ctrl_bridge_ptr.get()->OutDeviceName());
    return "";
}


#ifdef _USRDLL
extern "C"
{
#endif

    void __cdecl CloseOnExit() {
#pragma EXPORT
#if __DEBUG
    std::cout << "*CloseOnExit()" << std::endl;
#endif
    (void)CloseTMidiController();
}

#ifdef _USRDLL
}
#endif
