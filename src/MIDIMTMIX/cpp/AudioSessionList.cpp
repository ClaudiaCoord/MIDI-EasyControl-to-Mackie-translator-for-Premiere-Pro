/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTMIX DLL

	See README.mmt for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include <rpcdce.h>
#include <unordered_set>

namespace Common {
    namespace MIXER {

        static ASITEM_t item_empty__{};
        static ASLIST_t list_empty__{};

        static uint32_t CPID{ 0U };
        struct GUIDEqual {
            const bool operator()(const GUID& a, const GUID& b) const {
                return ::IsEqualGUID(a, b);
            }
            std::size_t operator()(const GUID& g) const {
                RPC_STATUS s = RPC_S_OK;
                return ::UuidHash(&const_cast<GUID&>(g), &s);
            }
        };

        /* template */

        template<typename T1>
        ASITEM_t& getitemfromlist_(
            ASLIST_t& v,
            const T1& val,
            std::function<bool(const T1&, ASITEM_t&)> cmpcb) {

            try {
                do {
                    if (v.empty()) break;
                    auto it = std::find_if(v.begin(), v.end(), [val, cmpcb](ASITEM_t& a) {
                        if (!a) return false;
                        return cmpcb(val, a);
                        });
                    if (it == v.end()) break;
                    return *static_cast<ASITEM_t*>(&*it);
                } while (0);
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return std::ref(item_empty__);
        }
        ASLIST_t getitemsfromlist_(ASLIST_t& v, std::function<bool(ASITEM_t&)> cmpcb) {
            ASLIST_t ov{};
            try {
                for (auto& a : v)
                    if (cmpcb(a)) ov.push_back(a);

            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return ov;
        }

        void AudioSessionList::on_change_(ASITEM_t& item, OnChangeType type) {
            try {
                if (item) event_send_cb_(std::move(item->GetSessionItemChange(type, true)));
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }

        /* class private */

        ASITEM_t& AudioSessionList::get_from_list_by_guid_(LPCGUID g) {
            return getitemfromlist_<GUID>(aslist_, static_cast<GUID>(*g), [&](const GUID& val, ASITEM_t& a) {
                if (!a) return false;
                return val == a->Item.App.Guid;
                });
        }
        ASITEM_t& AudioSessionList::check_list_by_item_(ASITEM_t& item) {
            const std::size_t nameid = item->Item.App.get<std::size_t>();
            auto it = std::find_if(aslist_.begin(), aslist_.end(), [nameid](ASITEM_t& a) {
                if (!a) return false;
                return (nameid == a->Item.App.get<std::size_t>());
                });
            if (it == aslist_.end()) return std::ref(item_empty__);
            return *static_cast<ASITEM_t*>(&*it);
        }
        int32_t AudioSessionList::count_app_(const std::size_t& nameid, const GUID& g) {
            try {
                std::unordered_set<GUID, GUIDEqual, GUIDEqual> v{};
                for (auto& a : aslist_) {
                    if (a && (nameid != a->Item.App.get<std::size_t>())) continue;
                    if ((g != GUID_NULL) ? (g != a->Item.App.Guid) : true) v.insert(a->Item.App.Guid);
                }
                return static_cast<int32_t>(v.size());
            } catch (...) {}
            return 0;
        }
        void AudioSessionList::rename_from_list_(ASITEM_t& item, ASITEM_t& asi) {
            try {
                int32_t cnt = count_app_(item->Item.App.get<std::size_t>(), item->Item.App.Guid);
                if (cnt > 0) {
                    if ((cnt == 1) && (asi)) {
                        asi->Item.App.set(cnt, asi->Item.App.get<std::wstring>());
                        on_change_(asi, OnChangeType::OnChangeUpdateData);
                    }
                    item->Item.App.set(cnt + 1, item->Item.App.get<std::wstring>());
                }
            } catch (...) {}
        }
        void AudioSessionList::clear_list_by_dup_(ASITEM_t& item) {
            const bool isdup = IsDuplicateRemoved.load();
            remove_from_list_([&](ASITEM_t& a) {
                if (!a) return true;
                return (isdup &&
                    (a->Item.IsEqualsApp(item->Item.App.get<std::size_t>()) ||
                     a->Item.App.Pid == item->Item.App.Pid)) ||
                    (a->Item.App.Guid == item->Item.App.Guid);
            });
        }
        void AudioSessionList::add_list_(ASITEM_t item) {
            try {
                if (!item) return;
                if (item->Item.IsEmptyApp() || (item->Item.App.Pid == CPID)) {
                    item.reset();
                    return;
                }

                if (!aslist_.empty()) {
                    ASITEM_t& asi = check_list_by_item_(item);

                    if (asi && !asi->IsEmpty()) {
                        if (item->Item.App.Guid == asi->Item.App.Guid) {
                            asi->Item.App.copy(item->Item.App);
                            item.reset();
                            on_change_(asi, OnChangeType::OnChangeUpdateData);
                            return;
                        }
                        else if (IsDuplicateRemoved) {
                            item->Item.Volume.copy(asi->Item.Volume, OnChangeType::OnChangeNone);
                            clear_list_by_dup_(item);
                        }
                        else rename_from_list_(item, asi);
                    }
                }

                if (!ctrlunitlist_.empty()) {
                    try {
                        bool once{ false };
                        for (auto& u : ctrlunitlist_) {
                            if (u && u->AppFound(item->Item.App.get<std::size_t>())) {
                                item->Item.Id.add(u->GetData());
                                if (IsSetAudioLevelOldValue && !once) {
                                    set_param_cb_(u, item);
                                    once = true;
                                }
                            }
                        }
                    } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }

                item->RegistrySession(true, this);
                on_change_(std::ref(item), OnChangeType::OnChangeNew);
                aslist_.push_back(std::move(item));

            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }
        void AudioSessionList::remove_from_list_(std::function<bool(ASITEM_t&)> cmpcb) {
            try {
                if (aslist_.empty()) return;
                auto it = std::remove_if(aslist_.begin(), aslist_.end(), [=](ASITEM_t& a) {
                    bool b = false;
                    try {
                        if (!a) return true;
                        if ((b = cmpcb(a))) {
                            on_change_(a, OnChangeType::OnChangeRemove);
                            update_values_(a);
                            a.reset();
                        }
                    } catch (...) {}
                    return b;
                    });
                if (it != aslist_.end()) aslist_.erase(it);

            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }
        void AudioSessionList::update_values_(ASITEM_t& item) {
            if (IsSetAudioLevelOldValue) {
                try {
                    const uint8_t v = item->Item.Volume.get<uint8_t>();
                    const bool m = item->Item.Volume.get<bool>();

                    for (auto& u : ctrlunitlist_)
                        if (u->AppFound(item->Item.App.get<std::size_t>())) u->UpdateVolume(v, m);

                } catch (...) {}
            }
        }

        /* class public */

        AudioSessionList::AudioSessionList(
            uint32_t cpid,
            bool dupremove,
            bool oldval,
            std::function<void(ASUNIT_t&, ASITEM_t&)> vcb,
            std::function<void(AudioSessionItemChange)> es)
            : IsDuplicateRemoved(dupremove), IsSetAudioLevelOldValue(oldval), set_param_cb_(vcb), event_send_cb_(es), wb_(worker_background::Get()) {
            CPID = cpid;
            lock_ = std::make_shared<locker_awaiter>();
        }
        AudioSessionList::~AudioSessionList() {
            try {
                lock_->reset();
                Clear();
            } catch (...) {}
        }
        AudioSessionList::operator bool() const {
            return aslist_.size() > 0;
        }

        void AudioSessionList::DuplicateRemoved(bool b) {
            IsDuplicateRemoved = b;
        }
        ASLIST_t& AudioSessionList::GetSessionList() {
            return std::ref(aslist_);
        }

        size_t AudioSessionList::Count() { return aslist_.size(); }
        bool AudioSessionList::IsEmpty() { return aslist_.empty(); }
        void AudioSessionList::Clear() {
            locker_auto locker(lock_, locker_auto::LockType::TypeLock);
            if (aslist_.empty()) return;
            try {
                ASLIST_t list(aslist_);
                aslist_.clear();
                for (auto& a : list) {
                    if (a) update_values_(a);
                    a.reset();
                }
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }
        void AudioSessionList::Add(ASITEM_t item) {
            wb_.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock_, locker_auto::LockType::TypeLock);
                add_list_(std::move(item));
                })
            );
        }
        void AudioSessionList::RemoveByPid(const uint32_t pid) {
            wb_.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock_, locker_auto::LockType::TypeLock);
                try {
                    if (locker.IsCanceled()) return;
                    remove_from_list_([&](ASITEM_t& a) {
                        if (!a) return true;
                        return a->Item.App.Pid == pid;
                        });
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::RemoveByGuid(const GUID guid) {
            wb_.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock_, locker_auto::LockType::TypeLock);
                try {
                    if (locker.IsCanceled()) return;
                    remove_from_list_([&](ASITEM_t& a) {
                        if (!a) return true;
                        return a->Item.App.Guid == guid;
                        });
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::RemoveByAppId(const std::size_t nameid) {
            wb_.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock_, locker_auto::LockType::TypeLockWait);
                try {
                    if (locker.IsCanceled()) return;
                    remove_from_list_([&](ASITEM_t& a) {
                        if (!a) return true;
                        return nameid == a->Item.App.get<std::size_t>();
                        });
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::RemoveByAppName(const std::wstring name) {
            std::size_t nameid = AudioSessionItemApp::hash(name);
            RemoveByAppId(nameid);
        }
        void AudioSessionList::RemoveByType(const TypeItems t) {
            wb_.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock_, locker_auto::LockType::TypeLockWait);
                try {
                    if (locker.IsCanceled()) return;
                    remove_from_list_([&](ASITEM_t& a) {
                        if (a == nullptr) return true;
                        return a->Item.GetType() == t;
                        });
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }

        ASITEM_t& AudioSessionList::GetByPid(const uint32_t pid) {
            if (lock_->IsLock()) return std::ref(item_empty__);
            try {
                return getitemfromlist_<uint32_t>(aslist_, pid, [&](const uint32_t& val, ASITEM_t& a) {
                    if (!a) return false;
                    return a->Item.App.Pid == val;
                    });
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return std::ref(item_empty__);
        }
        ASITEM_t& AudioSessionList::GetByGuid(const GUID guid) {
            if (lock_->IsLock()) return std::ref(item_empty__);
            try {
                return getitemfromlist_<GUID>(aslist_, guid, [&](const GUID& val, ASITEM_t& a) {
                    if (!a) return false;
                    return a->Item.App.Guid == val;
                    });
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return std::ref(item_empty__);
        }
        ASLIST_t AudioSessionList::GetByAppId(const std::size_t nameid) {
            if (lock_->IsLock()) return std::ref(list_empty__);
            try {
                return getitemsfromlist_(aslist_, [=](ASITEM_t& a) {
                    return nameid == a->Item.App.get<std::size_t>();
                });
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return std::ref(list_empty__);
        }
        ASLIST_t AudioSessionList::GetByAppName(const std::wstring name) {
            std::size_t nameid = AudioSessionItemApp::hash(name);
            return GetByAppId(nameid);
        }
        ASLIST_t AudioSessionList::GetByMidiId(const uint32_t midikey) {
            if (lock_->IsLock()) return std::ref(list_empty__);
            try {
                return getitemsfromlist_(aslist_, [=](ASITEM_t& a) {
                    return (a->Item.FoundId(midikey) != MIDI::MidiUnitType::UNITNONE);
                });
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return std::ref(list_empty__);
        }
        int32_t AudioSessionList::CountByAppId(const std::size_t& nameid) {
            if (lock_->IsLock()) return 0;
            return count_app_(nameid, GUID_NULL);
        }

        /* Event Changed */

        void AudioSessionList::DisplayNameChange(LPCGUID g, LPCWSTR s) {
            wb_.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock_, locker_auto::LockType::TypeLockWait);
                try {
                    ASITEM_t& item = get_from_list_by_guid_(g);
                    if (item && !item->IsEmpty()) {
                        item->Item.App.set(static_cast<std::size_t>(1), Utils::to_string((LPWSTR)s));
                        on_change_(item, OnChangeType::OnChangeNoCbUpdateData);
                    }
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::IconPathChange(LPCGUID g, LPCWSTR s) {
            wb_.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock_, locker_auto::LockType::TypeLockWait);
                try {
                    ASITEM_t& item = get_from_list_by_guid_(g);
                    if (item && !item->IsEmpty()) item->Item.App.Icon = Utils::to_string((LPWSTR)s);
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::GuidChange(LPCGUID g, LPCGUID gn) {
            wb_.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock_, locker_auto::LockType::TypeLockWait);
                try {
                    ASITEM_t& item = get_from_list_by_guid_(g);
                    if (item && !item->IsEmpty()) item->Item.App.Guid = static_cast<GUID>(*gn);
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::VolumeChange(LPCGUID g, float v, bool m) {
            wb_.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock_, locker_auto::LockType::TypeLockWait);
                try {
                    ASITEM_t& item = get_from_list_by_guid_(g);
                    if (item && !item->IsEmpty()) {
                        bool mute = item->Item.Volume.get<bool>();
                        item->Item.Volume.set(v);
                        item->Item.Volume.set(m);
                        on_change_(item, (mute == m) ? OnChangeType::OnChangeNoCbUpdateVolume : OnChangeType::OnChangeNoCbUpdateMute);
                    }
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::VolumeChange(LPCGUID g, float v) {
            wb_.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock_, locker_auto::LockType::TypeLockWait);
                try {
                    ASITEM_t& item = get_from_list_by_guid_(g);
                    if (item && !item->IsEmpty()) {
                        item->Item.Volume.set(v);
                        on_change_(item, OnChangeType::OnChangeNoCbUpdateVolume);
                    }
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::AliveChange() {
            if (lock_->IsOnlyOne()) return;

            wb_.to_async(std::async(std::launch::async, [=](ASLIST_t& v, std::shared_ptr<locker_awaiter>& lock) {
                locker_auto locker(lock, locker_auto::LockType::TypeLockOnlyOne);
                if (!locker.Begin()) return;

                try {
                    do {
                        if (v.empty()) break;
                        
                        std::vector<uint32_t> vr{};
                        for (ASITEM_t& a : v) {
                            if (a && (a->Item.App.Pid != 0U) && (!Utils::is_pid_running(a->Item.App.Pid)))
                                vr.push_back(a->Item.App.Pid);
                        }
                        if (vr.empty()) break;
                        if (locker.IsCanceled()) return;

                        remove_from_list_([&](ASITEM_t& a) {
                            if (!a) return true;
                            for (uint32_t& i : vr)
                                if (i == a->Item.App.Pid) return true;
                            return false;
                            });
                    } while (0);
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            }, std::ref(aslist_), std::ref(lock_)));
        }

        /* MIDI key Changed */

        void AudioSessionList::SetControlUnits(std::shared_ptr<JSON::MMTConfig>& mmt) {

            if (!mmt) return;
            try {
                wb_.to_async(std::async(std::launch::async, [=]() {
                    try {
                        do {
                            ctrlunitlist_.clear();
                            for (auto& u : mmt->units) {
                                try {
                                    if ((u.target == MIDI::Mackie::Target::VOLUMEMIX) && (!u.appvolume.empty()))
                                        ctrlunitlist_.push_back(std::make_shared<AudioSessionUnit>(u));
                                } catch (...) {}
                            }

                            locker_auto locker(lock_, locker_auto::LockType::TypeLockWait);
                            if (locker.IsCanceled()) break;

                            if (aslist_.empty()) break;

                            for (auto& a : aslist_) {
                                if (!a) continue;
                                bool once{ false };
                                for (auto& u : ctrlunitlist_) {
                                    if (u && u->AppFound(a->Item.App.get<std::size_t>())) {
                                        a->Item.Id.add(u->GetData());
                                        if (IsSetAudioLevelOldValue && !once) {
                                            set_param_cb_(u, a);
                                            once = true;
                                        }
                                        on_change_(a, OnChangeType::OnChangeNoCbUpdateData);
                                    }
                                }
                            }
                        } while (0);

                    } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }
    }
}