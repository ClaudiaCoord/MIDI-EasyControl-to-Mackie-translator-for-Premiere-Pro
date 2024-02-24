/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {

    class FLAG_EXPORT filewatch_event {
    public:
        enum Actions : uint16_t {
            NONE = 0,
            Error,
            FileOldName,
            FileRename,
            FileChange,
            FileCreate,
            FileDeleted,
            Other
        };
        static std::wstring ActionsString(const Actions&);

        Actions action{ Actions::NONE };
        uint64_t time{ 0U };
        std::wstring name{};

        filewatch_event();
        filewatch_event(Actions, const std::wstring&);
        ~filewatch_event() = default;

        std::filesystem::path path() const;
        const bool exists() const;
    };

    typedef std::function<void(const filewatch_event&)> filewatch_event_t;

    class FLAG_EXPORT common_filewatch {
    private:
        handle_ptr<HANDLE, default_handleio_deleter> handle_{};
        common_event<filewatch_event_t> event_watch_{};
        std::vector<uint8_t> buffer_{};
        std::jthread task_{};

        void loop_(std::stop_token&);
        void event_cb_(DWORD, DWORD);

    public:

        common_filewatch();
        ~common_filewatch();

        const bool start(const std::filesystem::path&);
        const bool start(const std::wstring&);
        void stop();

        void event_add(filewatch_event_t, const uint32_t);
        void event_remove(const uint32_t);
        void event_clear();

        const bool isrun() const;
    };
}