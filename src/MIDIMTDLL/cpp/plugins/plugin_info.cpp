/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL + MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace IO {

		PluginInfo::PluginInfo(uint32_t id, uint32_t did, GUID g, std::wstring p, std::wstring_view& n, std::wstring_view& d)
			: config_id_(id), dialog_id_(did), guid_(g), path_(p), name_(std::wstring(n.data())), desc_(std::wstring(d.data())) {
			build_version_();
		}
		PluginInfo::PluginInfo(uint32_t id, uint32_t did, GUID g, std::wstring p, std::wstring n, std::wstring d)
			: config_id_(id), dialog_id_(did), guid_(g), path_(p), name_(n), desc_(d) {
			build_version_();
		}

		void PluginInfo::build_version_() {
			try {
				if (ver_) return;

				std::wstring p(MAX_PATH, 0);
				if (!::GetModuleFileNameW(0, p.data(), MAX_PATH)) return;

				DWORD vh{ 0 }, vs;
				if (!(vs = GetFileVersionInfoSizeW(p.data(), &vh))) return;
				if (!vh) return;

				std::wstring s(vs, 0);
				#pragma warning( push )
				#pragma warning( disable : 6388 )
				if (!::GetFileVersionInfoW(p.data(), vh, vs, s.data())) return;
				#pragma warning( pop )

				uint32_t			psz = 0;
				VS_FIXEDFILEINFO* pfi = nullptr;
				if (!VerQueryValueW(s.c_str(), L"\\", (LPVOID*)&pfi, &psz)) return;

				ver_ = pfi->dwFileVersionLS;
			} catch (...) {}
		}

		const bool PluginInfo::operator==(GUID& g) {
			return Utils::is_guid_equals(guid_, g);
		}
		const bool PluginInfo::operator==(uint32_t id) {
			return config_id_ == id;
		}
		const bool PluginInfo::operator==(std::wstring& name) {
			return name_._Equal(name);
		}

		const std::wstring& PluginInfo::Path() {
			return path_;
		}
		const std::wstring& PluginInfo::Name() {
			return name_;
		}
		const std::wstring& PluginInfo::Desc() {
			return desc_;
		}
		const std::wstring  PluginInfo::File() {
			try {
				if (path_.empty()) return std::wstring();
				std::filesystem::path p(path_);
				return p.stem().wstring();
			} catch (...) {}
			return std::wstring();
		}
		const uint32_t PluginInfo::ConfigId() {
			return config_id_;
		}
		const uint32_t PluginInfo::DialogId() {
			return dialog_id_;
		}
		const GUID& PluginInfo::Guid() {
			return std::ref(guid_);
		}
		const std::wstring PluginInfo::VersionString() {
			try {
				if (!ver_) build_version_();
				return (log_string()
					<< (uint32_t)((ver_ >> 24) & 0xff) << L"."
					<< (uint32_t)((ver_ >> 16) & 0xff) << L"."
					<< (uint32_t)((ver_ >>  8) & 0xff) << L"."
					<< (uint32_t)((ver_ >>  0) & 0xff));

			} catch (...) {}
			return std::wstring();
		}
		const uint32_t PluginInfo::Version() {
			if (!ver_) build_version_();
			return ver_;
		}
	}
}
