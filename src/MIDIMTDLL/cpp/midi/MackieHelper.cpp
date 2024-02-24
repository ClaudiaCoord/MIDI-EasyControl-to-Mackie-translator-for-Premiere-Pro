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
		class NamesMackieHelper {
		public:
			static constexpr std::wstring_view TargetNONE = L"not action defined"sv;

			static constexpr std::wstring_view TargetMAV = L"audio volume (master)"sv;
			static constexpr std::wstring_view TargetMAP = L"audio balance (master)"sv;
			static constexpr std::wstring_view TargetMAM = L"audio mute (master)"sv;
			static constexpr std::wstring_view TargetMAS = L"audio solo (master)"sv;
			static constexpr std::wstring_view TargetJOG = L"JOG (video frame rewind)"sv;

			static constexpr std::wstring_view TargetAV1 = L"audio volume (channel 1)"sv;
			static constexpr std::wstring_view TargetAV2 = L"audio volume (channel 2)"sv;
			static constexpr std::wstring_view TargetAV3 = L"audio volume (channel 3)"sv;
			static constexpr std::wstring_view TargetAV4 = L"audio volume (channel 4)"sv;
			static constexpr std::wstring_view TargetAV5 = L"audio volume (channel 5)"sv;
			static constexpr std::wstring_view TargetAV6 = L"audio volume (channel 6)"sv;
			static constexpr std::wstring_view TargetAV7 = L"audio volume (channel 7)"sv;
			static constexpr std::wstring_view TargetAV8 = L"audio volume (channel 8)"sv;
			static constexpr std::wstring_view TargetXV9 = L"audio volume (channel 9)"sv;

			static constexpr std::wstring_view TargetAP1 = L"audio balance (channel 1)"sv;
			static constexpr std::wstring_view TargetAP2 = L"audio balance (channel 2)"sv;
			static constexpr std::wstring_view TargetAP3 = L"audio balance (channel 3)"sv;
			static constexpr std::wstring_view TargetAP4 = L"audio balance (channel 4)"sv;
			static constexpr std::wstring_view TargetAP5 = L"audio balance (channel 5)"sv;
			static constexpr std::wstring_view TargetAP6 = L"audio balance (channel 6)"sv;
			static constexpr std::wstring_view TargetAP7 = L"audio balance (channel 7)"sv;
			static constexpr std::wstring_view TargetAP8 = L"audio balance (channel 8)"sv;
			static constexpr std::wstring_view TargetXP9 = L"audio balance (channel 9)"sv;

			static constexpr std::wstring_view TargetB11 = L"audio solo (channel 1)"sv;
			static constexpr std::wstring_view TargetB12 = L"audio solo (channel 2)"sv;
			static constexpr std::wstring_view TargetB13 = L"audio solo (channel 3)"sv;
			static constexpr std::wstring_view TargetB14 = L"audio solo (channel 4)"sv;
			static constexpr std::wstring_view TargetB15 = L"audio solo (channel 5)"sv;
			static constexpr std::wstring_view TargetB16 = L"audio solo (channel 6)"sv;
			static constexpr std::wstring_view TargetB17 = L"audio solo (channel 7)"sv;
			static constexpr std::wstring_view TargetB18 = L"audio solo (channel 8)"sv;
			static constexpr std::wstring_view TargetB19 = L"audio solo (channel 9)"sv;

			static constexpr std::wstring_view TargetB21 = L"audio mute (channel 1)"sv;
			static constexpr std::wstring_view TargetB22 = L"audio mute (channel 2)"sv;
			static constexpr std::wstring_view TargetB23 = L"audio mute (channel 3)"sv;
			static constexpr std::wstring_view TargetB24 = L"audio mute (channel 4)"sv;
			static constexpr std::wstring_view TargetB25 = L"audio mute (channel 5)"sv;
			static constexpr std::wstring_view TargetB26 = L"audio mute (channel 6)"sv;
			static constexpr std::wstring_view TargetB27 = L"audio mute (channel 7)"sv;
			static constexpr std::wstring_view TargetB28 = L"audio mute (channel 8)"sv;
			static constexpr std::wstring_view TargetB29 = L"audio mute (channel 9)"sv;

			static constexpr std::wstring_view TargetB31 = L"audio select (channel 1)"sv;
			static constexpr std::wstring_view TargetB32 = L"audio select (channel 2)"sv;
			static constexpr std::wstring_view TargetB33 = L"audio select (channel 3)"sv;
			static constexpr std::wstring_view TargetB34 = L"audio select (channel 4)"sv;
			static constexpr std::wstring_view TargetB35 = L"audio select (channel 5)"sv;
			static constexpr std::wstring_view TargetB36 = L"audio select (channel 6)"sv;
			static constexpr std::wstring_view TargetB37 = L"audio select (channel 7)"sv;
			static constexpr std::wstring_view TargetB38 = L"audio select (channel 8)"sv;
			static constexpr std::wstring_view TargetB39 = L"audio select (channel 9)"sv;

			static constexpr std::wstring_view TargetFUN11 = L"functions F1 (standard mode)"sv;
			static constexpr std::wstring_view TargetFUN12 = L"functions F2 (standard mode)"sv;
			static constexpr std::wstring_view TargetFUN13 = L"functions F3 (standard mode)"sv;
			static constexpr std::wstring_view TargetFUN14 = L"functions F4 (standard mode)"sv;
			static constexpr std::wstring_view TargetFUN15 = L"functions F5 (standard mode)"sv;
			static constexpr std::wstring_view TargetFUN16 = L"functions F6 (standard mode)"sv;
			static constexpr std::wstring_view TargetFUN17 = L"functions F7 (standard mode)"sv;
			static constexpr std::wstring_view TargetFUN18 = L"functions F8 (standard mode)"sv;

			static constexpr std::wstring_view TargetFUN21 = L"functions F1 (held down mode)"sv;
			static constexpr std::wstring_view TargetFUN22 = L"functions F2 (held down mode)"sv;
			static constexpr std::wstring_view TargetFUN23 = L"functions F3 (held down mode)"sv;
			static constexpr std::wstring_view TargetFUN24 = L"functions F4 (held down mode)"sv;
			static constexpr std::wstring_view TargetFUN25 = L"functions F5 (held down mode)"sv;
			static constexpr std::wstring_view TargetFUN26 = L"functions F6 (held down mode)"sv;
			static constexpr std::wstring_view TargetFUN27 = L"functions F7 (held down mode)"sv;
			static constexpr std::wstring_view TargetFUN28 = L"functions F8 (held down mode)"sv;

			static constexpr std::wstring_view TargetRewind = L"Rewind (player)"sv;
			static constexpr std::wstring_view TargetForward = L"Forward (player)"sv;
			static constexpr std::wstring_view TargetStop = L"Stop (player)"sv;
			static constexpr std::wstring_view TargetPlay = L"Play (player)"sv;
			static constexpr std::wstring_view TargetRecord = L"Record (player)"sv;
			static constexpr std::wstring_view TargetUp = L"Up"sv;
			static constexpr std::wstring_view TargetDown = L"Down"sv;
			static constexpr std::wstring_view TargetLeft = L"Left"sv;
			static constexpr std::wstring_view TargetRight = L"Right"sv;
			static constexpr std::wstring_view TargetZoom = L"Zoom"sv;
			static constexpr std::wstring_view TargetScrub = L"Scrub"sv;

			static constexpr std::wstring_view TargetMMKScrub = L"LAUNCH MEDIA SELECT"sv;
			static constexpr std::wstring_view TargetMMKZoom = L"ZOOM"sv;
			static constexpr std::wstring_view TargetMMKRecord = L"PLAY"sv;
			static constexpr std::wstring_view TargetMMKRewind = L"MEDIA PREV TRACK"sv;
			static constexpr std::wstring_view TargetMMKForward = L"MEDIA NEXT TRACK"sv;
			static constexpr std::wstring_view TargetMMKStop = L"MEDIA STOP"sv;
			static constexpr std::wstring_view TargetMMKPlay = L"MEDIA PLAY/PAUSE"sv;
			static constexpr std::wstring_view TargetMMKUp = L"VOLUME UP"sv;
			static constexpr std::wstring_view TargetMMKDown = L"VOLUME DOWN"sv;
			static constexpr std::wstring_view TargetMMKLeft = L"VOLUME MUTE"sv;
			static constexpr std::wstring_view TargetMMKRight = L"PAUSE"sv;

			static constexpr std::wstring_view TargetVmScript = L"VM Script"sv;
			static constexpr std::wstring_view TargetLightKey8 = L"Lighting DMX controls (8 bits)"sv;
			static constexpr std::wstring_view TargetLightKey16 = L"Lighting DMX controls (16 bits)"sv;
			static constexpr std::wstring_view TargetMixer = L"System mixer volume/mute"sv;
			static constexpr std::wstring_view TargetMediaKey = L"System multimedia key"sv;
			static constexpr std::wstring_view TargetMqttKey = L"Smart House key (MQTT)"sv;

			static constexpr std::wstring_view TypeClickOnce = L"|Once|"sv;
			static constexpr std::wstring_view TypeClickLong = L"|Long|"sv;
			static constexpr std::wstring_view TypeClickTrigger = L"|Trigger|"sv;
			static constexpr std::wstring_view TypeClickSlider = L"ClickSlider"sv;
			static constexpr std::wstring_view TypeClickNONE = L"|none|"sv;
		};
		#pragma endregion

		std::wstring_view getClickType(const Mackie::ClickType t) {
			switch (t) {
				using enum Mackie::ClickType;
				case ClickOnce: return NamesMackieHelper::TypeClickOnce;
				case ClickLong: return NamesMackieHelper::TypeClickLong;
				case ClickTrigger: return NamesMackieHelper::TypeClickTrigger;
				case ClickSlider: return NamesMackieHelper::TypeClickSlider;
				case ClickUnknown:
				default: return NamesMackieHelper::TypeClickNONE;
			}
		}
		std::wstring_view MackieHelper::GetClickType(const Mackie::ClickType& t) {
			return getClickType(t);
		}
		std::wstring_view MackieHelper::GetClickType(const uint8_t& u) {
			return getClickType(static_cast<Mackie::ClickType>(u));
		}
		std::wstring_view MackieHelper::GetClickType(const uint32_t u) {
			return getClickType(static_cast<Mackie::ClickType>(u));
		}

		std::wstring MackieHelper::GetScriptTarget(const Common::MIDI::Mackie::Target& t) {
			switch (t) {
				using enum MIDI::Mackie::Target;
				case B11:
				case B12:
				case B13:
				case B14:
				case B15:
				case B16:
				case B17:
				case B18:
				case B19:
				case B21:
				case B22:
				case B23:
				case B24:
				case B25:
				case B26:
				case B27:
				case B28:
				case B29:
				case B31:
				case B32:
				case B33:
				case B34:
				case B35:
				case B36:
				case B37:
				case B38:
				case B39: return (log_string() << L"Script " << (static_cast<uint16_t>(t) - 22)).str();
				default: return L"-";
			}
		}
		int32_t MackieHelper::GetScriptTargetID(const Common::MIDI::Mackie::Target& t) {
			switch (t) {
				using enum MIDI::Mackie::Target;
				case B11: return IDM_COL5_SCRIPT_B11;
				case B12: return IDM_COL5_SCRIPT_B12;
				case B13: return IDM_COL5_SCRIPT_B13;
				case B14: return IDM_COL5_SCRIPT_B14;
				case B15: return IDM_COL5_SCRIPT_B15;
				case B16: return IDM_COL5_SCRIPT_B16;
				case B17: return IDM_COL5_SCRIPT_B17;
				case B18: return IDM_COL5_SCRIPT_B18;
				case B19: return IDM_COL5_SCRIPT_B19;
				case B21: return IDM_COL5_SCRIPT_B21;
				case B22: return IDM_COL5_SCRIPT_B22;
				case B23: return IDM_COL5_SCRIPT_B23;
				case B24: return IDM_COL5_SCRIPT_B24;
				case B25: return IDM_COL5_SCRIPT_B25;
				case B26: return IDM_COL5_SCRIPT_B26;
				case B27: return IDM_COL5_SCRIPT_B27;
				case B28: return IDM_COL5_SCRIPT_B28;
				case B29: return IDM_COL5_SCRIPT_B29;
				case B31: return IDM_COL5_SCRIPT_B31;
				case B32: return IDM_COL5_SCRIPT_B32;
				case B33: return IDM_COL5_SCRIPT_B33;
				case B34: return IDM_COL5_SCRIPT_B34;
				case B35: return IDM_COL5_SCRIPT_B35;
				case B36: return IDM_COL5_SCRIPT_B36;
				case B37: return IDM_COL5_SCRIPT_B37;
				case B38: return IDM_COL5_SCRIPT_B38;
				case B39: return IDM_COL5_SCRIPT_B39;
				default:  return IDM_COL5_SCRIPT_EMPTY;
			}
		}
		std::wstring MackieHelper::GetSmartHomeTarget(const Common::MIDI::Mackie::Target& t) {
			switch (t) {
				using enum MIDI::Mackie::Target;
				case B11:
				case B12:
				case B13:
				case B14:
				case B15:
				case B16:
				case B17:
				case B18:
				case B19:
				case B21:
				case B22:
				case B23:
				case B24:
				case B25:
				case B26:
				case B27:
				case B28:
				case B29:
				case B31:
				case B32:
				case B33:
				case B34:
				case B35:
				case B36:
				case B37:
				case B38:
				case B39: return (log_string() << L"on/off " << (static_cast<uint16_t>(t) - 22)).str();
				case AV1:
				case AV2:
				case AV3:
				case AV4:
				case AV5:
				case AV6:
				case AV7:
				case AV8:
				case XV9:
				case AP1:
				case AP2:
				case AP3:
				case AP4:
				case AP5:
				case AP6:
				case AP7:
				case AP8:
				case XP9: return (log_string() << L"level " << (static_cast<uint16_t>(t) - 4)).str();
				default: return L"-";
			}
		}
		int32_t MackieHelper::GetSmartHomeTargetID(const Mackie::Target& t) {
			switch (t) {
				using enum Mackie::Target;
				case AV1: return IDM_COL5_MQTT_AV1;
				case AV2: return IDM_COL5_MQTT_AV2;
				case AV3: return IDM_COL5_MQTT_AV3;
				case AV4: return IDM_COL5_MQTT_AV4;
				case AV5: return IDM_COL5_MQTT_AV5;
				case AV6: return IDM_COL5_MQTT_AV6;
				case AV7: return IDM_COL5_MQTT_AV7;
				case AV8: return IDM_COL5_MQTT_AV8;
				case XV9: return IDM_COL5_MQTT_XV9;

				case AP1: return IDM_COL5_MQTT_AP1;
				case AP2: return IDM_COL5_MQTT_AP2;
				case AP3: return IDM_COL5_MQTT_AP3;
				case AP4: return IDM_COL5_MQTT_AP4;
				case AP5: return IDM_COL5_MQTT_AP5;
				case AP6: return IDM_COL5_MQTT_AP6;
				case AP7: return IDM_COL5_MQTT_AP7;
				case AP8: return IDM_COL5_MQTT_AP8;
				case XP9: return IDM_COL5_MQTT_XP9;

				case B11: return IDM_COL5_MQTT_B11;
				case B12: return IDM_COL5_MQTT_B12;
				case B13: return IDM_COL5_MQTT_B13;
				case B14: return IDM_COL5_MQTT_B14;
				case B15: return IDM_COL5_MQTT_B15;
				case B16: return IDM_COL5_MQTT_B16;
				case B17: return IDM_COL5_MQTT_B17;
				case B18: return IDM_COL5_MQTT_B18;
				case B19: return IDM_COL5_MQTT_B19;

				case B21: return IDM_COL5_MQTT_B21;
				case B22: return IDM_COL5_MQTT_B22;
				case B23: return IDM_COL5_MQTT_B23;
				case B24: return IDM_COL5_MQTT_B24;
				case B25: return IDM_COL5_MQTT_B25;
				case B26: return IDM_COL5_MQTT_B26;
				case B27: return IDM_COL5_MQTT_B27;
				case B28: return IDM_COL5_MQTT_B28;
				case B29: return IDM_COL5_MQTT_B29;

				case B31: return IDM_COL5_MQTT_B31;
				case B32: return IDM_COL5_MQTT_B32;
				case B33: return IDM_COL5_MQTT_B33;
				case B34: return IDM_COL5_MQTT_B34;
				case B35: return IDM_COL5_MQTT_B35;
				case B36: return IDM_COL5_MQTT_B36;
				case B37: return IDM_COL5_MQTT_B37;
				case B38: return IDM_COL5_MQTT_B38;
				case B39: return IDM_COL5_MQTT_B39;
				default:  return IDM_COL5_MQTT_EMPTY;
			}
		}
		int32_t MackieHelper::GetTargetID(const Mackie::Target& t) {
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
				case VMSCRIPT:		return IDM_COL4_VMSCRIPT;
				case MQTTKEY:		return IDM_COL4_MQTTKEY;
				case MEDIAKEY:		return IDM_COL4_MEDIAKEY;
				case VOLUMEMIX:		return IDM_COL4_VOLUMEMIX;
				default: return IDM_COL4_NONE;
			}
		}

		std::wstring_view getTarget(const Mackie::Target t) {
			switch (t) {
				using enum Mackie::Target;
				case MAV: return NamesMackieHelper::TargetMAV;
				case MAP: return NamesMackieHelper::TargetMAP;
				case MAM: return NamesMackieHelper::TargetMAM;
				case MAS: return NamesMackieHelper::TargetMAS;
				case JOG: return NamesMackieHelper::TargetJOG;

				case AV1: return NamesMackieHelper::TargetAV1;
				case AV2: return NamesMackieHelper::TargetAV2;
				case AV3: return NamesMackieHelper::TargetAV3;
				case AV4: return NamesMackieHelper::TargetAV4;
				case AV5: return NamesMackieHelper::TargetAV5;
				case AV6: return NamesMackieHelper::TargetAV6;
				case AV7: return NamesMackieHelper::TargetAV7;
				case AV8: return NamesMackieHelper::TargetAV8;
				case XV9: return NamesMackieHelper::TargetXV9;

				case AP1: return NamesMackieHelper::TargetAP1;
				case AP2: return NamesMackieHelper::TargetAP2;
				case AP3: return NamesMackieHelper::TargetAP3;
				case AP4: return NamesMackieHelper::TargetAP4;
				case AP5: return NamesMackieHelper::TargetAP5;
				case AP6: return NamesMackieHelper::TargetAP6;
				case AP7: return NamesMackieHelper::TargetAP7;
				case AP8: return NamesMackieHelper::TargetAP8;
				case XP9: return NamesMackieHelper::TargetXP9;

				case B11: return NamesMackieHelper::TargetB11;
				case B12: return NamesMackieHelper::TargetB12;
				case B13: return NamesMackieHelper::TargetB13;
				case B14: return NamesMackieHelper::TargetB14;
				case B15: return NamesMackieHelper::TargetB15;
				case B16: return NamesMackieHelper::TargetB16;
				case B17: return NamesMackieHelper::TargetB17;
				case B18: return NamesMackieHelper::TargetB18;
				case B19: return NamesMackieHelper::TargetB19;

				case B21: return NamesMackieHelper::TargetB21;
				case B22: return NamesMackieHelper::TargetB22;
				case B23: return NamesMackieHelper::TargetB23;
				case B24: return NamesMackieHelper::TargetB24;
				case B25: return NamesMackieHelper::TargetB25;
				case B26: return NamesMackieHelper::TargetB26;
				case B27: return NamesMackieHelper::TargetB27;
				case B28: return NamesMackieHelper::TargetB28;
				case B29: return NamesMackieHelper::TargetB29;

				case B31: return NamesMackieHelper::TargetB31;
				case B32: return NamesMackieHelper::TargetB32;
				case B33: return NamesMackieHelper::TargetB33;
				case B34: return NamesMackieHelper::TargetB34;
				case B35: return NamesMackieHelper::TargetB35;
				case B36: return NamesMackieHelper::TargetB36;
				case B37: return NamesMackieHelper::TargetB37;
				case B38: return NamesMackieHelper::TargetB38;
				case B39: return NamesMackieHelper::TargetB39;

				case FUN11: return NamesMackieHelper::TargetFUN11;
				case FUN12: return NamesMackieHelper::TargetFUN12;
				case FUN13: return NamesMackieHelper::TargetFUN13;
				case FUN14: return NamesMackieHelper::TargetFUN14;
				case FUN15: return NamesMackieHelper::TargetFUN15;
				case FUN16: return NamesMackieHelper::TargetFUN16;
				case FUN17: return NamesMackieHelper::TargetFUN17;
				case FUN18: return NamesMackieHelper::TargetFUN18;

				case FUN21: return NamesMackieHelper::TargetFUN21;
				case FUN22: return NamesMackieHelper::TargetFUN22;
				case FUN23: return NamesMackieHelper::TargetFUN23;
				case FUN24: return NamesMackieHelper::TargetFUN24;
				case FUN25: return NamesMackieHelper::TargetFUN25;
				case FUN26: return NamesMackieHelper::TargetFUN26;
				case FUN27: return NamesMackieHelper::TargetFUN27;
				case FUN28: return NamesMackieHelper::TargetFUN28;

				case SYS_Rewind: return NamesMackieHelper::TargetRewind;
				case SYS_Forward: return NamesMackieHelper::TargetForward;
				case SYS_Stop: return NamesMackieHelper::TargetStop;
				case SYS_Play: return NamesMackieHelper::TargetPlay;
				case SYS_Record: return NamesMackieHelper::TargetRecord;
				case SYS_Up: return NamesMackieHelper::TargetUp;
				case SYS_Down: return NamesMackieHelper::TargetDown;
				case SYS_Left: return NamesMackieHelper::TargetLeft;
				case SYS_Right: return NamesMackieHelper::TargetRight;
				case SYS_Zoom: return NamesMackieHelper::TargetZoom;
				case SYS_Scrub: return NamesMackieHelper::TargetScrub;
				
				case VMSCRIPT:	return NamesMackieHelper::TargetVmScript;
				case LIGHTKEY16B: return NamesMackieHelper::TargetLightKey16;
				case LIGHTKEY8B: return NamesMackieHelper::TargetLightKey8;
				case MQTTKEY: return NamesMackieHelper::TargetMqttKey;
				case MEDIAKEY: return NamesMackieHelper::TargetMediaKey;
				case VOLUMEMIX: return NamesMackieHelper::TargetMixer;
				default: return NamesMackieHelper::TargetNONE;
			}
		}
		std::wstring_view MackieHelper::GetTarget(const Mackie::Target& t) {
			return getTarget(t);
		}
		std::wstring_view MackieHelper::GetTarget(const uint8_t& u) {
			return getTarget(static_cast<Mackie::Target>(u));
		}
		std::wstring_view MackieHelper::GetTarget(const uint32_t u) {
			return getTarget(static_cast<Mackie::Target>(u));
		}

		std::wstring_view getTranslateMMKey(const Mackie::Target t) {
			switch (t) {
				using enum Mackie::Target;
				case SYS_Rewind: return NamesMackieHelper::TargetMMKRewind;
				case SYS_Forward: return NamesMackieHelper::TargetMMKForward;
				case SYS_Stop: return NamesMackieHelper::TargetMMKStop;
				case SYS_Play: return NamesMackieHelper::TargetMMKPlay;
				case SYS_Record: return NamesMackieHelper::TargetMMKRecord;
				case SYS_Up: return NamesMackieHelper::TargetMMKUp;
				case SYS_Down: return NamesMackieHelper::TargetMMKDown;
				case SYS_Left: return NamesMackieHelper::TargetMMKLeft;
				case SYS_Right: return NamesMackieHelper::TargetMMKRight;
				case SYS_Zoom: return NamesMackieHelper::TargetMMKZoom;
				case SYS_Scrub: return NamesMackieHelper::TargetMMKScrub;
				default: return NamesMackieHelper::TargetNONE;
			}
		}
		std::wstring_view MackieHelper::GetTranslateMMKey(const Mackie::Target& t) {
			return getTranslateMMKey(t);
		}
		std::wstring_view MackieHelper::GetTranslateMMKey(const uint8_t& u) {
			return getTranslateMMKey(static_cast<Mackie::Target>(u));
		}
		std::wstring_view MackieHelper::GetTranslateMMKey(const uint32_t u) {
			return getTranslateMMKey(static_cast<Mackie::Target>(u));
		}
	}
}
