/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol.
	+ Art-Net protocol.
	(c) CC 2023, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace LIGHT {

        const bool LightsConfig::empty() {
            return (dmxconf.empty() && artnetconf.empty());
        }
		void LightsConfig::Copy(LightsConfig& lc) {
			ispool = lc.ispool;
			dmxconf.Copy(lc.dmxconf);
			artnetconf.Copy(lc.artnetconf);
		}
        log_string LightsConfig::dump() {
            log_string ls{};
            ls << L"\n\tLights Config:";
			ls << L"\n\tDMX: " << dmxconf.dump().str();
			ls << L"\n\tARTNET: " << artnetconf.dump().str();
            return ls;
        }
	}
}