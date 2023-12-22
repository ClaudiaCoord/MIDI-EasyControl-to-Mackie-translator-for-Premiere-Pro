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
			worker_background& wb_;
			std::shared_ptr<locker_awaiter> lock_;
			ASLIST_t aslist_;
			AULIST_t ctrlunitlist_;

			void add_list_(ASITEM_t);
			int32_t count_app_(const std::size_t&, const GUID&);
			void clear_list_by_dup_(ASITEM_t&);
			void rename_from_list_(ASITEM_t&, ASITEM_t&);
			void remove_from_list_(std::function<bool(ASITEM_t&)>);
			void update_values_(ASITEM_t&);
			void on_change_(ASITEM_t&, OnChangeType);
			ASITEM_t& get_from_list_by_guid_(LPCGUID);
			ASITEM_t& check_list_by_item_(ASITEM_t&);

			AudioSessionList() = default;

		public:

			std::atomic<bool> IsSetAudioLevelOldValue{ true };
			std::atomic<bool> IsDuplicateRemoved { false };
			std::function<void(ASUNIT_t&, ASITEM_t&)> set_param_cb_ = [](ASUNIT_t&, ASITEM_t&) {};
			std::function<void(AudioSessionItemChange)> event_send_cb_ = [](AudioSessionItemChange) {};

			AudioSessionList(
				uint32_t cpid,
				bool dupremove,
				bool oldval,
				std::function<void(ASUNIT_t&, ASITEM_t&)>,
				std::function<void(AudioSessionItemChange)>);
			~AudioSessionList();

			operator bool() const;

			void DuplicateRemoved(bool b);
			void SetControlUnits(std::shared_ptr<JSON::MMTConfig>& md);
			ASLIST_t& GetSessionList();

			void Add(ASITEM_t item);

			void RemoveByGuid(const GUID guid);
			void RemoveByPid(const uint32_t pid);
			void RemoveByAppId(const std::size_t title);
			void RemoveByAppName(const std::wstring name);
			void RemoveByType(const TypeItems t);

			ASITEM_t& GetByGuid(const GUID guid);
			ASITEM_t& GetByPid(const uint32_t pid);
			ASLIST_t GetByAppId(const std::size_t nameid);
			ASLIST_t GetByAppName(const std::wstring name);
			ASLIST_t GetByMidiId(const uint32_t midiid);
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
