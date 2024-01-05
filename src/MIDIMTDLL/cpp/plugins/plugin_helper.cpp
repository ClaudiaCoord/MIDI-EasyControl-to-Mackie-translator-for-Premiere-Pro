/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL + MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace IO {

		using namespace std::string_view_literals;

		/* PluginHelper */

		class ClassTypeNames {
		public:
			#pragma region Class Type Names
			static constexpr std::wstring_view TypeClassIn = L"Class Input"sv;
			static constexpr std::wstring_view TypeClassOut = L"Class Output (auto)"sv;
			static constexpr std::wstring_view TypeClassOut1 = L"Class Output 1"sv;
			static constexpr std::wstring_view TypeClassOut2 = L"Class Output 2"sv;
			static constexpr std::wstring_view TypeClassSys = L"Class System"sv;

			static constexpr std::wstring_view TypeClassProxy = L"Class Proxy"sv;
			static constexpr std::wstring_view TypeClassMixer = L"Class Mixer"sv;
			static constexpr std::wstring_view TypeClassMqtt = L"Class Smart Home"sv;
			static constexpr std::wstring_view TypeClassMonitor = L"Class Monitor"sv;
			static constexpr std::wstring_view TypeClassMediaKey = L"Class Media Key"sv;
			static constexpr std::wstring_view TypeClassRemote = L"Class Remote controls"sv;
			static constexpr std::wstring_view TypeClassInMidi = L"Class In Midi"sv;
			static constexpr std::wstring_view TypeClassOutMidi = L"Class Out Midi"sv;
			static constexpr std::wstring_view TypeClassOutMidiMackie = L"Class Out Midi Mackie"sv;
			static constexpr std::wstring_view TypeClassVirtualMidi = L"Class Virtual Midi"sv;
			static constexpr std::wstring_view TypeClassNone = L"Class None"sv;

			static constexpr std::wstring_view TypeTologIn = L"IO/Input"sv;
			static constexpr std::wstring_view TypeTologOut = L"IO/Output"sv;
			static constexpr std::wstring_view TypeTologSys = L"System"sv;

			static constexpr std::wstring_view TypeTologProxy = L"IO/Proxy"sv;
			static constexpr std::wstring_view TypeTologMixer = L"IO/Mixer"sv;
			static constexpr std::wstring_view TypeTologMqtt = L"IO/MQTT"sv;
			static constexpr std::wstring_view TypeTologMonitor = L"IO/Monitor"sv;
			static constexpr std::wstring_view TypeTologMediaKey = L"IO/Media Key"sv;
			static constexpr std::wstring_view TypeTologRemote = L"IO/Remote"sv;
			static constexpr std::wstring_view TypeTologMidi = L"IO/MIDI"sv;
			#pragma endregion
		};

		class CbTypeNames {
		public:
			#pragma region CallBack Type Names
			static constexpr std::wstring_view TypeOut1Cb = L"Out(1)"sv;
			static constexpr std::wstring_view TypeOut2Cb = L"Out(2)"sv;
			static constexpr std::wstring_view TypeIn1Cb = L"In(1)"sv;
			static constexpr std::wstring_view TypeIn2Cb = L"In(2)"sv;
			static constexpr std::wstring_view TypeLogCb = L"Log"sv;
			static constexpr std::wstring_view TypeLogsCb = L"Log(s)"sv;
			static constexpr std::wstring_view TypeLogoCb = L"Log(o)"sv;
			static constexpr std::wstring_view TypePidCb = L"Pid"sv;
			static constexpr std::wstring_view TypeConfCb = L"Conf"sv;
			static constexpr std::wstring_view TypeIdCb = L"Id"sv;
			static constexpr std::wstring_view TypeNoneCb = L"None"sv;
			#pragma endregion
		};

		#define IS_CBVALUE_(A,B) uint16_t x = static_cast<uint16_t>(B); ((A & x) == x)

		static std::wstring_view getClassTypes(PluginClassTypes t) {
			switch (t) {
				using enum PluginClassTypes;
				case ClassIn: return ClassTypeNames::TypeClassIn;
				case ClassOut: return ClassTypeNames::TypeClassOut;
				case ClassOut1: return ClassTypeNames::TypeClassOut1;
				case ClassOut2: return ClassTypeNames::TypeClassOut2;
				case ClassSys: return ClassTypeNames::TypeClassSys;
				case ClassProxy: return ClassTypeNames::TypeClassProxy;
				case ClassMixer: return ClassTypeNames::TypeClassMixer;
				case ClassRemote: return ClassTypeNames::TypeClassRemote;
				case ClassMqttKey: return ClassTypeNames::TypeClassMqtt;
				case ClassMonitor: return ClassTypeNames::TypeClassMonitor;
				case ClassMediaKey: return ClassTypeNames::TypeClassMediaKey;
				case ClassInMidi: return ClassTypeNames::TypeClassInMidi;
				case ClassOutMidi: return ClassTypeNames::TypeClassOutMidi;
				case ClassOutMidiMackie: return ClassTypeNames::TypeClassOutMidiMackie;
				case ClassVirtualMidi: return ClassTypeNames::TypeClassVirtualMidi;
				case ClassNone:
				default: return ClassTypeNames::TypeClassNone;
			}
		}
		static std::wstring_view getToLogTypes(PluginClassTypes t) {
			switch (t) {
				using enum PluginClassTypes;
				case ClassIn: return ClassTypeNames::TypeTologIn;
				case ClassOut:
				case ClassOut1:
				case ClassOut2: return ClassTypeNames::TypeTologOut;
				case ClassSys: return ClassTypeNames::TypeTologSys;
				case ClassProxy: return ClassTypeNames::TypeTologProxy;
				case ClassMixer: return ClassTypeNames::TypeTologMixer;
				case ClassMqttKey: return ClassTypeNames::TypeTologMqtt;
				case ClassMonitor: return ClassTypeNames::TypeTologMonitor;
				case ClassMediaKey: return ClassTypeNames::TypeTologMediaKey;
				case ClassRemote: return ClassTypeNames::TypeTologRemote;
				case ClassInMidi:
				case ClassOutMidi:
				case ClassOutMidiMackie:
				case ClassVirtualMidi: return ClassTypeNames::TypeTologMidi;
				case ClassNone:
				default: return ClassTypeNames::TypeClassNone;
			}
		}
		static log_string getCbTypes(PluginCbType t) {

			uint32_t k = static_cast<uint32_t>(t);
			log_string ls{};

			if (t == PluginCbType::None)
				ls << CbTypeNames::TypeNoneCb << L" | ";
			if (IS_CBVALUE_(k, PluginCbType::Out1Cb))
				ls << CbTypeNames::TypeOut1Cb << L" | ";
			if (IS_CBVALUE_(k, PluginCbType::Out2Cb))
				ls << CbTypeNames::TypeOut2Cb << L" | ";
			if (IS_CBVALUE_(k, PluginCbType::In1Cb))
				ls << CbTypeNames::TypeIn1Cb << L" | ";
			if (IS_CBVALUE_(k, PluginCbType::In2Cb))
				ls << CbTypeNames::TypeIn2Cb << L" | ";
			if (IS_CBVALUE_(k, PluginCbType::LogCb))
				ls << CbTypeNames::TypeLogCb << L" | ";
			if (IS_CBVALUE_(k, PluginCbType::LogsCb))
				ls << CbTypeNames::TypeLogsCb << L" | ";
			if (IS_CBVALUE_(k, PluginCbType::LogoCb))
				ls << CbTypeNames::TypeLogoCb << L" | ";
			if (IS_CBVALUE_(k, PluginCbType::PidCb))
				ls << CbTypeNames::TypePidCb << L" | ";
			if (IS_CBVALUE_(k, PluginCbType::ConfCb))
				ls << CbTypeNames::TypeConfCb << L" | ";
			if (IS_CBVALUE_(k, PluginCbType::IdCb))
				ls << CbTypeNames::TypeIdCb << L" | ";

			return ls;
		}
		std::wstring_view PluginHelper::GetClassTypes(PluginClassTypes& t) {
			return getClassTypes(t);
		}
		std::wstring_view PluginHelper::GetTologTypes(PluginClassTypes& t) {
			return getToLogTypes(t);
		}
		log_string PluginHelper::GetCbType(PluginCbType& t) {
			return getCbTypes(t);
		}
	}
}
