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
			static constexpr std::wstring_view TypeClassSys = L"Class System"sv;

			static constexpr std::wstring_view TypeClassMixer = L"Class Mixer"sv;
			static constexpr std::wstring_view TypeClassMqtt = L"Class Smart Home"sv;
			static constexpr std::wstring_view TypeClassMonitor = L"Class Monitor"sv;
			static constexpr std::wstring_view TypeClassMediaKey = L"Class Media Key"sv;
			static constexpr std::wstring_view TypeClassRemote = L"Class Remote controls"sv;
			static constexpr std::wstring_view TypeClassMidi = L"Class Midi"sv;
			static constexpr std::wstring_view TypeClassLightKey = L"Class Lights"sv;
			static constexpr std::wstring_view TypeClassVmScript = L"Class VM Script"sv;
			static constexpr std::wstring_view TypeClassNone = L"Class None"sv;

			static constexpr std::wstring_view TypeTologIn = L"IO/Input"sv;
			static constexpr std::wstring_view TypeTologOut = L"IO/Output"sv;
			static constexpr std::wstring_view TypeTologSys = L"System"sv;

			static constexpr std::wstring_view TypeTologMixer = L"IO/Mixer"sv;
			static constexpr std::wstring_view TypeTologMqtt = L"IO/MQTT"sv;
			static constexpr std::wstring_view TypeTologMonitor = L"IO/Monitor"sv;
			static constexpr std::wstring_view TypeTologMediaKey = L"IO/Media Key"sv;
			static constexpr std::wstring_view TypeTologRemote = L"IO/Remote"sv;
			static constexpr std::wstring_view TypeTologMidi = L"IO/MIDI"sv;
			static constexpr std::wstring_view TypeTologVmScript = L"VM/SCRIPT"sv;
			static constexpr std::wstring_view TypeTologLights = L"LIGHTS/DMX/ARTNET"sv;
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

		static std::wstring_view getClassTypes(const PluginClassTypes t) {
			switch (t) {
				using enum PluginClassTypes;
				case ClassIn: return ClassTypeNames::TypeClassIn;
				case ClassOut: return ClassTypeNames::TypeClassOut;
				case ClassSys: return ClassTypeNames::TypeClassSys;
				case ClassMixer: return ClassTypeNames::TypeClassMixer;
				case ClassRemote: return ClassTypeNames::TypeClassRemote;
				case ClassMqttKey: return ClassTypeNames::TypeClassMqtt;
				case ClassMonitor: return ClassTypeNames::TypeClassMonitor;
				case ClassMediaKey: return ClassTypeNames::TypeClassMediaKey;
				case ClassMidi: return ClassTypeNames::TypeClassMidi;
				case ClassLightKey: return ClassTypeNames::TypeClassLightKey;
				case ClassVmScript: return ClassTypeNames::TypeClassVmScript;
				case ClassNone:
				default: return ClassTypeNames::TypeClassNone;
			}
		}
		static std::wstring_view getToLogTypes(const PluginClassTypes t) {
			switch (t) {
				using enum PluginClassTypes;
				case ClassIn: return ClassTypeNames::TypeTologIn;
				case ClassOut: return ClassTypeNames::TypeTologOut;
				case ClassSys: return ClassTypeNames::TypeTologSys;
				case ClassMixer: return ClassTypeNames::TypeTologMixer;
				case ClassMqttKey: return ClassTypeNames::TypeTologMqtt;
				case ClassMonitor: return ClassTypeNames::TypeTologMonitor;
				case ClassMediaKey: return ClassTypeNames::TypeTologMediaKey;
				case ClassRemote: return ClassTypeNames::TypeTologRemote;
				case ClassMidi: return ClassTypeNames::TypeTologMidi;
				case ClassLightKey: return ClassTypeNames::TypeTologLights;
				case ClassVmScript: return ClassTypeNames::TypeTologVmScript;
				case ClassNone:
				default: return ClassTypeNames::TypeClassNone;
			}
		}
		static log_string getCbTypes(const PluginCbType t) {

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
		std::wstring_view PluginHelper::GetClassTypes(const PluginClassTypes& t) {
			return getClassTypes(t);
		}
		std::wstring_view PluginHelper::GetTologTypes(const PluginClassTypes& t) {
			return getToLogTypes(t);
		}
		log_string PluginHelper::GetCbType(const PluginCbType& t) {
			return getCbTypes(t);
		}
	}
}
