/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIXER {

		using namespace std::string_view_literals;

		constexpr std::wstring_view strTypeSession = L"Type SESSION"sv;
		constexpr std::wstring_view strTypeMaster = L"Type MASTER"sv;
		constexpr std::wstring_view strTypeDevice = L"Type DEVICE"sv;
		constexpr std::wstring_view strTypeNone = L"Type NONE"sv;

		constexpr std::wstring_view strOnChangeNone = L"On change NONE"sv;
		constexpr std::wstring_view strOnChangeNew = L"On change NEW"sv;
		constexpr std::wstring_view strOnChangeUpdateData = L"On change update DATA"sv;
		constexpr std::wstring_view strOnChangeUpdateVolume = L"On change update VOLUME"sv;
		constexpr std::wstring_view strOnChangeUpdatePan = L"On change update PANORAMA"sv;
		constexpr std::wstring_view strOnChangeUpdateMute = L"On change update MUTE"sv;
		constexpr std::wstring_view strOnChangeRemove = L"On change DELETE"sv;

		constexpr std::wstring_view strOnChangeNoCbUpdateData = L"On change update DATA (NO CB CALL)"sv;
		constexpr std::wstring_view strOnChangeNoCbUpdateVolume = L"On change update VOLUME (NO CB CALL)"sv;
		constexpr std::wstring_view strOnChangeNoCbUpdatePan = L"On change update PANORAMA (NO CB CALL)"sv;
		constexpr std::wstring_view strOnChangeNoCbUpdateMute = L"On change update MUTE (NO CB CALL)"sv;

		/* static helper */

		std::wstring_view AudioSessionHelper::TypeItemsHelper(TypeItems type) {
			switch (type) {
				case Common::MIXER::TypeItems::TypeSession: return strTypeSession;
				case Common::MIXER::TypeItems::TypeMaster: return strTypeMaster;
				case Common::MIXER::TypeItems::TypeDevice: return strTypeDevice;
				case Common::MIXER::TypeItems::TypeNone: return strTypeNone;
				default: return L"";
			}
		}
		std::wstring_view AudioSessionHelper::OnChangeTypeHelper(OnChangeType type) {
			switch (type) {
				case Common::MIXER::OnChangeType::OnChangeNone: return strOnChangeNone;
				case Common::MIXER::OnChangeType::OnChangeNew:  return strOnChangeNew;
				case Common::MIXER::OnChangeType::OnChangeUpdateData: return strOnChangeUpdateData;
				case Common::MIXER::OnChangeType::OnChangeUpdateVolume: return strOnChangeUpdateVolume;
				case Common::MIXER::OnChangeType::OnChangeUpdatePan: return strOnChangeUpdatePan;
				case Common::MIXER::OnChangeType::OnChangeUpdateMute:  return strOnChangeUpdateMute;
				case Common::MIXER::OnChangeType::OnChangeNoCbUpdateData: return strOnChangeNoCbUpdateData;
				case Common::MIXER::OnChangeType::OnChangeNoCbUpdateVolume: return strOnChangeNoCbUpdateVolume;
				case Common::MIXER::OnChangeType::OnChangeNoCbUpdatePan: return strOnChangeNoCbUpdatePan;
				case Common::MIXER::OnChangeType::OnChangeNoCbUpdateMute:  return strOnChangeNoCbUpdateMute;
				case Common::MIXER::OnChangeType::OnChangeRemove: return strOnChangeRemove;
				default: return L"";
			}
		}
	}
}


