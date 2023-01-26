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

		void AudioSessionItemChangeData::Copy(AudioSessionItemChangeData& data) {
			Id = data.Id;
			Action = data.Action;
		}

		AudioSessionItemChange::AudioSessionItemChange() {
		}
		AudioSessionItemChange::AudioSessionItemChange(AudioSessionItem* item, OnChangeType t) {
			if (set_(item)) {
				actiontype__ = onchangetypefilter_(t);
				iscallaudiocb__ = onchangecbfilter_(t);
				Copy(item);
			}
		}
		const OnChangeType AudioSessionItemChange::GetAction() {
			return actiontype__;
		}
		const bool AudioSessionItemChange::IsCallAudioCb() const {
			return iscallaudiocb__;
		}
		const bool AudioSessionItemChange::IsValid() {
			if (Typeitem == TypeItems::TypeNone) return false;
			for (uint8_t i = 0U; i < AudioSessionItemChange::DataSize; i++) {
				AudioSessionItemChangeData& dst = Data[i];
				if ((dst.Id > 0U) && (dst.Action != AudioAction::AUDIO_NONE))
					return true;
			}
			return false;
		}
		[[maybe_unused]] const bool AudioSessionItemChange::IsKeyFound(uint32_t key) {
			for (uint8_t i = 0U; i < AudioSessionItemChange::DataSize; i++)
				if (Data[i].Id == key) return true;
			return false;
		}
		[[maybe_unused]] const bool AudioSessionItemChange::IsKeyFound(uint32_t key, AudioAction act) {
			switch (act) {
				case AudioAction::AUDIO_VOLUME:		if (Data[0].Id == key) return true; break;
				case AudioAction::AUDIO_PANORAMA:	if (Data[1].Id == key) return true; break;
				case AudioAction::AUDIO_MUTE:		if (Data[2].Id == key) return true; break;
				default: break;
			}
			return false;
		}
		const bool AudioSessionItemChange::IsKeyFound(AudioAction act) {
			switch (act) {
				case AudioAction::AUDIO_VOLUME:		if (Data[0].Id > 0U) return true; break;
				case AudioAction::AUDIO_PANORAMA:	if (Data[1].Id > 0U) return true; break;
				case AudioAction::AUDIO_MUTE:		if (Data[2].Id > 0U) return true; break;
				default: break;
			}
			return false;
		}
		const bool AudioSessionItemChange::IsKeyFound(AudioSessionItemChangeData data[]) {
			for (uint8_t i = 0U; i < AudioSessionItemChange::DataSize; i++)
				if ((data[i].Id != 0U) && (Data[i].Id == data[i].Id)) return true;
			return false;
		}

		void AudioSessionItemChange::Copy(AudioSessionItem* item) {
			Typeitem = item->Typeitem;
			Volume = item->Volume.GetVolume();
			Mute = item->Volume.GetMute();
			Name = std::wstring(item->GetName());
			Path = std::wstring(item->GetPath());
		}
		void AudioSessionItemChange::Copy(AudioSessionItemChange& item) {
			if (!set_(item)) return;

			Name = (item.Name.empty()) ? Name : std::wstring(item.Name);
			Path = (item.Path.empty()) ? Path : std::wstring(item.Path);

			Typeitem = item.Typeitem;
			Volume = item.Volume;
			Mute = item.Mute;
		}
		log_string AudioSessionItemChange::ToString() {
			log_string ls{};
			ls  << L"*[" << Name << L"], cmd:["
				<< AudioSessionHelper::OnChangeTypeHelper(GetAction()) << L"], ("
				<< Data[0].Id << L"|" << Data[1].Id << L"|" << Data[2].Id << L"), ("
				<< AudioSessionHelper::TypeItemsHelper(Typeitem)
				<< L"), { Volume:" << Volume << L", Mute:" << Mute << L" }\n";
			return ls;
		}

		/* private */

		bool AudioSessionItemChange::set_(AudioSessionItem* item) {

			if (item == nullptr) return false;

			bool found = false;
			uint32_t* ids = item->MidiId.Get();
			for (uint8_t i = 0U; i < AudioSessionItemChange::DataSize; i++) {
				AudioSessionItemChangeData& dst = Data[i];
				if ((dst.Id == 0U) && (ids[i] > 0U)) {
					dst.Id = ids[i];
					dst.Action = getaction_(i);
					found = true;
					continue;
				}
				if (dst.Id == ids[i]) {
					AudioAction act = getaction_(i);
					if (dst.Action != act) {
						dst.Action = act;
						found = true;
					}
				}
			}
			return found;
		}
		bool AudioSessionItemChange::set_(AudioSessionItemChange& item) {
			bool found = false;
			for (uint8_t i = 0U; i < AudioSessionItemChange::DataSize; i++) {
				AudioSessionItemChangeData& dst = Data[i],
										  & src = item.Data[i];
				if ((dst.Id == 0U) && (src.Id > 0U)) {
					if (!checkaction_(i, src.Action)) return false;
					dst.Copy(src);
					found = true;
					continue;
				}
				if (dst.Id == src.Id) {
					if ((dst.Action == src.Action) && (Typeitem == item.Typeitem))
						continue;
					if (!checkaction_(i, src.Action))
						dst.Copy(src);
					found = true;
				}
			}
			return found;
		}

		AudioAction AudioSessionItemChange::getaction_(uint8_t i) {
			switch (i) {
				case 0: return  AudioAction::AUDIO_VOLUME;
				case 1: return  AudioAction::AUDIO_PANORAMA;
				case 2: return  AudioAction::AUDIO_MUTE;
				default: return AudioAction::AUDIO_NONE;
			}
		}
		bool AudioSessionItemChange::checkaction_(uint8_t i, AudioAction act) {
				 if ((i == 0U) && (act == AudioAction::AUDIO_VOLUME)) return true;
			else if ((i == 1U) && (act == AudioAction::AUDIO_PANORAMA)) return true;
			else if ((i == 2U) && (act == AudioAction::AUDIO_MUTE)) return true;
			return false;
		}

		const OnChangeType AudioSessionItemChange::onchangetypefilter_(OnChangeType t) {
			switch (t) {
				case OnChangeType::OnChangeNoCbUpdatePan:    return OnChangeType::OnChangeUpdatePan;
				case OnChangeType::OnChangeNoCbUpdateData:   return OnChangeType::OnChangeUpdateData;
				case OnChangeType::OnChangeNoCbUpdateMute:   return OnChangeType::OnChangeUpdateMute;
				case OnChangeType::OnChangeNoCbUpdateVolume: return OnChangeType::OnChangeUpdateVolume;
				default: return t;
			}
		}
		const bool AudioSessionItemChange::onchangecbfilter_(OnChangeType t) {
			switch (t) {
				case OnChangeType::OnChangeNew:
				case OnChangeType::OnChangeNone:
				case OnChangeType::OnChangeRemove:
				case OnChangeType::OnChangeNoCbUpdatePan:
				case OnChangeType::OnChangeNoCbUpdateData:
				case OnChangeType::OnChangeNoCbUpdateMute:
				case OnChangeType::OnChangeNoCbUpdateVolume: return false;
				default: return true;
			}
		}
	}
}