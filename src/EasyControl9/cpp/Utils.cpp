/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#include "..\pch.h"

std::string Utils::BuildOutDeviceName(std::string ss, std::string se) {
    std::filesystem::path p = std::filesystem::path(ss);
    std::string s = p.stem().string().c_str();
    s.append(se);
    return s;
}
std::string Utils::ConvertCofigName(std::string ss) {
    std::filesystem::path p = std::filesystem::path(ss);
    return p.stem().string().c_str();
}
void Utils::ErrorMessage(std::runtime_error& err, f_Fn_status console) {
    if (console != nullptr) console(err.what());
}
void Utils::ErrorMessage(std::string err, f_Fn_status console) {
    if (console != nullptr) console(err);
}
const char* Utils::ErrorMessage(MMRESULT res) {
    if (res != MMSYSERR_NOERROR) {
        wchar_t txt[256]{};
        midiOutGetErrorTextW(res, txt, 256);
        std::wstring ws(txt);
        std::string s(ws.begin(), ws.end());
        return s.c_str();
    }
    return "";
}
