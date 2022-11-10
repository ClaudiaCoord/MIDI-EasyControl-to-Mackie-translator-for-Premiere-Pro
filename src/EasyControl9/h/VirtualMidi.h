/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "..\pch.h"

class VirtualMidi
{
private:
    const char* LogTag = "MIDI virtual device ";
    bool isEnable = false;

    LPVM_MIDI_PORT __midi_port = nullptr;

    std::wstring deviceName{};
    StatusDelegate & __status;
    static VirtualMidi* ctrl;

public:

    VirtualMidi(StatusDelegate & fstatus, std::string & devname);
    ~VirtualMidi();
    bool Start();
    void Stop();
    void Dispose();
    bool IsEnable();
    std::string DeviceName();
    bool SendToPort(Mackie::MIDIDATA & m);
    void SetCallbackStatus(f_Fn_status f);
    void LogCallback(std::string s);
};

