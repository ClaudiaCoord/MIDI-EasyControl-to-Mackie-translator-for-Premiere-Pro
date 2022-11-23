/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2023, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once


#include "..\framework.h"
using namespace std::string_view_literals;

constexpr std::wstring_view TypeFADER = L"FADER"sv;
constexpr std::wstring_view TypeSLIDER = L"SLIDER"sv;
constexpr std::wstring_view TypeBUTTON = L"BUTTON"sv;
constexpr std::wstring_view TypeBUTTONT = L"BUTTON TOGGLE"sv;
constexpr std::wstring_view TypeKNOB = L"KNOB"sv;
constexpr std::wstring_view TypeKNOBI = L"KNOB INVERSE"sv;

constexpr std::wstring_view TypeNONE = L"control not defined"sv;
constexpr std::wstring_view SceneNONE = L"not scene selected"sv;
constexpr std::wstring_view TargetNONE = L"control not defined"sv;

constexpr std::wstring_view TargetMAV = L"audio volume (master)"sv;
constexpr std::wstring_view TargetMAP = L"audio balance (master)"sv;
constexpr std::wstring_view TargetMAM = L"audio mute (master)"sv;
constexpr std::wstring_view TargetMAS = L"audio solo (master)"sv;
constexpr std::wstring_view TargetJOG = L"JOG (video frame rewind)"sv;

constexpr std::wstring_view TargetAV1 = L"audio volume (channel 1)"sv;
constexpr std::wstring_view TargetAV2 = L"audio volume (channel 2)"sv;
constexpr std::wstring_view TargetAV3 = L"audio volume (channel 3)"sv;
constexpr std::wstring_view TargetAV4 = L"audio volume (channel 4)"sv;
constexpr std::wstring_view TargetAV5 = L"audio volume (channel 5)"sv;
constexpr std::wstring_view TargetAV6 = L"audio volume (channel 6)"sv;
constexpr std::wstring_view TargetAV7 = L"audio volume (channel 7)"sv;
constexpr std::wstring_view TargetAV8 = L"audio volume (channel 8)"sv;
constexpr std::wstring_view TargetAV9 = L"audio volume (channel 9)"sv;

constexpr std::wstring_view TargetAP1 = L"audio balance (channel 1)"sv;
constexpr std::wstring_view TargetAP2 = L"audio balance (channel 2)"sv;
constexpr std::wstring_view TargetAP3 = L"audio balance (channel 3)"sv;
constexpr std::wstring_view TargetAP4 = L"audio balance (channel 4)"sv;
constexpr std::wstring_view TargetAP5 = L"audio balance (channel 5)"sv;
constexpr std::wstring_view TargetAP6 = L"audio balance (channel 6)"sv;
constexpr std::wstring_view TargetAP7 = L"audio balance (channel 7)"sv;
constexpr std::wstring_view TargetAP8 = L"audio balance (channel 8)"sv;
constexpr std::wstring_view TargetAP9 = L"audio balance (channel 9)"sv;

constexpr std::wstring_view TargetB11 = L"audio solo (channel 1)"sv;
constexpr std::wstring_view TargetB12 = L"audio solo (channel 2)"sv;
constexpr std::wstring_view TargetB13 = L"audio solo (channel 3)"sv;
constexpr std::wstring_view TargetB14 = L"audio solo (channel 4)"sv;
constexpr std::wstring_view TargetB15 = L"audio solo (channel 5)"sv;
constexpr std::wstring_view TargetB16 = L"audio solo (channel 6)"sv;
constexpr std::wstring_view TargetB17 = L"audio solo (channel 7)"sv;
constexpr std::wstring_view TargetB18 = L"audio solo (channel 8)"sv;
constexpr std::wstring_view TargetB19 = L"audio solo (channel 9)"sv;

constexpr std::wstring_view TargetB21 = L"audio mute (channel 1)"sv;
constexpr std::wstring_view TargetB22 = L"audio mute (channel 2)"sv;
constexpr std::wstring_view TargetB23 = L"audio mute (channel 3)"sv;
constexpr std::wstring_view TargetB24 = L"audio mute (channel 4)"sv;
constexpr std::wstring_view TargetB25 = L"audio mute (channel 5)"sv;
constexpr std::wstring_view TargetB26 = L"audio mute (channel 6)"sv;
constexpr std::wstring_view TargetB27 = L"audio mute (channel 7)"sv;
constexpr std::wstring_view TargetB28 = L"audio mute (channel 8)"sv;
constexpr std::wstring_view TargetB29 = L"audio mute (channel 9)"sv;

constexpr std::wstring_view TargetB31 = L"audio select (channel 1)"sv;
constexpr std::wstring_view TargetB32 = L"audio select (channel 2)"sv;
constexpr std::wstring_view TargetB33 = L"audio select (channel 3)"sv;
constexpr std::wstring_view TargetB34 = L"audio select (channel 4)"sv;
constexpr std::wstring_view TargetB35 = L"audio select (channel 5)"sv;
constexpr std::wstring_view TargetB36 = L"audio select (channel 6)"sv;
constexpr std::wstring_view TargetB37 = L"audio select (channel 7)"sv;
constexpr std::wstring_view TargetB38 = L"audio select (channel 8)"sv;
constexpr std::wstring_view TargetB39 = L"audio select (channel 9)"sv;

constexpr std::wstring_view TargetFUN11 = L"functions F1 (standard mode)"sv;
constexpr std::wstring_view TargetFUN12 = L"functions F2 (standard mode)"sv;
constexpr std::wstring_view TargetFUN13 = L"functions F3 (standard mode)"sv;
constexpr std::wstring_view TargetFUN14 = L"functions F4 (standard mode)"sv;
constexpr std::wstring_view TargetFUN15 = L"functions F5 (standard mode)"sv;
constexpr std::wstring_view TargetFUN16 = L"functions F6 (standard mode)"sv;
constexpr std::wstring_view TargetFUN17 = L"functions F7 (standard mode)"sv;
constexpr std::wstring_view TargetFUN18 = L"functions F8 (standard mode)"sv;

constexpr std::wstring_view TargetFUN21 = L"functions F1 (held down mode)"sv;
constexpr std::wstring_view TargetFUN22 = L"functions F2 (held down mode)"sv;
constexpr std::wstring_view TargetFUN23 = L"functions F3 (held down mode)"sv;
constexpr std::wstring_view TargetFUN24 = L"functions F4 (held down mode)"sv;
constexpr std::wstring_view TargetFUN25 = L"functions F5 (held down mode)"sv;
constexpr std::wstring_view TargetFUN26 = L"functions F6 (held down mode)"sv;
constexpr std::wstring_view TargetFUN27 = L"functions F7 (held down mode)"sv;
constexpr std::wstring_view TargetFUN28 = L"functions F8 (held down mode)"sv;

constexpr std::wstring_view TargetRewind = L"Rewind (player)"sv;
constexpr std::wstring_view TargetForward = L"Forward (player)"sv;
constexpr std::wstring_view TargetStop = L"Stop (player)"sv;
constexpr std::wstring_view TargetPlay = L"Play (player)"sv;
constexpr std::wstring_view TargetRecord = L"Record"sv;
constexpr std::wstring_view TargetUp = L"Up"sv;
constexpr std::wstring_view TargetDown = L"Down"sv;
constexpr std::wstring_view TargetLeft = L"Left"sv;
constexpr std::wstring_view TargetRight = L"Right"sv;
constexpr std::wstring_view TargetZoom = L"Zoom"sv;
constexpr std::wstring_view TargetScrub = L"Scrub"sv;

class FLAG_EXPORT MackieHelper
{
public:
	static std::wstring_view GetType(uint32_t i) {
		switch (i) {
			case 0: return TypeFADER;
			case 1: return TypeSLIDER;
			case 2: return TypeKNOB;
			case 3: return TypeBUTTON;
			case 4: return TypeBUTTONT;
			case 5: return TypeKNOBI;
			default: break;
		}
		return TypeNONE;
	}
	static std::wstring_view GetScene(uint32_t i) {
		switch (i) {
			case 176: return L"1";
			case 177: return L"2";
			case 178: return L"3";
			case 179: return L"4";
			case 192: return L"1";
			case 193: return L"2";
			case 194: return L"3";
			case 211: return L"4";
			case 243: return L"4";
			default: break;
		}
		return SceneNONE;
	}
	static std::wstring_view GetTarget(uint32_t i) {
		switch (i) {
			case 0: return TargetMAV;
			case 1: return TargetMAP;
			case 2: return TargetMAM;
			case 3: return TargetMAS;
			case 4: return TargetJOG;

			case 5: return TargetAV1;
			case 6: return TargetAV2;
			case 7: return TargetAV3;
			case 8: return TargetAV4;
			case 9: return TargetAV5;
			case 10: return TargetAV6;
			case 11: return TargetAV7;
			case 12: return TargetAV8;
			case 13: return TargetAV9;

			case 14: return TargetAP1;
			case 15: return TargetAP2;
			case 16: return TargetAP3;
			case 17: return TargetAP4;
			case 18: return TargetAP5;
			case 19: return TargetAP6;
			case 20: return TargetAP7;
			case 21: return TargetAP8;
			case 22: return TargetAP9;

			case 23: return TargetB11;
			case 24: return TargetB12;
			case 25: return TargetB13;
			case 26: return TargetB14;
			case 27: return TargetB15;
			case 28: return TargetB16;
			case 29: return TargetB17;
			case 30: return TargetB18;
			case 31: return TargetB19;

			case 32: return TargetB21;
			case 33: return TargetB22;
			case 34: return TargetB23;
			case 35: return TargetB24;
			case 36: return TargetB25;
			case 37: return TargetB26;
			case 38: return TargetB27;
			case 39: return TargetB28;
			case 40: return TargetB29;

			case 41: return TargetB31;
			case 42: return TargetB32;
			case 43: return TargetB33;
			case 44: return TargetB34;
			case 45: return TargetB35;
			case 46: return TargetB36;
			case 47: return TargetB37;
			case 48: return TargetB38;
			case 49: return TargetB39;

			case 50: return TargetFUN11;
			case 51: return TargetFUN12;
			case 52: return TargetFUN13;
			case 53: return TargetFUN14;
			case 54: return TargetFUN15;
			case 55: return TargetFUN16;
			case 56: return TargetFUN17;
			case 57: return TargetFUN18;

			case 58: return TargetFUN21;
			case 59: return TargetFUN22;
			case 60: return TargetFUN23;
			case 61: return TargetFUN24;
			case 62: return TargetFUN25;
			case 63: return TargetFUN26;
			case 64: return TargetFUN27;
			case 65: return TargetFUN28;

			case 66: return TargetRewind;
			case 67: return TargetForward;
			case 68: return TargetStop;
			case 69: return TargetPlay;
			case 70: return TargetRecord;
			case 71: return TargetUp;
			case 72: return TargetDown;
			case 73: return TargetLeft;
			case 74: return TargetRight;
			case 75: return TargetZoom;
			case 76: return TargetScrub;

			default: break;
		}
		return TargetNONE;
	}
};

