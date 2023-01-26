/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once


#include "global.h"

namespace Common {
	namespace MIDI {

		using namespace std::string_view_literals;

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

		constexpr std::wstring_view TargetMixer = L"System mixer volume/mute"sv;
		constexpr std::wstring_view TargetMediaKey = L"System multimedia key"sv;
		constexpr std::wstring_view TargetMqttKey = L"Smart House key (MQTT)"sv;

		constexpr std::wstring_view TypeClickOnce = L"|Once|"sv;
		constexpr std::wstring_view TypeClickLong = L"|Long|"sv;
		constexpr std::wstring_view TypeClickTrigger = L"|Trigger|"sv;
		constexpr std::wstring_view TypeClickSlider = L"ClickSlider"sv;
		constexpr std::wstring_view TypeClickNONE = L"|none|"sv;

		std::wstring_view getClickType(Common::MIDI::Mackie::ClickType t) {
			switch (t) {
				case Common::MIDI::Mackie::ClickType::ClickOnce: return TypeClickOnce;
				case Common::MIDI::Mackie::ClickType::ClickLong: return TypeClickLong;
				case Common::MIDI::Mackie::ClickType::ClickTrigger: return TypeClickTrigger;
				case Common::MIDI::Mackie::ClickType::ClickSlider: return TypeClickSlider;
				case Common::MIDI::Mackie::ClickType::ClickUnknown:
				default: return TypeClickNONE;
			}
		}
		std::wstring_view MackieHelper::GetClickType(Common::MIDI::Mackie::ClickType& t) {
			return getClickType(t);
		}
		std::wstring_view MackieHelper::GetClickType(uint8_t& u) {
			return getClickType(static_cast<Common::MIDI::Mackie::ClickType>(u));
		}
		std::wstring_view MackieHelper::GetClickType(uint32_t u) {
			return getClickType(static_cast<Common::MIDI::Mackie::ClickType>(u));
		}

		int32_t MackieHelper::GetSmartHomeTargetID(Common::MIDI::Mackie::Target& t) {
			switch (t) {
				case Mackie::Target::AV1: return IDM_COL4_MQTT_AV1;
				case Mackie::Target::AV2: return IDM_COL4_MQTT_AV2;
				case Mackie::Target::AV3: return IDM_COL4_MQTT_AV3;
				case Mackie::Target::AV4: return IDM_COL4_MQTT_AV4;
				case Mackie::Target::AV5: return IDM_COL4_MQTT_AV5;
				case Mackie::Target::AV6: return IDM_COL4_MQTT_AV6;
				case Mackie::Target::AV7: return IDM_COL4_MQTT_AV7;
				case Mackie::Target::AV8: return IDM_COL4_MQTT_AV8;
				case Mackie::Target::XV9: return IDM_COL4_MQTT_XV9;

				case Mackie::Target::AP1: return IDM_COL4_MQTT_AP1;
				case Mackie::Target::AP2: return IDM_COL4_MQTT_AP2;
				case Mackie::Target::AP3: return IDM_COL4_MQTT_AP3;
				case Mackie::Target::AP4: return IDM_COL4_MQTT_AP4;
				case Mackie::Target::AP5: return IDM_COL4_MQTT_AP5;
				case Mackie::Target::AP6: return IDM_COL4_MQTT_AP6;
				case Mackie::Target::AP7: return IDM_COL4_MQTT_AP7;
				case Mackie::Target::AP8: return IDM_COL4_MQTT_AP8;
				case Mackie::Target::XP9: return IDM_COL4_MQTT_XP9;

				case Mackie::Target::B11: return IDM_COL4_MQTT_B11;
				case Mackie::Target::B12: return IDM_COL4_MQTT_B12;
				case Mackie::Target::B13: return IDM_COL4_MQTT_B13;
				case Mackie::Target::B14: return IDM_COL4_MQTT_B14;
				case Mackie::Target::B15: return IDM_COL4_MQTT_B15;
				case Mackie::Target::B16: return IDM_COL4_MQTT_B16;
				case Mackie::Target::B17: return IDM_COL4_MQTT_B17;
				case Mackie::Target::B18: return IDM_COL4_MQTT_B18;
				case Mackie::Target::B19: return IDM_COL4_MQTT_B19;

				case Mackie::Target::B21: return IDM_COL4_MQTT_B21;
				case Mackie::Target::B22: return IDM_COL4_MQTT_B22;
				case Mackie::Target::B23: return IDM_COL4_MQTT_B23;
				case Mackie::Target::B24: return IDM_COL4_MQTT_B24;
				case Mackie::Target::B25: return IDM_COL4_MQTT_B25;
				case Mackie::Target::B26: return IDM_COL4_MQTT_B26;
				case Mackie::Target::B27: return IDM_COL4_MQTT_B27;
				case Mackie::Target::B28: return IDM_COL4_MQTT_B28;
				case Mackie::Target::B29: return IDM_COL4_MQTT_B29;

				case Mackie::Target::B31: return IDM_COL4_MQTT_B31;
				case Mackie::Target::B32: return IDM_COL4_MQTT_B32;
				case Mackie::Target::B33: return IDM_COL4_MQTT_B33;
				case Mackie::Target::B34: return IDM_COL4_MQTT_B34;
				case Mackie::Target::B35: return IDM_COL4_MQTT_B35;
				case Mackie::Target::B36: return IDM_COL4_MQTT_B36;
				case Mackie::Target::B37: return IDM_COL4_MQTT_B37;
				case Mackie::Target::B38: return IDM_COL4_MQTT_B38;
				case Mackie::Target::B39: return IDM_COL4_MQTT_B39;
				default: break;
			}
			return IDM_COL4_MQTT_NONE;
		}
		int32_t MackieHelper::GetTargetID(Common::MIDI::Mackie::Target& t) {
			switch (t) {
				case Mackie::Target::MAV: return IDM_COL4_MAV;
				case Mackie::Target::MAP: return IDM_COL4_MAP;
				case Mackie::Target::MAM: return IDM_COL4_MAM;
				case Mackie::Target::MAS: return IDM_COL4_MAS;
				case Mackie::Target::JOG: return IDM_COL4_JOG;

				case Mackie::Target::AV1: return IDM_COL4_AV1;
				case Mackie::Target::AV2: return IDM_COL4_AV2;
				case Mackie::Target::AV3: return IDM_COL4_AV3;
				case Mackie::Target::AV4: return IDM_COL4_AV4;
				case Mackie::Target::AV5: return IDM_COL4_AV5;
				case Mackie::Target::AV6: return IDM_COL4_AV6;
				case Mackie::Target::AV7: return IDM_COL4_AV7;
				case Mackie::Target::AV8: return IDM_COL4_AV8;
				case Mackie::Target::XV9: return IDM_COL4_XV9;

				case Mackie::Target::AP1: return IDM_COL4_AP1;
				case Mackie::Target::AP2: return IDM_COL4_AP2;
				case Mackie::Target::AP3: return IDM_COL4_AP3;
				case Mackie::Target::AP4: return IDM_COL4_AP4;
				case Mackie::Target::AP5: return IDM_COL4_AP5;
				case Mackie::Target::AP6: return IDM_COL4_AP6;
				case Mackie::Target::AP7: return IDM_COL4_AP7;
				case Mackie::Target::AP8: return IDM_COL4_AP8;
				case Mackie::Target::XP9: return IDM_COL4_XP9;

				case Mackie::Target::B11: return IDM_COL4_B11;
				case Mackie::Target::B12: return IDM_COL4_B12;
				case Mackie::Target::B13: return IDM_COL4_B13;
				case Mackie::Target::B14: return IDM_COL4_B14;
				case Mackie::Target::B15: return IDM_COL4_B15;
				case Mackie::Target::B16: return IDM_COL4_B16;
				case Mackie::Target::B17: return IDM_COL4_B17;
				case Mackie::Target::B18: return IDM_COL4_B18;
				case Mackie::Target::B19: return IDM_COL4_B19;

				case Mackie::Target::B21: return IDM_COL4_B21;
				case Mackie::Target::B22: return IDM_COL4_B22;
				case Mackie::Target::B23: return IDM_COL4_B23;
				case Mackie::Target::B24: return IDM_COL4_B24;
				case Mackie::Target::B25: return IDM_COL4_B25;
				case Mackie::Target::B26: return IDM_COL4_B26;
				case Mackie::Target::B27: return IDM_COL4_B27;
				case Mackie::Target::B28: return IDM_COL4_B28;
				case Mackie::Target::B29: return IDM_COL4_B29;

				case Mackie::Target::B31: return IDM_COL4_B31;
				case Mackie::Target::B32: return IDM_COL4_B32;
				case Mackie::Target::B33: return IDM_COL4_B33;
				case Mackie::Target::B34: return IDM_COL4_B34;
				case Mackie::Target::B35: return IDM_COL4_B35;
				case Mackie::Target::B36: return IDM_COL4_B36;
				case Mackie::Target::B37: return IDM_COL4_B37;
				case Mackie::Target::B38: return IDM_COL4_B38;
				case Mackie::Target::B39: return IDM_COL4_B39;

				case Mackie::Target::FUN11: return IDM_COL4_FUN11;
				case Mackie::Target::FUN12: return IDM_COL4_FUN12;
				case Mackie::Target::FUN13: return IDM_COL4_FUN13;
				case Mackie::Target::FUN14: return IDM_COL4_FUN14;
				case Mackie::Target::FUN15: return IDM_COL4_FUN15;
				case Mackie::Target::FUN16: return IDM_COL4_FUN16;
				case Mackie::Target::FUN17: return IDM_COL4_FUN17;
				case Mackie::Target::FUN18: return IDM_COL4_FUN18;

				case Mackie::Target::FUN21: return IDM_COL4_FUN21;
				case Mackie::Target::FUN22: return IDM_COL4_FUN22;
				case Mackie::Target::FUN23: return IDM_COL4_FUN23;
				case Mackie::Target::FUN24: return IDM_COL4_FUN24;
				case Mackie::Target::FUN25: return IDM_COL4_FUN25;
				case Mackie::Target::FUN26: return IDM_COL4_FUN26;
				case Mackie::Target::FUN27: return IDM_COL4_FUN27;
				case Mackie::Target::FUN28: return IDM_COL4_FUN28;

				case Mackie::Target::SYS_Rewind:	return IDM_COL4_SYS_Rewind;
				case Mackie::Target::SYS_Forward:	return IDM_COL4_SYS_Forward;
				case Mackie::Target::SYS_Stop:		return IDM_COL4_SYS_Stop;
				case Mackie::Target::SYS_Play:		return IDM_COL4_SYS_Play;
				case Mackie::Target::SYS_Record:	return IDM_COL4_SYS_Record;
				case Mackie::Target::SYS_Up:		return IDM_COL4_SYS_Up;
				case Mackie::Target::SYS_Down:		return IDM_COL4_SYS_Down;
				case Mackie::Target::SYS_Left:		return IDM_COL4_SYS_Left;
				case Mackie::Target::SYS_Right:		return IDM_COL4_SYS_Right;
				case Mackie::Target::SYS_Zoom:		return IDM_COL4_SYS_Zoom;
				case Mackie::Target::SYS_Scrub:		return IDM_COL4_SYS_Scrub;

				case Mackie::Target::MQTTKEY:		return IDM_COL4_MQTTKEY;
				case Mackie::Target::MEDIAKEY:		return IDM_COL4_MEDIAKEY;
				case Mackie::Target::VOLUMEMIX:		return IDM_COL4_VOLUMEMIX;
				default: break;
			}
			return IDM_COL4_NONE;
		}

		std::wstring_view getTarget(Common::MIDI::Mackie::Target t) {
			switch (t) {
				case Mackie::Target::MAV: return TargetMAV;
				case Mackie::Target::MAP: return TargetMAP;
				case Mackie::Target::MAM: return TargetMAM;
				case Mackie::Target::MAS: return TargetMAS;
				case Mackie::Target::JOG: return TargetJOG;

				case Mackie::Target::AV1: return TargetAV1;
				case Mackie::Target::AV2: return TargetAV2;
				case Mackie::Target::AV3: return TargetAV3;
				case Mackie::Target::AV4: return TargetAV4;
				case Mackie::Target::AV5: return TargetAV5;
				case Mackie::Target::AV6: return TargetAV6;
				case Mackie::Target::AV7: return TargetAV7;
				case Mackie::Target::AV8: return TargetAV8;
				case Mackie::Target::XV9: return TargetXV9;

				case Mackie::Target::AP1: return TargetAP1;
				case Mackie::Target::AP2: return TargetAP2;
				case Mackie::Target::AP3: return TargetAP3;
				case Mackie::Target::AP4: return TargetAP4;
				case Mackie::Target::AP5: return TargetAP5;
				case Mackie::Target::AP6: return TargetAP6;
				case Mackie::Target::AP7: return TargetAP7;
				case Mackie::Target::AP8: return TargetAP8;
				case Mackie::Target::XP9: return TargetXP9;

				case Mackie::Target::B11: return TargetB11;
				case Mackie::Target::B12: return TargetB12;
				case Mackie::Target::B13: return TargetB13;
				case Mackie::Target::B14: return TargetB14;
				case Mackie::Target::B15: return TargetB15;
				case Mackie::Target::B16: return TargetB16;
				case Mackie::Target::B17: return TargetB17;
				case Mackie::Target::B18: return TargetB18;
				case Mackie::Target::B19: return TargetB19;

				case Mackie::Target::B21: return TargetB21;
				case Mackie::Target::B22: return TargetB22;
				case Mackie::Target::B23: return TargetB23;
				case Mackie::Target::B24: return TargetB24;
				case Mackie::Target::B25: return TargetB25;
				case Mackie::Target::B26: return TargetB26;
				case Mackie::Target::B27: return TargetB27;
				case Mackie::Target::B28: return TargetB28;
				case Mackie::Target::B29: return TargetB29;

				case Mackie::Target::B31: return TargetB31;
				case Mackie::Target::B32: return TargetB32;
				case Mackie::Target::B33: return TargetB33;
				case Mackie::Target::B34: return TargetB34;
				case Mackie::Target::B35: return TargetB35;
				case Mackie::Target::B36: return TargetB36;
				case Mackie::Target::B37: return TargetB37;
				case Mackie::Target::B38: return TargetB38;
				case Mackie::Target::B39: return TargetB39;

				case Mackie::Target::FUN11: return TargetFUN11;
				case Mackie::Target::FUN12: return TargetFUN12;
				case Mackie::Target::FUN13: return TargetFUN13;
				case Mackie::Target::FUN14: return TargetFUN14;
				case Mackie::Target::FUN15: return TargetFUN15;
				case Mackie::Target::FUN16: return TargetFUN16;
				case Mackie::Target::FUN17: return TargetFUN17;
				case Mackie::Target::FUN18: return TargetFUN18;

				case Mackie::Target::FUN21: return TargetFUN21;
				case Mackie::Target::FUN22: return TargetFUN22;
				case Mackie::Target::FUN23: return TargetFUN23;
				case Mackie::Target::FUN24: return TargetFUN24;
				case Mackie::Target::FUN25: return TargetFUN25;
				case Mackie::Target::FUN26: return TargetFUN26;
				case Mackie::Target::FUN27: return TargetFUN27;
				case Mackie::Target::FUN28: return TargetFUN28;

				case Mackie::Target::SYS_Rewind: return TargetRewind;
				case Mackie::Target::SYS_Forward: return TargetForward;
				case Mackie::Target::SYS_Stop: return TargetStop;
				case Mackie::Target::SYS_Play: return TargetPlay;
				case Mackie::Target::SYS_Record: return TargetRecord;
				case Mackie::Target::SYS_Up: return TargetUp;
				case Mackie::Target::SYS_Down: return TargetDown;
				case Mackie::Target::SYS_Left: return TargetLeft;
				case Mackie::Target::SYS_Right: return TargetRight;
				case Mackie::Target::SYS_Zoom: return TargetZoom;
				case Mackie::Target::SYS_Scrub: return TargetScrub;

				case Mackie::Target::MQTTKEY: return TargetMqttKey;
				case Mackie::Target::MEDIAKEY: return TargetMediaKey;
				case Mackie::Target::VOLUMEMIX: return TargetMixer;
				default: break;
			}
			return TargetNONE;
		}
		std::wstring_view MackieHelper::GetTarget(Common::MIDI::Mackie::Target& t) {
			return getTarget(t);
		}
		std::wstring_view MackieHelper::GetTarget(uint8_t& u) {
			return getTarget(static_cast<Common::MIDI::Mackie::Target>(u));
		}
		std::wstring_view MackieHelper::GetTarget(uint32_t u) {
			return getTarget(static_cast<Common::MIDI::Mackie::Target>(u));
		}

		std::wstring_view getTranslateMMKey(Common::MIDI::Mackie::Target t) {
			switch (t) {

				case Mackie::Target::SYS_Rewind: return TargetMMKRewind;
				case Mackie::Target::SYS_Forward: return TargetMMKForward;
				case Mackie::Target::SYS_Stop: return TargetMMKStop;
				case Mackie::Target::SYS_Play: return TargetMMKPlay;
				case Mackie::Target::SYS_Record: return TargetMMKRecord;
				case Mackie::Target::SYS_Up: return TargetMMKUp;
				case Mackie::Target::SYS_Down: return TargetMMKDown;
				case Mackie::Target::SYS_Left: return TargetMMKLeft;
				case Mackie::Target::SYS_Right: return TargetMMKRight;
				case Mackie::Target::SYS_Zoom: return TargetMMKZoom;
				case Mackie::Target::SYS_Scrub: return TargetMMKScrub;

				default: break;
			}
			return TargetNONE;
		}
		std::wstring_view MackieHelper::GetTranslateMMKey(Common::MIDI::Mackie::Target& t) {
			return getTranslateMMKey(static_cast<Common::MIDI::Mackie::Target>(t));
		}
		std::wstring_view MackieHelper::GetTranslateMMKey(uint8_t& u) {
			return getTranslateMMKey(static_cast<Common::MIDI::Mackie::Target>(u));
		}
		std::wstring_view MackieHelper::GetTranslateMMKey(uint32_t u) {
			return getTranslateMMKey(static_cast<Common::MIDI::Mackie::Target>(u));
		}
	}
}
