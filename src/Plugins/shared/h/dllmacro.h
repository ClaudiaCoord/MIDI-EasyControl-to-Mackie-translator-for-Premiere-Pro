/*
	MIDI-MT plugins part.
	(c) CC 2023, MIT

	TEMPLATE MMTPLUGINx* DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

using namespace std::string_view_literals;

#if defined (_DEBUG)
constexpr std::wstring_view debug_trace_header_ = L"\t* Plugin call: "sv;
#	define TRACE_CALL()   ::OutputDebugStringW((std::wstring() + debug_trace_header_.data() + __FUNCTIONW__ + L"\n").c_str());
#	define TRACE_CALLX(A) ::OutputDebugStringW((std::wstring() + debug_trace_header_.data() + __FUNCTIONW__ + L" (" + A + L")\n").c_str());
#else
#	define TRACE_CALL()
#	define TRACE_CALLX(A)
#endif

extern "C" {
	extern HMODULE GetCurrentDllHinstance();
}

#include "..\..\Common\rc\resource_midimt.h"
