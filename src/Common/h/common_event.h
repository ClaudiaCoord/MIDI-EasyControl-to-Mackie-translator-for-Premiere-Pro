/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {

    template <typename T1>
    class common_event {
    private:
        std::forward_list<std::pair<uint32_t, T1>> list__;
        std::shared_ptr<locker_awaiter> lock__;
    public:

        common_event() = default;
        ~common_event() = default;

        template<typename... T3>
        void send(T3&&... args) {
            if (empty()) return;
            try {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockOnlyOne);
                for (auto& a : list__) a.second(args...);
            } catch (...) {}
        }

        template<typename... T3>
        void send_async(T3&&... args) {
            if (empty()) return;
            worker_background::Get().to_async(
                std::async(std::launch::async, [=]() { send(args...); }));
        }

        template<typename T2>
        void sends(std::initializer_list<T2> il) {
            if (empty()) return;
            try {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockOnlyOne);
                for (auto& a : list__)
                    for (auto& p : il) a.second(p);
            } catch (...) {}
        }

        template<typename T2>
        void sends_async(std::initializer_list<T2> il) {
            if (empty()) return;
            worker_background::Get().to_async(
                std::async(std::launch::async, [=](std::initializer_list<T2> il__) { sends(il__); }));
        }

        std::forward_list<std::pair<uint32_t, T1>>& get() {
            return std::ref(list__);
        }
        std::vector<uint32_t> list() {
            std::vector<uint32_t> v;
            try {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockOnlyOne);
                for (auto& a : list__) v.push_back(a.first);
            } catch (...) {}
            return v;
        }
        void clear() {
            list__.clear();
        }
        const bool empty() {
            return list__.empty();
        }
        uint32_t add(T1 fn) {
            try {
                uint32_t id = Utils::random_hash(&fn);
                add(fn, id);
                return id;
            } catch (...) {}
            return 0U;
        }
        void add(T1 fn, uint32_t id) {
            try {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockOnlyOne);
                if (!Utils::random_isvalid(id)) return;

                auto it = std::find_if(list__.begin(), list__.end(), [&](std::pair<uint32_t, T1>& a) -> bool { return a.first == id; });
                if (it != list__.end()) return;
                list__.push_front(std::pair<uint32_t, T1>(id, fn));
            } catch (...) {}
        }
        void remove(T1 fn) {
            try {
                uint32_t id = Utils::random_hash(&fn);
                remove(id);
            } catch (...) {}
        }
        void remove(uint32_t id) {
            try {
                locker_auto locker(lock__, locker_auto::LockType::TypeLockOnlyOne);
                if (!Utils::random_isvalid(id)) return;
                list__.remove_if([&](std::pair<uint32_t, T1>& a) -> bool { return a.first == id; });
            } catch (...) {}
        }
    };
}