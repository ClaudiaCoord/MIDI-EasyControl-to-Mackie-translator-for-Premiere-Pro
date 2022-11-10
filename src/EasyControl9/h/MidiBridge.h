/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "..\pch.h"

class MidiBridge
{
    const char* LogTag = "MIDI bridge ";
    bool isEnable = false;
    bool isConfig = false;
    bool isProxy = false;
    bool isAutoStart = false;

    StatusDelegate __status{};
    std::string __filepath{};
    std::shared_ptr<MidiDevice> config_ptr;

    std::unique_ptr<MidiControllerIn>  ctrl_in_ptr = nullptr;
    std::unique_ptr<MidiControllerOut> ctrl_out_ptr = nullptr;
    std::unique_ptr<MidiControllerProxy> ctrl_proxy_ptr = nullptr;

    static MidiBridge * ctrl;

    void Initializer2();
    void Initializer1(bool isstart);
    void Initializer0(f_Fn_status fstatus, std::string& fp);
    void LogCallback(std::string s);
    std::string CheckFilePath(std::string& s);

    MidiBridge() = delete;

public:

    MidiBridge(f_Fn_status fstatus, std::string fp);
    ~MidiBridge();

    void Dispose(bool isfull = true);
    bool Start(std::string& fp);
    bool Start();
    bool Stop();
    bool Load();
    bool Load(std::string& fp);
    bool Save();
    bool Save(std::string& fp);
    void SetButtonOnInterval(uint32_t msec);
    void SetButtonOnLongInterval(uint32_t msec);
    void SetCallbackProxy(f_Fn_proxy f);
    void SetCallbackStatus(f_Fn_status f);
    void SetCallbackInMonitor(f_Fn_monitor f);
    void RemoveCallbackStatus(f_Fn_status f);
    void SetAutoStart(bool b);
    void SetProxy(bool b);
    void SetInMonitor(bool b);
    void SetOutManualPort(bool b);

    bool IsStarted();
    bool IsProxy();
    bool IsEnable();
    bool IsConfig();
    bool IsInEnable();
    bool IsAutoStart();
    bool IsInMonitor();
    bool IsOutEnable();
    bool IsInConnect();
    bool IsOutConnect();
    bool IsOutManualPort();

    std::string InDeviceName();
    std::string OutDeviceName();
    std::vector<std::string> InDeviceList();
    std::shared_ptr<MidiDevice> GetConfig();
    bool SendToPort(MidiUnit* unit);
    bool SendToPort(DWORD d);

    // CALLBACK //

    static void MidiBridgeProc(MidiUnit* u);
};

