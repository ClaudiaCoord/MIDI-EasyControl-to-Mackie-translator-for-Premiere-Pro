/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIXER {

		class AudioSessionList {
		private:
			worker_background& wb__;
			std::shared_ptr<locker_awaiter> lock__;
			std::vector<AudioSessionItem*> onlinelist__;
			std::vector<Common::MIDI::MixerUnit> ctrlunitlist__;

			void addtolist_(AudioSessionItem*);
			void clearlistbydup_(AudioSessionItem*);
			void removefromlist_(std::function<bool(AudioSessionItem*&)>);
			AudioSessionItem* getfromlistbyguid_(LPCGUID);
			AudioSessionItem* checklistbyitem_(AudioSessionItem*);
			std::tuple<ListState, AudioSessionItem*> checkfromlist_(const uint32_t&, std::wstring&);

			AudioSessionList() = default;

		public:

			std::atomic<bool> IsSetAudioLevelOldValue;
			std::atomic<bool> IsDuplicateRemoved = true;
			std::function<void(Common::MIDI::MixerUnit&, AudioSessionItem*&)> SetParamCb = [](Common::MIDI::MixerUnit&, AudioSessionItem*&) {};

			AudioSessionList(
				uint32_t cpid,
				bool oldval,
				std::function<void(Common::MIDI::MixerUnit&, AudioSessionItem*&)> vcb);
			~AudioSessionList();

			FLAG_EXPORT operator bool() const;

			FLAG_EXPORT void DuplicateRemoved(bool b);
			FLAG_EXPORT std::vector<AudioSessionItem*>& GetSessionList();
			FLAG_EXPORT void SetControlUnits(std::shared_ptr<Common::MIDI::MidiDevice>& md);

			FLAG_EXPORT void Add(AudioSessionItem* item);

			FLAG_EXPORT void Remove(GUID& guid);
			FLAG_EXPORT void Remove(uint32_t& pid);
			FLAG_EXPORT void Remove(std::wstring& title);
			FLAG_EXPORT void Remove(TypeItems& t);

			FLAG_EXPORT AudioSessionItem* Get(GUID& guid);
			FLAG_EXPORT AudioSessionItem* Get(uint32_t& pid);
			FLAG_EXPORT AudioSessionItem* Get(std::wstring& name);
			FLAG_EXPORT AudioSessionItem* GetByMidiId(uint32_t midiid);
			FLAG_EXPORT ListState Found(uint32_t& pid, std::wstring& name);

			FLAG_EXPORT bool   IsEmpty();
			FLAG_EXPORT void   Clear();
			FLAG_EXPORT size_t Count();

			void DisplayNameChange(LPCGUID, LPCWSTR);
			void IconPathChange(LPCGUID, LPCWSTR);
			void VolumeChange(LPCGUID, float v, bool m);
			void VolumeChange(LPCGUID, float v);
			void GuidChange(LPCGUID, LPCGUID);
			void AliveChange();
		};
	}
}
