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

        #define TUPLE(A,B) std::tuple<ListState, AudioSessionItem*>(A, B)
        static uint32_t CPID;

        /* template */

        template<typename T1>
        AudioSessionItem* getfromlist_(std::vector<AudioSessionItem*>& v, const T1& val, std::function<bool(const T1&, AudioSessionItem*&)> cmpcb) {
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

        void on_change_(AudioSessionItem* item, OnChangeType type) {
            try {
                AudioSessionMixer::Get().event_send(item->GetSessionItemChange(type));
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }

        /* private */

        std::tuple<ListState, AudioSessionItem*> AudioSessionList::checkfromlist_(const uint32_t& pid, std::wstring& name) {
            try {
                AudioSessionItem* asi = getfromlist_<std::wstring>(onlinelist__, name, [&](const std::wstring& val, AudioSessionItem* a) {
                    if (a == nullptr) return false;
                    return val._Equal(a->Name);
                    });
                if (asi == nullptr) {
                    asi = getfromlist_<uint32_t>(onlinelist__, pid, [&](const uint32_t& val, AudioSessionItem* a) {
                        if (a == nullptr) return false;
                        return val == a->Pid;
                        });
                    if (asi != nullptr)
                        return TUPLE(Common::MIXER::ListState::StatePid, asi);
                }
                else {
                    if (asi->Pid == pid) return TUPLE(ListState::StateFound, asi);
                    return TUPLE(Common::MIXER::ListState::StateTitle, asi);
                }
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return TUPLE(Common::MIXER::ListState::StateNone, nullptr);
        }
        AudioSessionItem* AudioSessionList::getfromlistbyguid_(LPCGUID g) {
            return getfromlist_<GUID>(onlinelist__, static_cast<GUID>(*g), [&](const GUID& val, AudioSessionItem*& a) {
                if (a == nullptr) return false;
                return a->Guid == val;
                });
        }
        AudioSessionItem* AudioSessionList::checklistbyitem_(AudioSessionItem* item) {
            auto it = std::find_if(onlinelist__.begin(), onlinelist__.end(), [item](AudioSessionItem*& a) {
                if (a == nullptr) return false;
                return item->Pid == a->Pid || item->Guid == a->Guid;
                });
            if (it == onlinelist__.end()) return nullptr;
            return static_cast<AudioSessionItem*>(*it);
        }

        /* class private */

        void AudioSessionList::clearlistbydup_(AudioSessionItem* item) {
            if (IsDuplicateRemoved && !item->Name.empty()) {
                removefromlist_([&](AudioSessionItem*& a) {
                    if (a == nullptr) return true;
                    return a->Name._Equal(item->Name) || a->Pid == item->Pid || a->Guid == item->Guid;
                    });
            } else {
                removefromlist_([&](AudioSessionItem*& a) {
                    if (a == nullptr) return true;
                    return a->Pid == item->Pid || a->Guid == item->Guid;
                    });
            }
        }
        void AudioSessionList::addtolist_(AudioSessionItem* item) {
            try {
                do {
                    if (item == nullptr) return;
                    if (item->IsEmpty()) {
                        delete item;
                        return;
                    }
                    if (item->Pid == CPID) return;
                    if (onlinelist__.empty()) break;

                    AudioSessionItem* asi = checklistbyitem_(item);

                    if (asi != nullptr) {
                        if (IsDuplicateRemoved) {
                            item->MidiId.Copy(*asi);
                            on_change_(item, OnChangeType::OnChangeUpdateData);
                            clearlistbydup_(item);
                        } else {
                            if (item->Name._Equal(asi->Name)) {
                                asi->Copy(*item);
                                on_change_(asi, OnChangeType::OnChangeUpdateData);
                                delete item;
                                return;
                            } else {
                                item->MidiId.Copy(*asi);
                                clearlistbydup_(item);
                            }
                        }
                    }
                } while (0);

                if (!ctrlunitlist__.empty()) {
                    try {
                        for (auto& u : ctrlunitlist__) {
                            for (auto& app : u.appvolume) {
                                if (item->Name._Equal(app)) {
                                    item->MidiId.Set(u);
                                    if (IsSetAudioLevelOldValue)
                                        SetParamCb(u, item);
                                }
                            }
                        }
                    } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }

                item->RegistrySession(true, this);
                onlinelist__.push_back(item);
                on_change_(item, OnChangeType::OnChangeNew);

            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }
        void AudioSessionList::removefromlist_(std::function<bool(AudioSessionItem*&)> cmpcb) {
            try {
                if (onlinelist__.empty()) return;
                auto it = std::remove_if(onlinelist__.begin(), onlinelist__.end(), [=](AudioSessionItem*& a) {
                    bool b = false;
                    try {
                        if (a == nullptr) return true;
                        if ((b = cmpcb(a))) {
                            on_change_(a, OnChangeType::OnChangeRemove);
                            a->RegistrySession(false);
                            delete a;
                        }
                    } catch (...) {}
                    return b;
                    });
                if (it != onlinelist__.end())
                    onlinelist__.erase(it, onlinelist__.end());
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }

        /* class public */

        AudioSessionList::AudioSessionList(
            uint32_t cpid,
            bool oldval,
            std::function<void(Common::MIDI::MixerUnit&, AudioSessionItem*&)> vcb)
            : IsSetAudioLevelOldValue(oldval), SetParamCb(vcb), wb__(Common::worker_background::Get()) {
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
            return onlinelist__.size() > 0;
        }

        void AudioSessionList::DuplicateRemoved(bool b) {
            IsDuplicateRemoved = b;
        }
        std::vector<AudioSessionItem*>& AudioSessionList::GetSessionList() {
            return std::ref(onlinelist__);
        }

        size_t AudioSessionList::Count() { return onlinelist__.size(); }
        bool AudioSessionList::IsEmpty() { return onlinelist__.empty(); }
        void AudioSessionList::Clear() {
            locker_auto locker(lock__, locker_auto::LockType::TypeLock);
            if (onlinelist__.empty()) return;
            try {
                for (AudioSessionItem* a : onlinelist__) {
                    if (a != nullptr) {
                        a->RegistrySession(false);
                        delete a;
                    }
                }
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            onlinelist__.clear();
        }
        void AudioSessionList::Add(AudioSessionItem* item) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLock);
                addtolist_(item);
                }));
        }
        void AudioSessionList::Remove(uint32_t& pid) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLock);
                try {
                    if (locker.IsCanceled()) return;
                    removefromlist_([&](AudioSessionItem*& a) {
                        if (a == nullptr) return true;
                        return a->Pid == pid;
                        });
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::Remove(GUID& guid) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLock);
                try {
                    if (locker.IsCanceled()) return;
                    removefromlist_([&](AudioSessionItem*& a) {
                        if (a == nullptr) return true;
                        return a->Guid == guid;
                        });
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::Remove(std::wstring& name) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                try {
                    if (locker.IsCanceled()) return;
                    removefromlist_([&](AudioSessionItem*& a) {
                        if (a == nullptr) return true;
                        return name._Equal(a->Name);
                        });
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::Remove(TypeItems& t) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                try {
                    if (locker.IsCanceled()) return;
                    removefromlist_([&](AudioSessionItem*& a) {
                        if (a == nullptr) return true;
                        return a->Typeitem == t;
                        });
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }

        AudioSessionItem* AudioSessionList::Get(uint32_t& pid) {
            if (lock__->IsLock()) return nullptr;
            try {
                return getfromlist_<uint32_t>(onlinelist__, pid, [&](const uint32_t& val, AudioSessionItem*& a) {
                    if (a == nullptr) return false;
                    return a->Pid == val;
                    });
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return nullptr;
        }
        AudioSessionItem* AudioSessionList::Get(GUID& guid) {
            if (lock__->IsLock()) return nullptr;
            try {
                return getfromlist_<GUID>(onlinelist__, guid, [&](const GUID& val, AudioSessionItem*& a) {
                    if (a == nullptr) return false;
                    return a->Guid == val;
                    });
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return nullptr;
        }
        AudioSessionItem* AudioSessionList::Get(std::wstring& name) {
            if (lock__->IsLock()) return nullptr;
            try {
                return getfromlist_<std::wstring>(onlinelist__, name, [&](const std::wstring& val, AudioSessionItem*& a) {
                    if (a == nullptr) return false;
                    return val._Equal(a->Name);
                    });
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return nullptr;
        }
        AudioSessionItem* AudioSessionList::GetByMidiId(uint32_t midiid) {
            if (lock__->IsLock()) return nullptr;
            try {
                return getfromlist_<uint32_t>(onlinelist__, midiid, [&](const uint32_t& val, AudioSessionItem*& a) {
                    if (a == nullptr) return false;
                    return a->MidiId.Get(val) != AudioAction::AUDIO_NONE;
                    });
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return nullptr;
        }
        ListState AudioSessionList::Found(uint32_t& pid, std::wstring& name) {
            if (lock__->IsLock()) return ListState::StateBusy;
            try {
                std::tuple<ListState, AudioSessionItem*> t = checkfromlist_(pid, name);
                return std::get<0>(t);
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return ListState::StateError;
        }

        /* Event Changed */

        void AudioSessionList::DisplayNameChange(LPCGUID g, LPCWSTR s) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                try {
                    AudioSessionItem* item = getfromlistbyguid_(g);
                    if (item != nullptr) {
                        item->Name = Common::Utils::to_string((LPWSTR)s);
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
                    if (item != nullptr) item->Icon = Common::Utils::to_string((LPWSTR)s);
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::GuidChange(LPCGUID g, LPCGUID gn) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                try {
                    AudioSessionItem* item = getfromlistbyguid_(g);
                    if (item != nullptr) item->Guid = static_cast<GUID>(*gn);
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }));
        }
        void AudioSessionList::VolumeChange(LPCGUID g, float v, bool m) {
            wb__.to_async(std::async(std::launch::async, [=]() {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                try {
                    AudioSessionItem* item = getfromlistbyguid_(g);
                    if (item != nullptr) {
                        bool mute = item->Volume.GetMute();
                        item->Volume.Set(v, m);
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
                        item->Volume.SetVolume(v);
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
                                if ((a->Pid != 0U) && (!Common::Utils::is_pid_running(a->Pid)))
                                    vr.push_back(a->Pid);
                            }
                        }
                        if (vr.empty()) break;
                        if (locker.IsCanceled()) return;

                        removefromlist_([&](AudioSessionItem*& a) {
                            if (a == nullptr) return true;
                            for (uint32_t& i : vr)
                                if (i == a->Pid) return true;
                            return false;
                            });
                    } while (0);
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                }, std::ref(onlinelist__), std::ref(lock__)));
        }

        /* MIDI key Changed */

        void AudioSessionList::SetControlUnits(std::shared_ptr<Common::MIDI::MidiDevice>& md) {

            if (md == nullptr) return;
            try {
                wb__.to_async(std::async(std::launch::async, [=]() {
                    try {
                        std::vector<std::tuple<AudioSessionItem*, Common::MIDI::MixerUnit>> valuelist{};

                        do {
                            ctrlunitlist__.clear();
                            for (auto& u : md->units) {
                                if ((u.target == Common::MIDI::Mackie::Target::VOLUMEMIX) && (!u.appvolume.empty()))
                                    ctrlunitlist__.push_back(std::move(u.GetMixerUnit()));
                            }

                            locker_auto locker(lock__, locker_auto::LockType::TypeLockWait);
                            if (locker.IsCanceled()) break;

                            if (onlinelist__.empty()) break;
                            for (auto& a : onlinelist__) {
                                a->MidiId.Clear();
                                for (auto& u : ctrlunitlist__) {
                                    for (auto& app : u.appvolume) {
                                        if (a->Name._Equal(app)) {
                                            a->MidiId.Set(u);
                                            if (IsSetAudioLevelOldValue)
                                                valuelist.push_back(
                                                    std::tuple<AudioSessionItem*, Common::MIDI::MixerUnit>(std::ref(a), std::ref(u))
                                            );
                                        }
                                    }
                                }
                            }
                        } while (0);

                        if (!IsSetAudioLevelOldValue || lock__->IsCanceled()) return;

                        if (!valuelist.empty()) {
                            for (auto& t : valuelist)
                                SetParamCb(std::get<1>(t), std::get<0>(t));
                        }
                    } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
                    })
                );
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }
    }
}