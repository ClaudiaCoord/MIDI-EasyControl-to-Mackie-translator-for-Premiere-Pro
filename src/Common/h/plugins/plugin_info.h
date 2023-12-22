/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023, MIT

	MIDIMMT DLL + MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace IO {

		class FLAG_EXPORT PluginInfo {
		private:
			std::wstring path_{};
			std::wstring name_{};
			std::wstring desc_{};
			uint32_t ver_{ 0U };
			uint32_t config_id_{ 0U };
			uint32_t dialog_id_{ 0U };
			GUID guid_{};

			void build_version_();

		public:

			const bool operator==(GUID& g);
			const bool operator==(uint32_t id);
			const bool operator==(std::wstring& name);

			PluginInfo(uint32_t, uint32_t, GUID, std::wstring, std::wstring_view&, std::wstring_view&);
			PluginInfo(uint32_t, uint32_t, GUID, std::wstring, std::wstring, std::wstring);
			~PluginInfo() = default;

			const std::wstring& Path();
			const std::wstring& Name();
			const std::wstring& Desc();
			const std::wstring  File();
			const std::wstring  VersionString();
			const uint32_t Version();
			const uint32_t ConfigId();
			const uint32_t DialogId();
			const GUID& Guid();
		};
	}
}

