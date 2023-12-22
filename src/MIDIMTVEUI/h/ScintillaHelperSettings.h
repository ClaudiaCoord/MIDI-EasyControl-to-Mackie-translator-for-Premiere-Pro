/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Viewer/Editor interface.
	(c) CC 2023, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include "ILexer.h"
#include "Lexilla.h"
#include "SciLexer.h"

#define RGB_(R,G,B) (R | (G << 8) | (B << 16))
#define RGBW_(R,G,B,A) (R | (G << 8) | (B << 16) | (A << 24))
#define COLORREF_(R,G,B) RGB_(::strtoul(R, nullptr, 16), ::strtoul(G, nullptr, 16), ::strtoul(B, nullptr, 16))

#if defined(_DEBUG_EVENT)
#include "ScintillaHelper.h"
#endif

namespace Common {
	namespace UI {

		#include "../rc/icons/BlackOut.xpm"
		#include "../rc/icons/FadeIn.xpm"
		#include "../rc/icons/FadeOut.xpm"
		#include "../rc/icons/Off.xpm"
		#include "../rc/icons/On.xpm"
		#include "../rc/icons/Pulse.xpm"
		#include "../rc/icons/PulseOff.xpm"
		#include "../rc/icons/Value.xpm"
		#include "../rc/icons/Red.xpm"
		#include "../rc/icons/Green.xpm"
		#include "../rc/icons/Blue.xpm"
		#include "../rc/icons/White.xpm"

		using namespace std::string_view_literals;

		extern "C" bool __stdcall Scintilla_Register(HINSTANCE);
		extern "C" void __stdcall Scintilla_UnRegister();

		enum class lexer_type : int {
			TYPE_NONE = 0,
			TYPE_FORE,
			TYPE_BACK,
			TYPE_SET
		};

		struct lexer_color {
			lexer_type type{ lexer_type::TYPE_NONE };
			uint32_t group{ 0 };
			uptr_t id{ 0 };
			sptr_t color{ 0 };
		};

		constexpr lexer_color lexer_cpp_color[]{
			{ lexer_type::TYPE_FORE, 0, SCE_C_DEFAULT, RGB_(88, 110, 117) },
			{ lexer_type::TYPE_FORE, 0, SCE_C_COMMENT, RGB_(147, 161, 161) },
			{ lexer_type::TYPE_FORE, 0, SCE_C_COMMENTLINE, RGB_(88, 133, 133) },
			{ lexer_type::TYPE_FORE, 0, SCE_C_COMMENTDOC, RGB_(67, 101, 101) },
			{ lexer_type::TYPE_FORE, 0, SCE_C_COMMENTLINEDOC, RGB_(58, 96, 96) },
			{ lexer_type::TYPE_FORE, 0, SCE_C_COMMENTDOCKEYWORD, RGB_(8, 102, 102) },
			{ lexer_type::TYPE_FORE, 0, SCE_C_COMMENTDOCKEYWORDERROR, RGB_(4, 140, 140) },
			{ lexer_type::TYPE_FORE, 0, SCE_C_NUMBER, RGB_(211, 54, 130) },
			{ lexer_type::TYPE_FORE, 0, SCE_C_WORD, RGB_(205, 88, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_C_WORD2, RGB_(205, 34, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_C_STRING, RGB_(38, 145, 134) },
			{ lexer_type::TYPE_FORE, 0, SCE_C_IDENTIFIER, RGB_(60, 50, 5) },
			{ lexer_type::TYPE_FORE, 0, SCE_C_PREPROCESSOR, RGB_(11, 88, 144) },
			{ lexer_type::TYPE_FORE, 0, SCE_C_OPERATOR, RGB_(133, 153, 0) },
			{ lexer_type::TYPE_SET,  SCI_SETSELFORE,     1, RGB_(82,  33, 0) },
			{ lexer_type::TYPE_SET,  SCI_SETSELBACK,     1, RGB_(255, 227, 193) },
			{ lexer_type::TYPE_SET,  SCI_STYLESETFORE,   0, RGB_(180, 180, 180) },
			{ lexer_type::TYPE_SET,  SCI_STYLESETBACK,   0, RGB_(255, 255, 255) },
			{ lexer_type::TYPE_SET,  SCI_SETMARGINBACKN, 0, RGB_(207, 204, 201) },
			{ lexer_type::TYPE_SET,  SCI_SETMARGINBACKN, 2, RGB_(200, 200, 200) },
			{ lexer_type::TYPE_SET,  SCI_MARKERSETFORE,  2, RGB_(0, 192, 0) },
			{ lexer_type::TYPE_SET,  SCI_MARKERSETFORE,  6, RGB_(228, 94, 94) },
			{ lexer_type::TYPE_SET,  SCI_MARKERSETFORE, 24, RGB_(25, 25, 25) },
			{ lexer_type::TYPE_SET,  SCI_SETCARETFORE, RGB_(115, 115, 115), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_CARET, RGBW_(192, 192, 192, 255), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_CARET_LINE_BACK, RGBW_(238, 232, 213, 200), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_SELECTION_TEXT, RGBW_(0, 0, 0, 255), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_SELECTION_BACK, RGBW_(0, 0, 0, 255), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_LIST, RGBW_(190, 190, 190, 255), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_LIST_BACK, RGBW_(247, 247, 219, 255), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_LIST_SELECTED, RGBW_(0, 255, 255, 255), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_LIST_SELECTED_BACK, RGBW_(126, 121, 121, 255), 0 },
		};
		constexpr lexer_color lexer_json_color[]{
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_DEFAULT, RGB_(0, 0, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_URI, RGB_(0, 0x80, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_COMPACTIRI, RGB_(0, 0x80, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_LINECOMMENT, RGB_(0, 0x80, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_BLOCKCOMMENT, RGB_(0, 0x80, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_OPERATOR, RGB_(0, 0x80, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_KEYWORD, RGB_(0x80, 0x80, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_LDKEYWORD, RGB_(0, 0x80, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_NUMBER, RGB_(0, 0x80, 0x80) },
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_STRING, RGB_(0x80, 0, 0x80) },
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_STRINGEOL, RGB_(0, 0, 0x80) },
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_PROPERTYNAME, RGB_(0x40, 0, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_ESCAPESEQUENCE, RGB_(0x80, 0, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_JSON_ERROR, RGB_(0x80, 0, 0) },
		};
		constexpr lexer_color lexer_log_color[]{
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_DEFAULT, RGB_(0, 0, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_NUMBER, RGB_(108, 108, 108) },
			{ lexer_type::TYPE_BACK, 0, SCE_LOG_NUMBER, RGB_(245, 245, 245) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_STRING, RGB_(128, 0, 128) },
			{ lexer_type::TYPE_BACK, 0, SCE_LOG_STRING, RGB_(245, 245, 245) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_COMMENT, RGB_(86, 16, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_SYMBOL, RGB_(86, 16, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_BINARY, RGB_(86, 16, 0) },
			{ lexer_type::TYPE_BACK, 0, SCE_LOG_BINARY, RGB_(245, 245, 245) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_BOOL, RGB_(0, 128, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_SELF, RGB_(0, 128, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_SUPER, RGB_(128, 128, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_NIL, RGB_(0, 128, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_GLOBAL, RGB_(0, 0, 128) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_RETURN, RGB_(64, 0, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_SPECIAL, RGB_(128, 0, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_KWSEND, RGB_(128, 0, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_ASSIGN, RGB_(128, 0, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_CHARACTER, RGB_(128, 0, 0) },
			{ lexer_type::TYPE_FORE, 0, SCE_LOG_SPEC_SEL, RGB_(128, 0, 0) },
			{ lexer_type::TYPE_SET,  SCI_SETSELFORE,     1, RGB_(82,  33, 0) },
			{ lexer_type::TYPE_SET,  SCI_SETSELBACK,     1, RGB_(255, 227, 193) },
			{ lexer_type::TYPE_SET,  SCI_STYLESETFORE,   0, RGB_(45, 0, 0) },
			{ lexer_type::TYPE_SET,  SCI_STYLESETBACK,   0, RGB_(245, 245, 245) },
			{ lexer_type::TYPE_SET,  SCI_SETMARGINBACKN, 0, RGB_(207, 204, 201) },
			{ lexer_type::TYPE_SET,  SCI_SETMARGINBACKN, 2, RGB_(200, 200, 200) },
			{ lexer_type::TYPE_SET,  SCI_MARKERSETFORE,  2, RGB_(0, 192, 0) },
			{ lexer_type::TYPE_SET,  SCI_MARKERSETFORE,  6, RGB_(228, 94, 94) },
			{ lexer_type::TYPE_SET,  SCI_MARKERSETFORE, 24, RGB_(25, 25, 25) },
			{ lexer_type::TYPE_SET,  SCI_SETCARETFORE, RGB_(115, 115, 115), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_CARET, RGBW_(192, 192, 192, 255), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_CARET_LINE_BACK, RGBW_(238, 232, 213, 200), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_SELECTION_TEXT, RGBW_(0, 0, 0, 255), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_SELECTION_BACK, RGBW_(245, 245, 245, 255), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_LIST, RGBW_(190, 190, 190, 255), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_LIST_BACK, RGBW_(247, 247, 219, 255), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_LIST_SELECTED, RGBW_(0, 255, 255, 255), 0 },
			{ lexer_type::TYPE_SET,  SC_ELEMENT_LIST_SELECTED_BACK, RGBW_(245, 245, 245, 255), 0 }
		};
		constexpr std::string_view scint_cpp_list_keywords = "BlackOut()?0;FadeIn(dmx_channel, milliseconds)?1;FadeOut(dmx_channel, milliseconds)?2;Pulse(dmx_channel, min_value, max_value, milliseconds)?3;PulseOff(dmx_channel)?4;Value(dmx_channel, 0-255)?5;Off(dmx_channel)?7;SetRed(dmx_channel)?8;SetGreen(dmx_channel)?9;SetBlue(dmx_channel)?10;SetWhite(dmx_channel)?11;Red()?8;Green()?9;Blue()?10;White()?11;ChannelsAvailable()?11;ChannelsBindings()?11;"sv;
		constexpr std::string_view scint_cpp_ident_keywords = "namespace ;int ;var ;def ;string ;if (bool);else if (bool);else;break;while(bool);eval(string);for();Object();Container();Map();Map_Pair();Range();Const_Range();Vector();Type_Info();Function();true;false;to_string(Object);puts(Object);print(Object);max(Object, Object);min(Object, Object);even(Object);for_each(Range, Function);map(Range, Function);foldl(Range, Function, Object);sum(Range);product(Range);take(Range, int);take_while(Range, Function);drop(Range, int);drop_while(Range, Function);reduce(Range, Function);filter(Container, Function);join(Range, string);reverse(Container);generate_range(Object, Object);concat(Range, Range);collate(Object, Object);zip_with(Function, Range, Range);zip(Range, Range);call_exists(Function, ...);retro(Range);retro(Const_Range);throw(Object);"sv;

		constexpr std::string_view scint_json_list_keywords = "units;lights;artnet;dmx;mqtt;true;false;null;"sv;
		constexpr std::string_view scint_json_ident_keywords = "scene;id;type;target;longtarget;onoff;value;apps;enable;port;universe;ip;mask;broadcast;port;baudrate;stopbits;timeout;name;config;autostart;manualport;jogscenefilter;proxy;btninterval;btnlonginterval;host;login;pass;prefix;sslpsk;certcapath;port;loglevel;isssl;selfsigned;"sv;

		constexpr std::string_view scint_log_list_keywords = "MIDI-MT;Copyright;MIDI;MIXER;MMKey;MQTT;LIGHT;PUBLISH;CONNECT;DISCONNECT;"sv;
		constexpr std::string_view scint_log_ident_keywords = "ArtNet;DMX;DMX512;Broker;SmartHome;LightBridge;MMKBridge;AudioSessionMixer;MidiControllerVirtual;MidiControllerIn;MidiControllerOut;MidiControllerBase;init;on_log;"sv;

		const char** scint_icons[] = { blackout_xpm, fadein_xpm, fadeout_xpm, pulse_xpm, pulseoff_xpm, value_xpm, on_xpm, off_xpm, red_xpm, green_xpm, blue_xpm, white_xpm };

		template<typename T>
		void send_lexer_settings_(T* clz, lexer_color& lc) {
			if (lc.type == lexer_type::TYPE_FORE)      (void) clz->send_fore_color(lc.id, lc.color);
			else if (lc.type == lexer_type::TYPE_BACK) (void) clz->send_bg_color(lc.id, lc.color);
			else if (lc.type == lexer_type::TYPE_SET)  (void) clz->send(lc.group, lc.id, lc.color);
		}

		template<typename T>
		void send_lexer_system_settings_(T* clz, lexer_color& lc) {
			if (lc.type == lexer_type::TYPE_FORE)      (void) clz->send_fore_color(lc.id, lc.color);
			else if (lc.type == lexer_type::TYPE_BACK) (void) clz->send_bg_color(lc.id, lc.color);
			else if (lc.type == lexer_type::TYPE_SET) {
				switch (lc.group) {
					case SCI_SETSELFORE:			clz->send(lc.group, lc.id, ::GetSysColor(COLOR_HIGHLIGHTTEXT)); break;
					case SCI_SETSELBACK:			clz->send(lc.group, lc.id, ::GetSysColor(COLOR_HIGHLIGHT)); break;
					case SCI_STYLESETFORE:			clz->send(lc.group, lc.id, ::GetSysColor(COLOR_WINDOWTEXT)); break;
					case SCI_STYLESETBACK:			clz->send(lc.group, lc.id, ::GetSysColor(COLOR_WINDOW)); break;
					case SCI_MARKERSETFORE:			clz->send(lc.group, lc.id, ::GetSysColor(COLOR_BTNHIGHLIGHT)); break;
					case SCI_SETMARGINBACKN:		clz->send(lc.group, lc.id, ::GetSysColor(COLOR_BTNFACE)); break;
					case SC_ELEMENT_LIST:
					case SCI_SETCARETFORE:
					case SC_ELEMENT_CARET:			clz->send(lc.group, ::GetSysColor(COLOR_WINDOWTEXT)); break;
					case SC_ELEMENT_LIST_BACK:		clz->send(lc.group, ::GetSysColor(COLOR_WINDOW)); break;
					case SC_ELEMENT_SELECTION_TEXT:
					case SC_ELEMENT_LIST_SELECTED:	clz->send(lc.group, ::GetSysColor(COLOR_HIGHLIGHTTEXT)); break;
					case SC_ELEMENT_LIST_SELECTED_BACK:
					case SC_ELEMENT_CARET_LINE_BACK:
					case SC_ELEMENT_SELECTION_BACK:	clz->send(lc.group, ::GetSysColor(COLOR_HIGHLIGHT)); break;
					default:						clz->send(lc.group, lc.id, lc.color); break;
				}

			}
		}
	}
}
