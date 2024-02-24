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
		class NamesMidiHelper {
		public:
			#pragma region strings view
			static constexpr std::wstring_view MackieOut = L"-Mackie-Out"sv;
			static constexpr std::wstring_view ProxyOut = L"-Proxy-Out-"sv;

			static constexpr std::wstring_view TypeKNOB = L"KNOB"sv;
			static constexpr std::wstring_view TypeFADER = L"FADER"sv;
			static constexpr std::wstring_view TypeSLIDER = L"SLIDER"sv;
			static constexpr std::wstring_view TypeBUTTON = L"BUTTON"sv;
			static constexpr std::wstring_view TypeBUTTONT = L"BUTTON TOGGLE"sv;
			static constexpr std::wstring_view TypeKNOBI = L"KNOB INVERSE"sv;
			static constexpr std::wstring_view TypeFADERI = L"FADER INVERSE"sv;
			static constexpr std::wstring_view TypeSLIDERI = L"SLIDER INVERSE"sv;
			static constexpr std::wstring_view TypeNONE = L"control not defined"sv;

			static constexpr std::wstring_view SceneNONE = L"scene not in list"sv;

			static constexpr std::wstring_view SceneSC1 = L"1"sv;
			static constexpr std::wstring_view SceneSC2 = L"2"sv;
			static constexpr std::wstring_view SceneSC3 = L"3"sv;
			static constexpr std::wstring_view SceneSC4 = L"4"sv;
			static constexpr std::wstring_view SceneSC101 = L"101"sv;
			static constexpr std::wstring_view SceneSC1KNOB = L"1"sv;
			static constexpr std::wstring_view SceneSC2KNOB = L"2"sv;
			static constexpr std::wstring_view SceneSC3KNOB = L"3"sv;
			static constexpr std::wstring_view SceneSC4KNOB = L"4"sv;
			static constexpr std::wstring_view SceneSC4BTN = L"4"sv;

			#pragma endregion
		};

		const std::wstring MidiHelper::GetSuffixMackieOut() {
			return NamesMidiHelper::MackieOut.data();
		}
		const std::wstring MidiHelper::GetSuffixProxyOut() {
			return NamesMidiHelper::ProxyOut.data();
		}

		std::wstring_view getType(MidiUnitType t) {
			switch (t) {
				using enum MidiUnitType;
				case FADER: return NamesMidiHelper::TypeFADER;
				case SLIDER: return NamesMidiHelper::TypeSLIDER;
				case KNOB: return NamesMidiHelper::TypeKNOB;
				case BTN: return NamesMidiHelper::TypeBUTTON;
				case BTNTOGGLE: return NamesMidiHelper::TypeBUTTONT;
				case KNOBINVERT: return NamesMidiHelper::TypeKNOBI;
				case SLIDERINVERT: return NamesMidiHelper::TypeSLIDERI;
				case FADERINVERT: return NamesMidiHelper::TypeFADERI;
				default: break;
			}
			return NamesMidiHelper::TypeNONE;
		}
		std::wstring_view MidiHelper::GetType(MidiUnitType& t) {
			return getType(t);
		}
		std::wstring_view MidiHelper::GetType(uint8_t& i) {
			return getType(static_cast<MidiUnitType>(i));
		}
		std::wstring_view MidiHelper::GetType(uint32_t i) {
			return getType(static_cast<MidiUnitType>(i));
		}

		std::wstring_view getScene(MidiUnitScene t) {
			switch (t) {
				using enum MidiUnitScene;
				case SC1: return NamesMidiHelper::SceneSC1;
				case SC2: return NamesMidiHelper::SceneSC2;
				case SC3: return NamesMidiHelper::SceneSC3;
				case SC4: return NamesMidiHelper::SceneSC4;
				case SC101: return NamesMidiHelper::SceneSC101;
				case SC1KNOB: return NamesMidiHelper::SceneSC1KNOB;
				case SC2KNOB: return NamesMidiHelper::SceneSC2KNOB;
				case SC3KNOB: return NamesMidiHelper::SceneSC3KNOB;
				case SC4KNOB: return NamesMidiHelper::SceneSC4KNOB;
				case SC4BTN: return NamesMidiHelper::SceneSC4BTN;
				default: break;
			}
			return NamesMidiHelper::SceneNONE;
		}
		std::wstring_view MidiHelper::GetScene(MidiUnitScene& t) {
			return getScene(t);
		}
		std::wstring_view MidiHelper::GetScene(uint8_t& u) {
			return getScene(static_cast<MidiUnitScene>(u));
		}
		std::wstring_view MidiHelper::GetScene(uint32_t u) {
			return getScene(static_cast<MidiUnitScene>(u));
		}
	}
}
