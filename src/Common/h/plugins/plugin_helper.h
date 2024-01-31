/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL + MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace IO {

		class FLAG_EXPORT PluginHelper {
		public:
			static log_string GetCbType(const PluginCbType&);
			static std::wstring_view GetClassTypes(const PluginClassTypes&);
			static std::wstring_view GetTologTypes(const PluginClassTypes&);
		};
	}
}

