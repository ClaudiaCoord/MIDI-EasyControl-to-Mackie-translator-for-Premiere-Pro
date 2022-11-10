/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "..\pch.h"

class MidiControllerOut {
private:
    const char* LogTag = "MIDI output device ";
    bool isEnable = false;
    bool isConnect = false;
    bool isManualPort = false;

    uint32_t __num_devices = 0;
    uint32_t __device_id = UINT_MAX;

    HMIDIOUT __midi_out_handle = nullptr;

    std::string deviceName{};
    StatusDelegate& __status;
    std::shared_ptr<MidiDevice> config_ptr = nullptr;
    std::shared_ptr <VirtualMidi> vmdevice_ptr = nullptr;
    static MidiControllerOut* ctrl;

    void Initializer();

    MidiControllerOut() = delete;

public:

    MidiControllerOut(StatusDelegate& fstatus, std::shared_ptr<MidiDevice> cnf);
    ~MidiControllerOut();
    bool Start();
    void Stop();
    void Dispose(bool b = true);
    bool IsEnable();
    bool IsConnect();
    bool IsManualPort();
    std::string DeviceName();
    void SetManualPort(bool b);
    void LogCallback(std::string s);
    bool SendToPort(MidiUnit* unit);
    bool SendToPort(DWORD d);

    // CALLBACK //

    static void CALLBACK MidiOutProc(HMIDIOUT __midi_out_handle, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
};

