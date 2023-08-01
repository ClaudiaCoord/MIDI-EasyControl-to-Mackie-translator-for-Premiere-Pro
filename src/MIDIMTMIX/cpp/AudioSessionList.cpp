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
#include <rpcdce.h>
#include <unordered_set>

namespace Common {
    namespace MIXER {

        static uint32_t CPID;
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
        AudioSessionItem* getitemfromlist_(std::vector<AudioSessionItem*>& v, const T1& val, std::function<bool(const T1&, AudioSessionItem*&)> cmpcb) {
            try {
                do {
                    if (v.empty()) break;
                    auto it = std::find_if(v.begin(), v.end(), [val, cmpcb](AudioSessionItem*& a) {
                        if (a == nullptr) return false;
                        return cmpcb(val, a);
                        });
                    if (it == v.end()) break;
                    return static_cast<AudioSessionItem*>(*it);
                } while (0);
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return nullptr;
        }
        std::vector<AudioSessionItem*> getitemsfromlist_(std::vector<AudioSessionItem*>& v, std::function<bool(AudioSessionItem*&)> cmpcb) {
            std::vector<AudioSessionItem*> ov{};
            try {
                for (auto& a : v)
                    if (cmpcb(a)) ov.push_back(a);

            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return ov;
        }

        void on_change_(AudioSessionItem* item, OnChangeType type) {
            try {
                AudioSessionMixer::Get().event_send(item->GetSessionItemChange(type, true));
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }

        /* class private */

        AudioSessionItem* AudioSessionList::getfromlistbyguid_(LPCGUID g) {
            return getitemfromlist_<GUID>(aslist__, static_cast<GUID>(*g), [&](const GUID& val, AudioSessionItem*& a) {
                if (a == nullptr) return false;
                return val == a->Item.App.Guid;
                });
        }
        AudioSessionItem* AudioSessionList::checklistbyitem_(AudioSessionItem* item) {
            const std::size_t nameid = item->Item.App.get<std::size_t>();
            auto it = std::find_if(aslist__.begin(), aslist__.end(), [nameid](AudioSessionItem*& a) {
                if (a == nullptr) return false;
                return (nameid == a->Item.App.get<std::size_t>());
                });
            if (it == aslist__.end()) return nullptr;
            return static_cast<AudioSessionItem*>(*it);
        }
        int32_t AudioSessionList::countapp_(const std::size_t& nameid, const GUID& g) {
            try {
                std::unordered_set<GUID, GUIDEqual, GUIDEqual> v{};
                for (auto& a : aslist__) {
                    if (nameid != a->Item.App.get<std::size_t>()) continue;
                    if ((g != GUID_NULL) ? (g != a->Item.App.Guid) : true) v.insert(a->Item.App.Guid);
                }
                return static_cast<int32_t>(v.size());
            } catch (...) {}
            return 0;
        }
        void AudioSessionList::renamefromlist_(AudioSessionItem* item, AudioSessionItem* asi) {
            try {
                int32_t cnt = countapp_(item->Item.App.get<std::size_t>(), item->Item.App.Guid);
                if (cnt > 0) {
                    if ((cnt == 1) && (asi != nullptr)) {
                        asi->Item.App.set(cnt, asi->Item.App.get<std::wstring>());
                        on_change_(asi, OnChangeType::OnChangeUpdateData);
                    }
                    item->Item.App.set(cnt + 1, item->Item.App.get<std::wstring>());
                }
            } catch (...) {}
        }
        void AudioSessionList::clearlistbydup_(AudioSessionItem* item) {
            const bool isdup = IsDuplicateRemoved.load();
            removefromlist_([&](AudioSessionItem*& a) {
                if (a == nullptr) return true;
                return (isdup && (a->Item.IsEqualsApp(item->Item.App.get<std::size_t>()) || a->Item.App.Pid == item->Item.App.Pid)) || (a->Item.App.Guid == item->Item.App.Guid);
            });
        }
        void AudioSessionList::addtolist_(AudioSessionItem* item) {
            try {
                if (item == nullptr) return;
                if (item->Item.IsEmptyApp() || (item->Item.App.Pid == CPID)) {
                    delete item;
                    return;
                }

                if (!aslist__.empty()) {
                    AudioSessionItem* asi = checklistbyitem_(item);

                    if (asi != nullptr) {
                        if (item->Item.App.Guid == asi->Item.App.Guid) {
                            asi->Item.App.copy(item->Item.App);
                            delete item;
                            on_change_(asi, OnChangeType::OnChangeUpdateData);
                            return;
                        }
                        else if (IsDuplicateRemoved) {
                            item->Item.Volume.copy(asi->Item.Volume, OnChangeType::OnChangeNone);
                            clearlistbydup_(item);
                        }
                        else {
                            renamefromlist_(item, asi);
                        }
                    }
                }

                if (!ctrlunitlist__.empty()) {
                    try {
                        bool once{ false };
                        for (auto& u : ctrlunitlist__) {
                            if (u.AppFound(item->Item.App.get<std::size_t>())) {
                                item->Item.Id.add(u.GetData());
                                if (IsSetAudioLevelOldValue && !once) {
                                    SetParamCb(u, item);
                                    once = true;
                                }
                            }
                        }
                    } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }

                item->RegistrySession(true, this);
                aslist__.push_back(item);
                on_change_(item, OnChangeType::OnChangeNew);

            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }
        void AudioSessionList::removefromlist_(std::function<bool(AudioSessionItem*&)> cmpcb) {
            try {
                if (aslist__.empty()) return;
                auto it = std::remove_if(aslist__.begin(), aslist__.end(), [=](AudioSessionItem*& a) {
                    bool b = false;
                    try {
                        if (a == nullptr) return true;
                        if ((b = cmpcb(a))) {
                            on_change_(a, OnChangeType::OnChangeRemove);
                            a->RegistrySession(false);
                            updatevalues_(a);
                            delete a;
                        }
                    } catch (...) {}
                    return b;
                    });
                if (it != aslist__.end())
                    aslist__.erase(it, aslist__.end());
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }
        void AudioSessionList::updatevalues_(AudioSessionItem* item) {
            if (IsSetAudioLevelOldValue) {
                try {
                    const uint8_t v = item->Item.Volume.get<uint8_t>();
                    const bool m = item->Item.Volume.get<bool>();

                    for (auto& u : ctrlunitlist__)
                        if (u.AppFound(item->Item.App.get<std::size_t>())) u.UpdateVolume(v, m);

                } catch (...) {}
            }
        }

        /* class public */

        AudioSessionList::AudioSessionList(
            uint32_t cpid,
            bool dupremove,
            bool oldval,
            std::function<void(AudioSessionUnit&, AudioSessionItem*&)> vcb)
            : IsDuplicateRemoved(dupremove), IsSetAudioLevelOldValue(oldval), SetParamCb(vcb), wb__(worker_background::Get()) {
            CPID = cpid;
            lock__ = std::make_shared<locker_awaiter>();
        }
        AudioSessionList::~AudioSessionList() {
            try {
                lock__->reset();
                Clear();
            } catch (...) {}
        }
        AudioSessionList::operator bool() const {
            return aslist__.size() > 0;
        }

        void AudioSessionList::DuplicateRemoved(bool b) {
            IsDuplicateRemoved = b;
        }
        std::vector<AudioSessionItem*>& AudioSessionList::GetSessionList() {
            return std::ref(aslist__);
        }

        size_t AudioSessionList::Count() { return aslist__.size(); }
        bool AudioSessionList::IsEmpty() { return aslist__.empty(); }
        void AudioSessionList::Clear() {
            locker_auto locker(lock__, locker_auto::LockType::TypeLock);
            if (aslist__.empty()) return;
            try {
                for (AudioSessionItem* a : aslist__) {
                    if (a != nullptr) {
                        a->RegistrySession(false);
                        updatevalues_(a);
                        delete a;
                    }
                }
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            aslist__.clear();
        }
        void AudioSessionList::Add(AudioSessionItem* item) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLock);
                addtolist_(item);
                }));
        }
        void AudioSessionList::RemoveByPid(const uint32_t pid) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLock);
                try {
                    if (locker.IsCanceled()) return;
                    removefromlist_([&](AudioSessionItem*& a) {
                        if (a == nullptr) return true;
                        return a->Item.App.Pid == pid;
                        });
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::RemoveByGuid(const GUID guid) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLock);
                try {
                    if (locker.IsCanceled()) return;
                    removefromlist_([&](AudioSessionItem*& a) {
                        if (a == nullptr) return true;
                        return a->Item.App.Guid == guid;
                        });
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::RemoveByAppId(const std::size_t nameid) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                try {
                    if (locker.IsCanceled()) return;
                    removefromlist_([&](AudioSessionItem*& a) {
                        if (a == nullptr) return true;
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
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                try {
                    if (locker.IsCanceled()) return;
                    removefromlist_([&](AudioSessionItem*& a) {
                        if (a == nullptr) return true;
                        return a->Item.GetType() == t;
                        });
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }

        AudioSessionItem* AudioSessionList::GetByPid(const uint32_t pid) {
            if (lock__->IsLock()) return nullptr;
            try {
                return getitemfromlist_<uint32_t>(aslist__, pid, [&](const uint32_t& val, AudioSessionItem*& a) {
                    if (a == nullptr) return false;
                    return a->Item.App.Pid == val;
                    });
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return nullptr;
        }
        AudioSessionItem* AudioSessionList::GetByGuid(const GUID guid) {
            if (lock__->IsLock()) return nullptr;
            try {
                return getitemfromlist_<GUID>(aslist__, guid, [&](const GUID& val, AudioSessionItem*& a) {
                    if (a == nullptr) return false;
                    return a->Item.App.Guid == val;
                    });
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return nullptr;
        }
        std::vector<AudioSessionItem*> AudioSessionList::GetByAppId(const std::size_t nameid) {
            do {
                if (lock__->IsLock()) break;
                try {
                    return getitemsfromlist_(aslist__, [=](AudioSessionItem*& a) {
                        return nameid == a->Item.App.get<std::size_t>();
                    });
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            } while (0);
            return std::vector<AudioSessionItem*>();
        }
        std::vector<AudioSessionItem*> AudioSessionList::GetByAppName(const std::wstring name) {
            std::size_t nameid = AudioSessionItemApp::hash(name);
            return GetByAppId(nameid);
        }
        std::vector<AudioSessionItem*> AudioSessionList::GetByMidiId(const uint32_t midikey) {
            do {
                if (lock__->IsLock()) break;
                try {
                    return getitemsfromlist_(aslist__, [=](AudioSessionItem*& a) {
                        return (a->Item.FoundId(midikey) != MIDI::MidiUnitType::UNITNONE);
                    });
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            } while (0);
            return std::vector<AudioSessionItem*>();
        }
        int32_t AudioSessionList::CountByAppId(const std::size_t& nameid) {
            if (lock__->IsLock()) return 0;
            return countapp_(nameid, GUID_NULL);
        }

        /* Event Changed */

        void AudioSessionList::DisplayNameChange(LPCGUID g, LPCWSTR s) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                try {
                    AudioSessionItem* item = getfromlistbyguid_(g);
                    if (item != nullptr) {
                        item->Item.App.set(static_cast<std::size_t>(1), Utils::to_string((LPWSTR)s));
                        on_change_(item, OnChangeType::OnChangeNoCbUpdateData);
                    }
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::IconPathChange(LPCGUID g, LPCWSTR s) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                try {
                    AudioSessionItem* item = getfromlistbyguid_(g);
                    if (item != nullptr) item->Item.App.Icon = Utils::to_string((LPWSTR)s);
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::GuidChange(LPCGUID g, LPCGUID gn) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                try {
                    AudioSessionItem* item = getfromlistbyguid_(g);
                    if (item != nullptr) item->Item.App.Guid = static_cast<GUID>(*gn);
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::VolumeChange(LPCGUID g, float v, bool m) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                try {
                    AudioSessionItem* item = getfromlistbyguid_(g);
                    if (item != nullptr) {
                        bool mute = item->Item.Volume.get<bool>();
                        item->Item.Volume.set(v);
                        item->Item.Volume.set(m);
                        on_change_(item, (mute == m) ? OnChangeType::OnChangeNoCbUpdateVolume : OnChangeType::OnChangeNoCbUpdateMute);
                    }
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::VolumeChange(LPCGUID g, float v) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                try {
                    AudioSessionItem* item = getfromlistbyguid_(g);
                    if (item != nullptr) {
                        item->Item.Volume.set(v);
                        on_change_(item, OnChangeType::OnChangeNoCbUpdateVolume);
                    }
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::AliveChange() {
            if (lock__->IsOnlyOne()) return;

            wb__.to_async(std::async(std::launch::async, [=](std::vector<AudioSessionItem*>& v, std::shared_ptr<locker_awaiter>& lock) {
                locker_auto locker(lock, locker_auto::LockType::TypeLockOnlyOne);
                if (locker.IsOnlyOne() || !locker.Begin()) return;
                if (locker.IsCanceled()) return;
                try {
                    do {
                        if (v.empty()) break;
                        
                        std::vector<uint32_t> vr;
                        for (AudioSessionItem* a : v) {
                            if (a != nullptr) {
                                if ((a->Item.App.Pid != 0U) && (!Utils::is_pid_running(a->Item.App.Pid)))
                                    vr.push_back(a->Item.App.Pid);
                            }
                        }
                        if (vr.empty()) break;
                        if (locker.IsCanceled()) return;

                        removefromlist_([&](AudioSessionItem*& a) {
                            if (a == nullptr) return true;
                            for (uint32_t& i : vr)
                                if (i == a->Item.App.Pid) return true;
                            return false;
                            });
                    } while (0);
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }, std::ref(aslist__), std::ref(lock__)));
        }

        /* MIDI key Changed */

        void AudioSessionList::SetControlUnits(std::shared_ptr<MIDI::MidiDevice>& md) {

            if (md == nullptr) return;
            try {
                wb__.to_async(std::async(std::launch::async, [=]() {
                    try {
                        do {
                            ctrlunitlist__.clear();
                            for (auto& u : md->units) {
                                try {
                                    if ((u.target == MIDI::Mackie::Target::VOLUMEMIX) && (!u.appvolume.empty()))
                                        ctrlunitlist__.push_back(AudioSessionUnit(u));
                                } catch (...) {}
                            }

                            locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                            if (locker.IsCanceled()) break;

                            if (aslist__.empty()) break;

                            for (auto& a : aslist__) {
                                bool once{ false };
                                for (auto& u : ctrlunitlist__) {
                                    if (u.AppFound(a->Item.App.get<std::size_t>())) {
                                        a->Item.Id.add(u.GetData());
                                        if (IsSetAudioLevelOldValue && !once) {
                                            SetParamCb(u, a);
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