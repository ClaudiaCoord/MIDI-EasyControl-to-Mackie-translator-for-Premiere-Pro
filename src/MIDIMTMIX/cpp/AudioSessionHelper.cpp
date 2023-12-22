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

		class MixerNames {
		public:
			static constexpr std::wstring_view TypeSession = L"Type SESSION"sv;
			static constexpr std::wstring_view TypeMaster = L"Type MASTER"sv;
			static constexpr std::wstring_view TypeDevice = L"Type DEVICE"sv;
			static constexpr std::wstring_view TypeNone = L"Type NONE"sv;

			static constexpr std::wstring_view OnNoChange = L"On NO change"sv;
			static constexpr std::wstring_view OnChangeNone = L"On change NONE"sv;
			static constexpr std::wstring_view OnChangeNew = L"On change NEW"sv;
			static constexpr std::wstring_view OnChangeUpdateData = L"On change update DATA"sv;
			static constexpr std::wstring_view OnChangeUpdateVolume = L"On change update VOLUME"sv;
			static constexpr std::wstring_view OnChangeUpdatePan = L"On change update PANORAMA"sv;
			static constexpr std::wstring_view OnChangeUpdateMute = L"On change update MUTE"sv;
			static constexpr std::wstring_view OnChangeUpdateAll = L"On change update All, VOLUME + MUTE"sv;
			static constexpr std::wstring_view OnChangeRemove = L"On change DELETE"sv;

			static constexpr std::wstring_view OnChangeNoCbUpdateData = L"On change update DATA (NO CB CALL)"sv;
			static constexpr std::wstring_view OnChangeNoCbUpdateVolume = L"On change update VOLUME (NO CB CALL)"sv;
			static constexpr std::wstring_view OnChangeNoCbUpdatePan = L"On change update PANORAMA (NO CB CALL)"sv;
			static constexpr std::wstring_view OnChangeNoCbUpdateMute = L"On change update MUTE (NO CB CALL)"sv;
			static constexpr std::wstring_view OnChangeNoCbUpdateAll = L"On change update All, VOLUME + MUTE (NO CB CALL)"sv;

			static constexpr std::wstring_view ReasonDeviceRemoval = L"device removal"sv;
			static constexpr std::wstring_view ReasonServerShutdown = L"server shutdown"sv;
			static constexpr std::wstring_view ReasonFormatChanged = L"format changed"sv;
			static constexpr std::wstring_view ReasonSessionLogoff = L"session logoff"sv;
			static constexpr std::wstring_view ReasonSessionDisconnected = L"session disconnected"sv;
			static constexpr std::wstring_view ReasonExclusiveModeOverride = L"exclusive mode override"sv;
		};

		/* static helper */

		std::wstring_view AudioSessionHelper::TypeItemsHelper(TypeItems type) {
			switch (type) {
				using enum TypeItems;
				case TypeSession: return MixerNames::TypeSession;
				case TypeMaster: return MixerNames::TypeMaster;
				case TypeDevice: return MixerNames::TypeDevice;
				case TypeNone: return MixerNames::TypeNone;
				default: return L"";
			}
		}
		std::wstring_view AudioSessionHelper::OnChangeTypeHelper(OnChangeType type) {
			switch (type) {
				using enum OnChangeType;
				case OnNoChange: return MixerNames::OnNoChange;
				case OnChangeNone: return MixerNames::OnChangeNone;
				case OnChangeNew:  return MixerNames::OnChangeNew;
				case OnChangeRemove: return MixerNames::OnChangeRemove;
				case OnChangeUpdateData: return MixerNames::OnChangeUpdateData;
				case OnChangeUpdateVolume: return MixerNames::OnChangeUpdateVolume;
				case OnChangeUpdatePan: return MixerNames::OnChangeUpdatePan;
				case OnChangeUpdateMute:  return MixerNames::OnChangeUpdateMute;
				case OnChangeUpdateAllValues: return MixerNames::OnChangeUpdateAll;
				case OnChangeNoCbUpdateData: return MixerNames::OnChangeNoCbUpdateData;
				case OnChangeNoCbUpdateVolume: return MixerNames::OnChangeNoCbUpdateVolume;
				case OnChangeNoCbUpdatePan: return MixerNames::OnChangeNoCbUpdatePan;
				case OnChangeNoCbUpdateMute:  return MixerNames::OnChangeNoCbUpdateMute;
				case OnChangeNoCbUpdateAllValues: return MixerNames::OnChangeNoCbUpdateAll;
				default: return L"";
			}
		}
		std::wstring_view AudioSessionHelper::DisconnectReasonHelper(AudioSessionDisconnectReason t) {
			switch (t) {
				using enum AudioSessionDisconnectReason;
				case DisconnectReasonFormatChanged: return MixerNames::ReasonFormatChanged;
				case DisconnectReasonSessionLogoff: return MixerNames::ReasonSessionLogoff;
				case DisconnectReasonDeviceRemoval: return MixerNames::ReasonDeviceRemoval;
				case DisconnectReasonServerShutdown: return MixerNames::ReasonServerShutdown;
				case DisconnectReasonSessionDisconnected: return MixerNames::ReasonSessionDisconnected;
				case DisconnectReasonExclusiveModeOverride: return MixerNames::ReasonExclusiveModeOverride;
				default: return L"";
			}
		}
	}
}


