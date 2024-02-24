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

namespace Common {

	using namespace std::string_view_literals;

	class NamesFileWatch {
	public:
		#pragma region FileWatch Names
		static constexpr std::wstring_view EVERROR = L"Error"sv;
		static constexpr std::wstring_view EVFILEOLDNAME = L"FileOldName"sv;
		static constexpr std::wstring_view EVFILERENAME = L"FileRename"sv;
		static constexpr std::wstring_view EVFILECHANGE = L"FileChange"sv;
		static constexpr std::wstring_view EVFILECREATE = L"FileCreate"sv;
		static constexpr std::wstring_view EVFILEDELETED = L"FileDeleted"sv;
		static constexpr std::wstring_view EVOTHER = L"Other"sv;
		static constexpr std::wstring_view EVNONE = L"None"sv;
		#pragma endregion
	};

	#pragma region filewatch_event
	filewatch_event::filewatch_event() {
	}
	filewatch_event::filewatch_event(Actions a, const std::wstring& s)
		: action(a), name(s) {
	}
	std::filesystem::path filewatch_event::path() const {
		return std::filesystem::path(name);
	}
	const bool filewatch_event::exists() const {
		return std::filesystem::exists(name);
	}

	std::wstring filewatch_event::ActionsString(const Actions& t) {
		switch (t) {
			using enum Actions;
			case Error: return NamesFileWatch::EVERROR.data();
			case FileOldName: return NamesFileWatch::EVFILEOLDNAME.data();
			case FileRename: return NamesFileWatch::EVFILERENAME.data();
			case FileChange: return NamesFileWatch::EVFILECHANGE.data();
			case FileCreate: return NamesFileWatch::EVFILECREATE.data();
			case FileDeleted: return NamesFileWatch::EVFILEDELETED.data();
			case Other: return NamesFileWatch::EVOTHER.data();
			default: return NamesFileWatch::EVNONE.data();
		}
	}
	#pragma endregion

	#pragma region common_filewatch
	common_filewatch::common_filewatch() {
		buffer_ = std::vector<uint8_t>(64 * 1024);
	}
	common_filewatch::~common_filewatch() {
		stop();
	}

	const bool common_filewatch::start(const std::wstring& s) {
		return start(std::filesystem::path(s));
	}
	const bool common_filewatch::start(const std::filesystem::path& p) {
		try {
			if (p.empty() || !std::filesystem::exists(p) || !std::filesystem::is_directory(p) || task_.joinable()) return false;
			HANDLE h = ::CreateFileW(
			  p.wstring().c_str(),
			  FILE_LIST_DIRECTORY,
			  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			  nullptr,
			  OPEN_EXISTING,
			  FILE_FLAG_BACKUP_SEMANTICS,
			  nullptr
			);

			if (!h || (h == INVALID_HANDLE_VALUE)) {
				to_log::Get() << log_string().to_log_format(__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_FILEWATCH_OPEN),
					p.wstring());
			} else {
				handle_.reset(h);
				task_ = std::jthread(
					[=](std::stop_token token) {
						try {
							while (!token.stop_requested()) {
								if (!handle_) break;
								loop_(std::ref(token));
							}
						} catch (...) {
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						}
					}
				);
				return task_.joinable();
			}

		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
		return false;
	}
	void common_filewatch::stop() {
		try {
			if (!handle_ && !task_.joinable()) return;
			if (task_.joinable()) {
				std::stop_source ssource = task_.get_stop_source();
				if (ssource.stop_possible())
					ssource.request_stop();

				if (auto h = task_.native_handle(); h)
					#pragma warning( push )
					#pragma warning( disable : 6258 )
					::TerminateThread(h, 1);
					#pragma warning( pop )
				task_.join();
			}
			handle_.reset();
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}
	void common_filewatch::loop_(std::stop_token& token) {
		if (token.stop_requested() || !handle_) return;
		try {

			DWORD size{ 0U };
			if (!::ReadDirectoryChangesExW(
				handle_,
				buffer_.data(),
				static_cast<DWORD>(buffer_.capacity()),
				true,
				FILE_NOTIFY_CHANGE_CREATION
				| FILE_NOTIFY_CHANGE_LAST_WRITE
				| FILE_NOTIFY_CHANGE_SIZE
				| FILE_NOTIFY_CHANGE_FILE_NAME
				| FILE_NOTIFY_CHANGE_DIR_NAME,
				&size,
				nullptr,
				nullptr,
				READ_DIRECTORY_NOTIFY_INFORMATION_CLASS::ReadDirectoryNotifyExtendedInformation)) {
				event_cb_(::GetLastError(), size);
			}
			event_cb_(0U, size);

		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}

	void common_filewatch::event_cb_(DWORD err, DWORD size) {
		try {
			switch (err) {
				case ERROR_SUCCESS: break;
				case ERROR_NOTIFY_ENUM_DIR: {
					filewatch_event evt{ filewatch_event(
						filewatch_event::Actions::Error,
						L"error/watch/enumerate/directory"
					) };
					event_watch_.send(std::move(evt));
					return;
				}
				case ERROR_IO_PENDING: {
					filewatch_event evt{ filewatch_event(
						filewatch_event::Actions::Error,
						L"error/watch/read/pending"
					) };
					event_watch_.send(std::move(evt));
					return;
				}
				case ERROR_MORE_DATA: {
					filewatch_event evt{ filewatch_event(
						filewatch_event::Actions::Error,
						L"error/watch/read/more/data"
					) };
					event_watch_.send(std::move(evt));
					return;
				}
				case ERROR_NOACCESS: {
					handle_.reset();
					to_log::Get() << log_string().to_log_string(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_FILEWATCH_NOACCESS));
					return;
				}
				case ERROR_INVALID_PARAMETER: {
					handle_.reset();
					to_log::Get() << log_string().to_log_string(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_FILEWATCH_PARAM));
					return;
				}
				default: {
					handle_.reset();
					to_log::Get() << log_string().to_log_string(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_FILEWATCH_SHUTDOWN));
					return;
				}
			}
			if (!size) {
				filewatch_event evt{ filewatch_event(
					filewatch_event::Actions::Error,
					L"error/watch/read/size"
				) };
				event_watch_.send(std::move(evt));
				return;
			}

			uint8_t* buf = buffer_.data();
			if (!buf) return;

			while (true) {
				filewatch_event evt{};
				PFILE_NOTIFY_EXTENDED_INFORMATION i = (PFILE_NOTIFY_EXTENDED_INFORMATION)buf;
				evt.name = std::wstring(i->FileName, i->FileNameLength / sizeof(wchar_t));
				evt.time = static_cast<uint64_t>(i->LastModificationTime.QuadPart);

				switch (i->Action) {
					case FILE_ACTION_RENAMED_OLD_NAME: evt.action = filewatch_event::Actions::FileOldName; break;
					case FILE_ACTION_RENAMED_NEW_NAME: evt.action = filewatch_event::Actions::FileRename; break;
					case FILE_ACTION_ADDED:			   evt.action = filewatch_event::Actions::FileCreate; break;
					case FILE_ACTION_REMOVED:		   evt.action = filewatch_event::Actions::FileDeleted; break;
					case FILE_ACTION_MODIFIED:		   evt.action = filewatch_event::Actions::FileChange; break;
					default:						   evt.action = filewatch_event::Actions::Other; break;
				};

				event_watch_.send(std::move(evt));

				if (i->NextEntryOffset == 0) break;
				buf += i->NextEntryOffset;
			}
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}

	void common_filewatch::event_add(filewatch_event_t f, const uint32_t id) {
		event_watch_.add(f, id);
	}
	void common_filewatch::event_remove(const uint32_t id) {
		event_watch_.remove(id);
	}
	void common_filewatch::event_clear() {
		event_watch_.clear();
	}

	const bool common_filewatch::isrun() const {
		return handle_;
	}
	#pragma endregion
}