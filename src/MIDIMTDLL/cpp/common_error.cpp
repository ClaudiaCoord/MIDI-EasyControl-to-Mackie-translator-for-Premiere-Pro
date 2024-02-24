/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include "common_error_string.h"

namespace Common {

	static constexpr std::size_t _DEFAULT_ERRMSG_Size = std::size(_DEFAULT_ERRMSGS);

	std::function<const std::wstring(uint32_t)> default_cb__() {
		return [=](uint32_t i)->std::wstring {
			return common_error_code::get_local_error(i);
		};
	}

	/* common_error_code */
	common_error_code::common_error_code() : cb_(default_cb__()) {
	}
	common_error_code& common_error_code::Get() {
		return std::ref(common_static::common_error_code_empty);
	}
	void common_error_code::set_default_error_cb() {
		cb_ = default_cb__();
	}
	void common_error_code::set_string_error_cb(std::function<std::wstring(uint32_t)> f) {
		cb_ = f;
	}
	std::wstring common_error_code::get_error(common_error_id i) {
		return cb_(static_cast<uint32_t>(i));
	}
	std::wstring common_error_code::get_error(uint32_t i) {
		return cb_(i);
	}
	const uint32_t common_error_code::get_size() {
		return _DEFAULT_ERRMSG_Size;
	}

	std::wstring common_error_code::get_local_error(uint32_t i) {
		try {
			if (i >= _DEFAULT_ERRMSG_Size)
				return _DEFAULT_ERRMSGS[(int)common_error_id::err_UNDEFINED].data();
			return _DEFAULT_ERRMSGS[i].data();

		} catch (...) {}
		return std::wstring();
	}

	/* common_error */
	void common_error::common_error_copy_(const common_error& e) {
		try {
			if (!e.method().empty()) {
				const std::wstring mt = e.method();
				method_ = std::wstring(mt.begin(), mt.end());
			}
			if (!e.message().empty()) {
				const std::wstring msg = e.message();
				msg_ = std::wstring(msg.begin(), msg.end());
			}
			line_ = e.line();
			code_ = e.code();
		} catch (...) {}
	}

	common_error& common_error::operator= (const common_error& e) noexcept {
		common_error_copy_(e);
		return *this;
	}
	common_error::common_error() noexcept
		: line_(-1), method_({}), msg_({}), code_(0) {
	}
	common_error::common_error(const uint32_t u) noexcept
		: line_(-1), method_({}), msg_({}), code_(u) {
	}
	common_error::common_error(const common_error& e) noexcept {
		common_error_copy_(e);
	}
	common_error::common_error(const uint32_t u, const std::wstring m, const int32_t l) noexcept
		: line_(l), method_(m), msg_({}), code_(u) {
	}
	common_error::common_error(const common_error_id i, const std::wstring m, const int32_t l) noexcept
		: line_(l), method_(m), msg_({}), code_(static_cast<uint32_t>(i)) {
	}
	common_error::common_error(const std::wstring e, const std::wstring m, const int32_t l) noexcept
		: line_(l), method_(m), code_(0), msg_(e) {
	}
	const std::wstring common_error::what() const {
		try {
			if (!msg_.empty())
				return msg_;

			log_string ls{};
			if (!method_.empty()) {
				if (line_ > 0)
					ls.to_log_method(method_.c_str(), line_);
				else
					ls.to_log_method(method_.c_str());
			}
			if (code_ == 0) ls << L"no errors";
			else ls << common_error_code::Get().get_error(code_);

			return ls.str();
		} catch (...) {}
		return std::wstring();
	}
	const std::wstring common_error::method() const noexcept {
		return method_;
	}
	const std::wstring common_error::message() const noexcept {
		return msg_;
	}
	const int common_error::line() const noexcept {
		return line_;
	}
	const uint32_t common_error::code() const noexcept {
		return code_;
	}
	const bool common_error::is_error() const {
		return code_ != 0;
	}

}
