/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace SCRIPT {

		const bool VmScriptDebug::is_debug() const {
			bool b = ::IsDebuggerPresent();
			if (!b) {
				BOOL pb{};
				if (::CheckRemoteDebuggerPresent(::GetCurrentProcess(), &pb))
					b = static_cast<bool>(pb);
			}
			return b;
		}
	}
}