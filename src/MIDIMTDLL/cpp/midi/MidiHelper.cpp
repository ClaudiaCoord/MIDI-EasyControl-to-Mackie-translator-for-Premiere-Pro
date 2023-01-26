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

		constexpr std::wstring_view strMackieOut = L"-Mackie-Out"sv;
		constexpr std::wstring_view strProxyOut = L"-Proxy-Out-"sv;

		constexpr std::wstring_view TypeKNOB = L"KNOB"sv;
		constexpr std::wstring_view TypeFADER = L"FADER"sv;
		constexpr std::wstring_view TypeSLIDER = L"SLIDER"sv;
		constexpr std::wstring_view TypeBUTTON = L"BUTTON"sv;
		constexpr std::wstring_view TypeBUTTONT = L"BUTTON TOGGLE"sv;
		constexpr std::wstring_view TypeKNOBI = L"KNOB INVERSE"sv;
		constexpr std::wstring_view TypeFADERI = L"FADER INVERSE"sv;
		constexpr std::wstring_view TypeSLIDERI = L"SLIDER INVERSE"sv;
		constexpr std::wstring_view TypeNONE = L"control not defined"sv;

		constexpr std::wstring_view TypeClassProxy = L"Class Proxy"sv;
		constexpr std::wstring_view TypeClassMixer = L"Class Mixer"sv;
		constexpr std::wstring_view TypeClassMonitor = L"Class Monitor"sv;
		constexpr std::wstring_view TypeClassMediaKey = L"Class Media Key"sv;
		constexpr std::wstring_view TypeClassInMidi = L"Class In Midi"sv;
		constexpr std::wstring_view TypeClassOutMidi = L"Class Out Midi"sv;
		constexpr std::wstring_view TypeClassOutMidiMackie = L"Class Out Midi Mackie"sv;
		constexpr std::wstring_view TypeClassVirtualMidi = L"Class Virtual Midi"sv;
		constexpr std::wstring_view TypeClassNone = L"Class None"sv;

		constexpr std::wstring_view SceneNONE = L"scene not in list"sv;

		const std::wstring MidiHelper::GetSuffixMackieOut() {
			return std::wstring(strMackieOut);
		}
		const std::wstring MidiHelper::GetSuffixProxyOut() {
			return std::wstring(strProxyOut);
		}


		std::wstring_view getClassTypes(ClassTypes t) {
			switch (t) {
				case Common::MIDI::ClassTypes::ClassProxy: return TypeClassProxy;
				case Common::MIDI::ClassTypes::ClassMixer: return TypeClassMixer;
				case Common::MIDI::ClassTypes::ClassMonitor: return TypeClassMonitor;
				case Common::MIDI::ClassTypes::ClassMediaKey: return TypeClassMediaKey;
				case Common::MIDI::ClassTypes::ClassInMidi: return TypeClassInMidi;
				case Common::MIDI::ClassTypes::ClassOutMidi: return TypeClassOutMidi;
				case Common::MIDI::ClassTypes::ClassOutMidiMackie: return TypeClassOutMidiMackie;
				case Common::MIDI::ClassTypes::ClassVirtualMidi: return TypeClassVirtualMidi;
				case Common::MIDI::ClassTypes::ClassNone:
				default: return TypeClassNone;
			}
		}
		std::wstring_view MidiHelper::GetClassTypes(ClassTypes& t) {
			return getClassTypes(t);
		}

		std::wstring_view getType(Common::MIDI::MidiUnitType t) {
			switch (t) {
				case 0: return TypeFADER;
				case 1: return TypeSLIDER;
				case 2: return TypeKNOB;
				case 3: return TypeBUTTON;
				case 4: return TypeBUTTONT;
				case 5: return TypeKNOBI;
				case 6: return TypeSLIDERI;
				case 7: return TypeFADERI;
				default: break;
			}
			return TypeNONE;
		}
		std::wstring_view MidiHelper::GetType(Common::MIDI::MidiUnitType& t) {
			return getType(t);
		}
		std::wstring_view MidiHelper::GetType(uint8_t& i) {
			return getType(static_cast<Common::MIDI::MidiUnitType>(i));
		}
		std::wstring_view MidiHelper::GetType(uint32_t i) {
			return getType(static_cast<Common::MIDI::MidiUnitType>(i));
		}

		std::wstring_view getScene(Common::MIDI::MidiUnitScene t) {
			
			switch (t) {
				case MidiUnitScene::SC1: return L"1"sv;
				case MidiUnitScene::SC2: return L"2"sv;
				case MidiUnitScene::SC3: return L"3"sv;
				case MidiUnitScene::SC4: return L"4"sv;
				case MidiUnitScene::SC1KNOB: return L"1"sv;
				case MidiUnitScene::SC2KNOB: return L"2"sv;
				case MidiUnitScene::SC3KNOB: return L"3"sv;
				case MidiUnitScene::SC4KNOB: return L"4"sv;
				case MidiUnitScene::SC4BTN: return L"4"sv;
				default: break;
			}
			return SceneNONE;
		}
		std::wstring_view MidiHelper::GetScene(Common::MIDI::MidiUnitScene& t) {
			return getScene(t);
		}
		std::wstring_view MidiHelper::GetScene(uint8_t& u) {
			return getScene(static_cast<Common::MIDI::MidiUnitScene>(u));
		}
		std::wstring_view MidiHelper::GetScene(uint32_t u) {
			return getScene(static_cast<Common::MIDI::MidiUnitScene>(u));
		}
	}
}
