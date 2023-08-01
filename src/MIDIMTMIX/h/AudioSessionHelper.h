/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include <audiopolicy.h>

namespace Common {
	namespace MIXER {

		class FLAG_EXPORT AudioSessionHelper
		{
		public:
			static std::wstring_view TypeItemsHelper(TypeItems);
			static std::wstring_view OnChangeTypeHelper(OnChangeType);
			static std::wstring_view DisconnectReasonHelper(AudioSessionDisconnectReason);
		};
	}
}
