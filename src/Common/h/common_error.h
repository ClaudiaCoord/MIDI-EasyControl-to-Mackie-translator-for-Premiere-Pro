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

    #define throw_common_error(err) throw common_error(err, __FUNCTIONW__, __LINE__)
    #define make_common_error(err) common_error(err, __FUNCTIONW__, __LINE__)

    class FLAG_EXPORT common_error_code {
    private:
        std::function<const std::wstring(uint32_t)> cb__;
    public:

        common_error_code();
        void set_default_error_cb();
        void set_string_error_cb(std::function<std::wstring(uint32_t)>);
        std::wstring get_error(common_error_id);
        std::wstring get_error(uint32_t);

        static common_error_code& Get();
        static std::wstring get_local_error(uint32_t);
    };


    class FLAG_EXPORT common_error {
    private:
        int line__;
        uint32_t code__;
        std::wstring msg__;
        std::wstring method__;

        void common_error_copy_(const common_error&);

    public:
        common_error& operator= (const common_error&) noexcept;

        common_error() noexcept;
        ~common_error() = default;
        common_error(const uint32_t) noexcept;
        common_error(const common_error&) noexcept;
        common_error(const uint32_t, const std::wstring, const int32_t) noexcept;
        common_error(const common_error_id, const std::wstring, const int32_t) noexcept;
        common_error(const std::wstring, const std::wstring, const int32_t) noexcept;
        const bool is_error() const;
        const int  line() const noexcept;
        const std::wstring what() const;
        const std::wstring method() const noexcept;
        const std::wstring message() const noexcept;
        const uint32_t code() const noexcept;
    };
}
