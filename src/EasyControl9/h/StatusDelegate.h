/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "..\pch.h"

class StatusDelegate
{
    std::vector<std::function<void(const std::string)>> funcs;
    size_t getFunctionAddress(std::function<void(std::string)> f) {
        typedef void(fnType)(std::string);
        fnType** fp = f.template target<fnType*>();
        return (size_t)*fp;
    }

public:
    template<class T> StatusDelegate& operator+=(T mFunc) { funcs.push_back(mFunc); return *this; }
    template<class T> StatusDelegate& operator-=(T mFunc) {
        for (auto iter = funcs.begin(); iter != funcs.end(); ++iter) {
            if (getFunctionAddress(*iter) == getFunctionAddress(mFunc)) {
                funcs.erase(iter);
                break;
            }
        }
        return *this;
    }
    void operator()(const std::string s) { for (auto& f : funcs) f(s); }
};

