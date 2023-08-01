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

		constexpr std::wstring_view strOnNoChange = L"On NO change"sv;
		constexpr std::wstring_view strOnChangeNone = L"On change NONE"sv;
		constexpr std::wstring_view strOnChangeNew = L"On change NEW"sv;
		constexpr std::wstring_view strOnChangeUpdateData = L"On change update DATA"sv;
		constexpr std::wstring_view strOnChangeUpdateVolume = L"On change update VOLUME"sv;
		constexpr std::wstring_view strOnChangeUpdatePan = L"On change update PANORAMA"sv;
		constexpr std::wstring_view strOnChangeUpdateMute = L"On change update MUTE"sv;
		constexpr std::wstring_view strOnChangeUpdateAll = L"On change update All, VOLUME + MUTE"sv;
		constexpr std::wstring_view strOnChangeRemove = L"On change DELETE"sv;

		constexpr std::wstring_view strOnChangeNoCbUpdateData = L"On change update DATA (NO CB CALL)"sv;
		constexpr std::wstring_view strOnChangeNoCbUpdateVolume = L"On change update VOLUME (NO CB CALL)"sv;
		constexpr std::wstring_view strOnChangeNoCbUpdatePan = L"On change update PANORAMA (NO CB CALL)"sv;
		constexpr std::wstring_view strOnChangeNoCbUpdateMute = L"On change update MUTE (NO CB CALL)"sv;
		constexpr std::wstring_view strOnChangeNoCbUpdateAll = L"On change update All, VOLUME + MUTE (NO CB CALL)"sv;


		constexpr std::wstring_view strReasonDeviceRemoval = L"device removal"sv;
		constexpr std::wstring_view strReasonServerShutdown = L"server shutdown"sv;
		constexpr std::wstring_view strReasonFormatChanged = L"format changed"sv;
		constexpr std::wstring_view strReasonSessionLogoff = L"session logoff"sv;
		constexpr std::wstring_view strReasonSessionDisconnected = L"session disconnected"sv;
		constexpr std::wstring_view strReasonExclusiveModeOverride = L"exclusive mode override"sv;


		/* static helper */

		std::wstring_view AudioSessionHelper::TypeItemsHelper(TypeItems type) {
			switch (type) {
				using enum TypeItems;
				case TypeSession: return strTypeSession;
				case TypeMaster: return strTypeMaster;
				case TypeDevice: return strTypeDevice;
				case TypeNone: return strTypeNone;
				default: return L"";
			}
		}
		std::wstring_view AudioSessionHelper::OnChangeTypeHelper(OnChangeType type) {
			switch (type) {
				using enum OnChangeType;
				case OnNoChange: return strOnNoChange;
				case OnChangeNone: return strOnChangeNone;
				case OnChangeNew:  return strOnChangeNew;
				case OnChangeRemove: return strOnChangeRemove;
				case OnChangeUpdateData: return strOnChangeUpdateData;
				case OnChangeUpdateVolume: return strOnChangeUpdateVolume;
				case OnChangeUpdatePan: return strOnChangeUpdatePan;
				case OnChangeUpdateMute:  return strOnChangeUpdateMute;
				case OnChangeUpdateAllValues: return strOnChangeUpdateAll;
				case OnChangeNoCbUpdateData: return strOnChangeNoCbUpdateData;
				case OnChangeNoCbUpdateVolume: return strOnChangeNoCbUpdateVolume;
				case OnChangeNoCbUpdatePan: return strOnChangeNoCbUpdatePan;
				case OnChangeNoCbUpdateMute:  return strOnChangeNoCbUpdateMute;
				case OnChangeNoCbUpdateAllValues: return strOnChangeNoCbUpdateAll;
				default: return L"";
			}
		}
		std::wstring_view AudioSessionHelper::DisconnectReasonHelper(AudioSessionDisconnectReason t) {
			switch (t) {
				using enum AudioSessionDisconnectReason;
				case DisconnectReasonFormatChanged: return strReasonFormatChanged;
				case DisconnectReasonSessionLogoff: return strReasonSessionLogoff;
				case DisconnectReasonDeviceRemoval: return strReasonDeviceRemoval;
				case DisconnectReasonServerShutdown: return strReasonServerShutdown;
				case DisconnectReasonSessionDisconnected: return strReasonSessionDisconnected;
				case DisconnectReasonExclusiveModeOverride: return strReasonExclusiveModeOverride;
				default: return L"";
			}
		}
	}
}


