/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol.
	+ Art-Net protocol.
	(c) CC 2023-2024, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace SCRIPT {

		class FLAG_EXPORT VmScriptConfig {
		public:
			bool enable{ false };
			bool script_watch{ false };
			std::wstring script_directory{};
			std::vector<std::wstring> script_list{};

			VmScriptConfig();
			void copy(const VmScriptConfig&);
			size_t count();
			void clear();
			void add(const std::wstring&);

			const bool empty();
			log_string dump();
		};
	}
}

