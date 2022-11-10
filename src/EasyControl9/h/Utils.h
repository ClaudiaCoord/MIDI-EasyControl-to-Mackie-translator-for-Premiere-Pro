/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "..\pch.h"

namespace Utils {

    /*
	std::string ConvertCofigName(std::string ss, std::string se) {
        std::filesystem::path p = std::filesystem::path(ss);
        std::string s = p.stem().string().c_str();
        s.append(se);
        s.append(p.extension().string().c_str());
        p.replace_filename(s.c_str());
        return p.string();
	}
    */
    std::string BuildOutDeviceName(std::string ss, std::string se);
    std::string ConvertCofigName(std::string ss);
    void ErrorMessage(std::runtime_error& err, f_Fn_status console);
    void ErrorMessage(std::string err, f_Fn_status console);
    const char* ErrorMessage(MMRESULT res);
}