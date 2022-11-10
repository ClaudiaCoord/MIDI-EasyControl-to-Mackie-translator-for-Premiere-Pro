/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "..\pch.h"
#define _BOOLTOLOG(A,B) " " << A << "=" << (B) << ","
#define BOOLTOLOG(A) _BOOLTOLOG(#A, A ? "yes" : "no")

class LogString
{
public:
    static const char LogTagOpen[];
    static const char LogTagClose[];
    static const char LogTagDone[];
    static const char LogNotConnect[];
    static const char LogNotDefined[];
    static const char LogNotFound[];
    static const char LogNotOpen[];
    static const char LogOpen[];
    static const char LogFound[];
    static const char LogErrorOpen[];
    static const char LogErrorClose[];
    static const char LogErrorFault[];
    static const char LogAutoStart[];
    static const char LogAlreadyRun[];
    static const char LogNotRun[];
    static const char LogStart[];
    static const char LogStop[];
    static const char LogBadValues[];

    static const char LogVMError1[];
    static const char LogVMError2[];
    static const char LogVMError3[];
    static const char LogVMError4[];
    static const char LogVMError5[];
    static const char LogVMError6[];
    static const char LogVMError7[];
    static const char LogVMError8[];
    static const char LogVMError9[];
    static const char LogVMError10[];
    static const char LogVMError11[];
    
    template<class T>
    LogString& operator<< (const T& arg) {
        m_stream << arg;
        return *this;
    }
    operator std::string() const {
        return m_stream.str();
    }
protected:
    std::stringstream m_stream;
};

