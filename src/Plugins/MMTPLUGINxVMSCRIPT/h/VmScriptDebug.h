/*
	MIDI-MT is a high level application driver for USB MIDI control surface.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace SCRIPT {

		using namespace std::string_view_literals;

		class VmScriptDebug {
		public:

			VmScriptDebug() = default;
			VmScriptDebug(const VmScriptDebug&) = default;
			~VmScriptDebug() = default;

			const bool is_debug() const;

			template <typename T>
			void puts(const T& s) {
				::OutputDebugStringW(
					(log_string() << s << L'\n').c_str()
				);
			}
		};
	}
}
