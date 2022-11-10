/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "..\pch.h"

class MidiControllerIn {
private:
    const char * LogTag = "MIDI input device ";
    bool isEnable = false;
    bool isConnect = false;
    bool isMonitor = false;

    uint32_t __num_devices = 0;
    uint32_t __device_id = UINT_MAX;
    
    HMIDIIN __midi_in_handle = nullptr;
    
    f_Fn_event event = nullptr;
    f_Fn_proxy proxy = nullptr;
    f_Fn_monitor monitor = nullptr;
    StatusDelegate & __status;
    std::shared_ptr<MidiDevice> config_ptr = nullptr;
    static MidiControllerIn *ctrl;

    void Initializer();
    void Initializer(f_Fn_event faction);
    void LogCallback(std::string s);

    MidiControllerIn() = delete;

public:

    std::vector<std::string> DeviceList;

    MidiControllerIn(f_Fn_event faction, StatusDelegate& fstatus, std::shared_ptr<MidiDevice> cnf);
    ~MidiControllerIn();
    bool Start();
    void Stop();
    void Dispose();

    bool IsEnable();
    bool IsConnect();
    bool IsMonitor();
    void SetCallbackEvent(f_Fn_event f);
    void SetCallbackProxy(f_Fn_proxy f);
    void SetCallbackMonitor(f_Fn_monitor f);
    void SetMonitor(bool b);

    std::string DeviceName();
    std::vector<std::string> GetDeviceList();
    void RunCallbackEvent(uint8_t scene, uint8_t id, uint8_t v, uint32_t t);
    void MonitorCallbackEvent(uint8_t scene, uint8_t id, uint8_t v, uint32_t t);

    // CALLBACK //
    static void CALLBACK MidiInProc(HMIDIIN midi_in_handle, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
};
