/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Viewer/Editor interface.
	(c) CC 2023-2024, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace UI {

		#define HLP_BITS_(A,B) if ((notify->modificationType & A) != 0) ss << "+" << B << ", ";
		#define HLP_BITS(A) HLP_BITS_(A,#A)

		class ScintillaHelper {
		public:
			static std::string parse_event(SCNotification* notify) {
				std::stringstream ss{};
				ss << "* ";
				if (notify->modificationType == SC_MOD_NONE)
					ss << "+SC_MOD_NONE, ";

				#include "ScintillaHelperEvent.h"

				if (!ss.str().empty()) {
					ss << "\n";
					ss << "* line:" << notify->line;
					ss << ", position:" << notify->position;
					ss << ", length:" << notify->length;
					ss << ", lines add:" << notify->linesAdded;
					ss << "\n";
					return ss.str();
				}
				return std::string();
			}
		};
	}
}


