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
			std::vector<AudioSessionItem*>  aslist__;
			std::vector<AudioSessionUnit>   ctrlunitlist__;

			void addtolist_(AudioSessionItem*);
			int32_t countapp_(const std::size_t&, const GUID&);
			void clearlistbydup_(AudioSessionItem*);
			void renamefromlist_(AudioSessionItem*, AudioSessionItem*);
			void removefromlist_(std::function<bool(AudioSessionItem*&)>);
			void updatevalues_(AudioSessionItem*);
			AudioSessionItem* getfromlistbyguid_(LPCGUID);
			AudioSessionItem* checklistbyitem_(AudioSessionItem*);

			AudioSessionList() = default;

		public:

			std::atomic<bool> IsSetAudioLevelOldValue{ true };
			std::atomic<bool> IsDuplicateRemoved { false };
			std::function<void(AudioSessionUnit&, AudioSessionItem*&)> SetParamCb = [](AudioSessionUnit&, AudioSessionItem*&) {};

			AudioSessionList(
				uint32_t cpid,
				bool dupremove,
				bool oldval,
				std::function<void(AudioSessionUnit&, AudioSessionItem*&)> vcb);
			~AudioSessionList();

			operator bool() const;

			void DuplicateRemoved(bool b);
			std::vector<AudioSessionItem*>& GetSessionList();
			void SetControlUnits(std::shared_ptr<Common::MIDI::MidiDevice>& md);

			void Add(AudioSessionItem* item);

			void RemoveByGuid(const GUID guid);
			void RemoveByPid(const uint32_t pid);
			void RemoveByAppId(const std::size_t title);
			void RemoveByAppName(const std::wstring name);
			void RemoveByType(const TypeItems t);

			AudioSessionItem* GetByGuid(const GUID guid);
			AudioSessionItem* GetByPid(const uint32_t pid);
			std::vector<AudioSessionItem*> GetByAppId(const std::size_t nameid);
			std::vector<AudioSessionItem*> GetByAppName(const std::wstring name);
			std::vector<AudioSessionItem*> GetByMidiId(const uint32_t midiid);
			int32_t CountByAppId(const std::size_t&);

			bool   IsEmpty();
			void   Clear();
			size_t Count();

			void DisplayNameChange(LPCGUID, LPCWSTR);
			void IconPathChange(LPCGUID, LPCWSTR);
			void VolumeChange(LPCGUID, float v, bool m);
			void VolumeChange(LPCGUID, float v);
			void GuidChange(LPCGUID, LPCGUID);
			void AliveChange();
		};
	}
}
