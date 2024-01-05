/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once


#include "global.h"

namespace Common {
	namespace MIDI {

		using namespace std::string_view_literals;

		#pragma region strings view
		constexpr std::wstring_view TargetNONE = L"not action defined"sv;

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
		constexpr std::wstring_view TargetXV9 = L"audio volume (channel 9)"sv;

		constexpr std::wstring_view TargetAP1 = L"audio balance (channel 1)"sv;
		constexpr std::wstring_view TargetAP2 = L"audio balance (channel 2)"sv;
		constexpr std::wstring_view TargetAP3 = L"audio balance (channel 3)"sv;
		constexpr std::wstring_view TargetAP4 = L"audio balance (channel 4)"sv;
		constexpr std::wstring_view TargetAP5 = L"audio balance (channel 5)"sv;
		constexpr std::wstring_view TargetAP6 = L"audio balance (channel 6)"sv;
		constexpr std::wstring_view TargetAP7 = L"audio balance (channel 7)"sv;
		constexpr std::wstring_view TargetAP8 = L"audio balance (channel 8)"sv;
		constexpr std::wstring_view TargetXP9 = L"audio balance (channel 9)"sv;

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
		constexpr std::wstring_view TargetRecord = L"Record (player)"sv;
		constexpr std::wstring_view TargetUp = L"Up"sv;
		constexpr std::wstring_view TargetDown = L"Down"sv;
		constexpr std::wstring_view TargetLeft = L"Left"sv;
		constexpr std::wstring_view TargetRight = L"Right"sv;
		constexpr std::wstring_view TargetZoom = L"Zoom"sv;
		constexpr std::wstring_view TargetScrub = L"Scrub"sv;

		constexpr std::wstring_view TargetMMKScrub = L"LAUNCH MEDIA SELECT"sv;
		constexpr std::wstring_view TargetMMKZoom = L"ZOOM"sv;
		constexpr std::wstring_view TargetMMKRecord = L"PLAY"sv;
		constexpr std::wstring_view TargetMMKRewind = L"MEDIA PREV TRACK"sv;
		constexpr std::wstring_view TargetMMKForward = L"MEDIA NEXT TRACK"sv;
		constexpr std::wstring_view TargetMMKStop = L"MEDIA STOP"sv;
		constexpr std::wstring_view TargetMMKPlay = L"MEDIA PLAY/PAUSE"sv;
		constexpr std::wstring_view TargetMMKUp = L"VOLUME UP"sv;
		constexpr std::wstring_view TargetMMKDown = L"VOLUME DOWN"sv;
		constexpr std::wstring_view TargetMMKLeft = L"VOLUME MUTE"sv;
		constexpr std::wstring_view TargetMMKRight = L"PAUSE"sv;

		constexpr std::wstring_view TargetLightKey8 = L"Lighting DMX controls (8 bits)"sv;
		constexpr std::wstring_view TargetLightKey16 = L"Lighting DMX controls (16 bits)"sv;
		constexpr std::wstring_view TargetMixer = L"System mixer volume/mute"sv;
		constexpr std::wstring_view TargetMediaKey = L"System multimedia key"sv;
		constexpr std::wstring_view TargetMqttKey = L"Smart House key (MQTT)"sv;

		constexpr std::wstring_view TypeClickOnce = L"|Once|"sv;
		constexpr std::wstring_view TypeClickLong = L"|Long|"sv;
		constexpr std::wstring_view TypeClickTrigger = L"|Trigger|"sv;
		constexpr std::wstring_view TypeClickSlider = L"ClickSlider"sv;
		constexpr std::wstring_view TypeClickNONE = L"|none|"sv;
		#pragma endregion

		std::wstring_view getClickType(Mackie::ClickType t) {
			switch (t) {
				using enum Mackie::ClickType;
				case ClickOnce: return TypeClickOnce;
				case ClickLong: return TypeClickLong;
				case ClickTrigger: return TypeClickTrigger;
				case ClickSlider: return TypeClickSlider;
				case ClickUnknown:
				default: return TypeClickNONE;
			}
		}
		std::wstring_view MackieHelper::GetClickType(Mackie::ClickType& t) {
			return getClickType(t);
		}
		std::wstring_view MackieHelper::GetClickType(uint8_t& u) {
			return getClickType(static_cast<Mackie::ClickType>(u));
		}
		std::wstring_view MackieHelper::GetClickType(uint32_t u) {
			return getClickType(static_cast<Mackie::ClickType>(u));
		}

		int32_t MackieHelper::GetSmartHomeTargetID(Mackie::Target& t) {
			switch (t) {
				using enum Mackie::Target;
				case AV1: return IDM_COL4_MQTT_AV1;
				case AV2: return IDM_COL4_MQTT_AV2;
				case AV3: return IDM_COL4_MQTT_AV3;
				case AV4: return IDM_COL4_MQTT_AV4;
				case AV5: return IDM_COL4_MQTT_AV5;
				case AV6: return IDM_COL4_MQTT_AV6;
				case AV7: return IDM_COL4_MQTT_AV7;
				case AV8: return IDM_COL4_MQTT_AV8;
				case XV9: return IDM_COL4_MQTT_XV9;

				case AP1: return IDM_COL4_MQTT_AP1;
				case AP2: return IDM_COL4_MQTT_AP2;
				case AP3: return IDM_COL4_MQTT_AP3;
				case AP4: return IDM_COL4_MQTT_AP4;
				case AP5: return IDM_COL4_MQTT_AP5;
				case AP6: return IDM_COL4_MQTT_AP6;
				case AP7: return IDM_COL4_MQTT_AP7;
				case AP8: return IDM_COL4_MQTT_AP8;
				case XP9: return IDM_COL4_MQTT_XP9;

				case B11: return IDM_COL4_MQTT_B11;
				case B12: return IDM_COL4_MQTT_B12;
				case B13: return IDM_COL4_MQTT_B13;
				case B14: return IDM_COL4_MQTT_B14;
				case B15: return IDM_COL4_MQTT_B15;
				case B16: return IDM_COL4_MQTT_B16;
				case B17: return IDM_COL4_MQTT_B17;
				case B18: return IDM_COL4_MQTT_B18;
				case B19: return IDM_COL4_MQTT_B19;

				case B21: return IDM_COL4_MQTT_B21;
				case B22: return IDM_COL4_MQTT_B22;
				case B23: return IDM_COL4_MQTT_B23;
				case B24: return IDM_COL4_MQTT_B24;
				case B25: return IDM_COL4_MQTT_B25;
				case B26: return IDM_COL4_MQTT_B26;
				case B27: return IDM_COL4_MQTT_B27;
				case B28: return IDM_COL4_MQTT_B28;
				case B29: return IDM_COL4_MQTT_B29;

				case B31: return IDM_COL4_MQTT_B31;
				case B32: return IDM_COL4_MQTT_B32;
				case B33: return IDM_COL4_MQTT_B33;
				case B34: return IDM_COL4_MQTT_B34;
				case B35: return IDM_COL4_MQTT_B35;
				case B36: return IDM_COL4_MQTT_B36;
				case B37: return IDM_COL4_MQTT_B37;
				case B38: return IDM_COL4_MQTT_B38;
				case B39: return IDM_COL4_MQTT_B39;
				default: break;
			}
			return IDM_COL4_MQTT_NONE;
		}
		int32_t MackieHelper::GetTargetID(Mackie::Target& t) {
			switch (t) {
				using enum Mackie::Target;
				case MAV: return IDM_COL4_MAV;
				case MAP: return IDM_COL4_MAP;
				case MAM: return IDM_COL4_MAM;
				case MAS: return IDM_COL4_MAS;
				case JOG: return IDM_COL4_JOG;

				case AV1: return IDM_COL4_AV1;
				case AV2: return IDM_COL4_AV2;
				case AV3: return IDM_COL4_AV3;
				case AV4: return IDM_COL4_AV4;
				case AV5: return IDM_COL4_AV5;
				case AV6: return IDM_COL4_AV6;
				case AV7: return IDM_COL4_AV7;
				case AV8: return IDM_COL4_AV8;
				case XV9: return IDM_COL4_XV9;

				case AP1: return IDM_COL4_AP1;
				case AP2: return IDM_COL4_AP2;
				case AP3: return IDM_COL4_AP3;
				case AP4: return IDM_COL4_AP4;
				case AP5: return IDM_COL4_AP5;
				case AP6: return IDM_COL4_AP6;
				case AP7: return IDM_COL4_AP7;
				case AP8: return IDM_COL4_AP8;
				case XP9: return IDM_COL4_XP9;

				case B11: return IDM_COL4_B11;
				case B12: return IDM_COL4_B12;
				case B13: return IDM_COL4_B13;
				case B14: return IDM_COL4_B14;
				case B15: return IDM_COL4_B15;
				case B16: return IDM_COL4_B16;
				case B17: return IDM_COL4_B17;
				case B18: return IDM_COL4_B18;
				case B19: return IDM_COL4_B19;

				case B21: return IDM_COL4_B21;
				case B22: return IDM_COL4_B22;
				case B23: return IDM_COL4_B23;
				case B24: return IDM_COL4_B24;
				case B25: return IDM_COL4_B25;
				case B26: return IDM_COL4_B26;
				case B27: return IDM_COL4_B27;
				case B28: return IDM_COL4_B28;
				case B29: return IDM_COL4_B29;

				case B31: return IDM_COL4_B31;
				case B32: return IDM_COL4_B32;
				case B33: return IDM_COL4_B33;
				case B34: return IDM_COL4_B34;
				case B35: return IDM_COL4_B35;
				case B36: return IDM_COL4_B36;
				case B37: return IDM_COL4_B37;
				case B38: return IDM_COL4_B38;
				case B39: return IDM_COL4_B39;

				case FUN11: return IDM_COL4_FUN11;
				case FUN12: return IDM_COL4_FUN12;
				case FUN13: return IDM_COL4_FUN13;
				case FUN14: return IDM_COL4_FUN14;
				case FUN15: return IDM_COL4_FUN15;
				case FUN16: return IDM_COL4_FUN16;
				case FUN17: return IDM_COL4_FUN17;
				case FUN18: return IDM_COL4_FUN18;

				case FUN21: return IDM_COL4_FUN21;
				case FUN22: return IDM_COL4_FUN22;
				case FUN23: return IDM_COL4_FUN23;
				case FUN24: return IDM_COL4_FUN24;
				case FUN25: return IDM_COL4_FUN25;
				case FUN26: return IDM_COL4_FUN26;
				case FUN27: return IDM_COL4_FUN27;
				case FUN28: return IDM_COL4_FUN28;

				case SYS_Rewind:	return IDM_COL4_SYS_Rewind;
				case SYS_Forward:	return IDM_COL4_SYS_Forward;
				case SYS_Stop:		return IDM_COL4_SYS_Stop;
				case SYS_Play:		return IDM_COL4_SYS_Play;
				case SYS_Record:	return IDM_COL4_SYS_Record;
				case SYS_Up:		return IDM_COL4_SYS_Up;
				case SYS_Down:		return IDM_COL4_SYS_Down;
				case SYS_Left:		return IDM_COL4_SYS_Left;
				case SYS_Right:		return IDM_COL4_SYS_Right;
				case SYS_Zoom:		return IDM_COL4_SYS_Zoom;
				case SYS_Scrub:		return IDM_COL4_SYS_Scrub;

				case LIGHTKEY16B:	return IDM_COL4_LIGHTKEY16B;
				case LIGHTKEY8B:	return IDM_COL4_LIGHTKEY8B;
				case MQTTKEY:		return IDM_COL4_MQTTKEY;
				case MEDIAKEY:		return IDM_COL4_MEDIAKEY;
				case VOLUMEMIX:		return IDM_COL4_VOLUMEMIX;
				default: break;
			}
			return IDM_COL4_NONE;
		}

		std::wstring_view getTarget(Mackie::Target t) {
			switch (t) {
				using enum Mackie::Target;
				case MAV: return TargetMAV;
				case MAP: return TargetMAP;
				case MAM: return TargetMAM;
				case MAS: return TargetMAS;
				case JOG: return TargetJOG;

				case AV1: return TargetAV1;
				case AV2: return TargetAV2;
				case AV3: return TargetAV3;
				case AV4: return TargetAV4;
				case AV5: return TargetAV5;
				case AV6: return TargetAV6;
				case AV7: return TargetAV7;
				case AV8: return TargetAV8;
				case XV9: return TargetXV9;

				case AP1: return TargetAP1;
				case AP2: return TargetAP2;
				case AP3: return TargetAP3;
				case AP4: return TargetAP4;
				case AP5: return TargetAP5;
				case AP6: return TargetAP6;
				case AP7: return TargetAP7;
				case AP8: return TargetAP8;
				case XP9: return TargetXP9;

				case B11: return TargetB11;
				case B12: return TargetB12;
				case B13: return TargetB13;
				case B14: return TargetB14;
				case B15: return TargetB15;
				case B16: return TargetB16;
				case B17: return TargetB17;
				case B18: return TargetB18;
				case B19: return TargetB19;

				case B21: return TargetB21;
				case B22: return TargetB22;
				case B23: return TargetB23;
				case B24: return TargetB24;
				case B25: return TargetB25;
				case B26: return TargetB26;
				case B27: return TargetB27;
				case B28: return TargetB28;
				case B29: return TargetB29;

				case B31: return TargetB31;
				case B32: return TargetB32;
				case B33: return TargetB33;
				case B34: return TargetB34;
				case B35: return TargetB35;
				case B36: return TargetB36;
				case B37: return TargetB37;
				case B38: return TargetB38;
				case B39: return TargetB39;

				case FUN11: return TargetFUN11;
				case FUN12: return TargetFUN12;
				case FUN13: return TargetFUN13;
				case FUN14: return TargetFUN14;
				case FUN15: return TargetFUN15;
				case FUN16: return TargetFUN16;
				case FUN17: return TargetFUN17;
				case FUN18: return TargetFUN18;

				case FUN21: return TargetFUN21;
				case FUN22: return TargetFUN22;
				case FUN23: return TargetFUN23;
				case FUN24: return TargetFUN24;
				case FUN25: return TargetFUN25;
				case FUN26: return TargetFUN26;
				case FUN27: return TargetFUN27;
				case FUN28: return TargetFUN28;

				case SYS_Rewind: return TargetRewind;
				case SYS_Forward: return TargetForward;
				case SYS_Stop: return TargetStop;
				case SYS_Play: return TargetPlay;
				case SYS_Record: return TargetRecord;
				case SYS_Up: return TargetUp;
				case SYS_Down: return TargetDown;
				case SYS_Left: return TargetLeft;
				case SYS_Right: return TargetRight;
				case SYS_Zoom: return TargetZoom;
				case SYS_Scrub: return TargetScrub;

				case LIGHTKEY16B: return TargetLightKey16;
				case LIGHTKEY8B: return TargetLightKey8;
				case MQTTKEY: return TargetMqttKey;
				case MEDIAKEY: return TargetMediaKey;
				case VOLUMEMIX: return TargetMixer;
				default: break;
			}
			return TargetNONE;
		}
		std::wstring_view MackieHelper::GetTarget(Mackie::Target& t) {
			return getTarget(t);
		}
		std::wstring_view MackieHelper::GetTarget(uint8_t& u) {
			return getTarget(static_cast<Mackie::Target>(u));
		}
		std::wstring_view MackieHelper::GetTarget(uint32_t u) {
			return getTarget(static_cast<Mackie::Target>(u));
		}

		std::wstring_view getTranslateMMKey(Mackie::Target t) {
			switch (t) {
				using enum Mackie::Target;
				case SYS_Rewind: return TargetMMKRewind;
				case SYS_Forward: return TargetMMKForward;
				case SYS_Stop: return TargetMMKStop;
				case SYS_Play: return TargetMMKPlay;
				case SYS_Record: return TargetMMKRecord;
				case SYS_Up: return TargetMMKUp;
				case SYS_Down: return TargetMMKDown;
				case SYS_Left: return TargetMMKLeft;
				case SYS_Right: return TargetMMKRight;
				case SYS_Zoom: return TargetMMKZoom;
				case SYS_Scrub: return TargetMMKScrub;

				default: break;
			}
			return TargetNONE;
		}
		std::wstring_view MackieHelper::GetTranslateMMKey(Mackie::Target& t) {
			return getTranslateMMKey(static_cast<Mackie::Target>(t));
		}
		std::wstring_view MackieHelper::GetTranslateMMKey(uint8_t& u) {
			return getTranslateMMKey(static_cast<Mackie::Target>(u));
		}
		std::wstring_view MackieHelper::GetTranslateMMKey(uint32_t u) {
			return getTranslateMMKey(static_cast<Mackie::Target>(u));
		}
	}
}
