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

    LPVM_MIDI_PORT __midi_port = nullptr;
    StatusDelegate& __status;

    std::shared_ptr<MidiDevice> config_ptr = nullptr;
    std::shared_ptr <VirtualMidi> vmdevice_ptr = nullptr;
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
    std::string DeviceName();
    static void SendToPort(DWORD d);

};
