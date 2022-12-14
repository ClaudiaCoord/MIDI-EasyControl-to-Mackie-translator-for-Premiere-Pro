/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "..\pch.h"

class MidiControllerProxy {
private:
    const char * LogTag = "MIDI proxy port ";
    bool isEnable = false;

    std::vector<std::shared_ptr<VirtualMidi>> vmdevices_ptr{};
    std::shared_ptr<MidiDevice> config_ptr = nullptr;
    StatusDelegate& __status;

    static MidiControllerProxy *ctrl;

    void LogCallback(std::string s);

    MidiControllerProxy() = delete;

public:

    MidiControllerProxy(StatusDelegate & fstatus, std::shared_ptr<MidiDevice> cnf);
    ~MidiControllerProxy();
    bool Start();
    void Stop();
    void Dispose();
    bool IsEnable();
    void SetProxyCount(uint32_t i);
    uint32_t GetProxyCount();
    std::string DeviceName(uint32_t i);
    static void SendToPort(DWORD d);

};
