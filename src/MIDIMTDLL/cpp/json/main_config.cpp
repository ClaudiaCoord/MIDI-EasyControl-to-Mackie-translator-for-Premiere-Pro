/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace JSON {

		using namespace Common::MIDI;

		MMTConfig::MMTConfig() {}
		MMTConfig::~MMTConfig() { Clear(); }
		bool MMTConfig::empty() { return units.empty(); }
		void MMTConfig::Init() { Clear(); }
		void MMTConfig::Clear() {
			if (!units.empty()) units.clear();
		}
		void MMTConfig::Add(MidiUnit mu) {
			units.push_back(std::move(mu));
		}
		MMTConfig* MMTConfig::get() {
			return this;
		}
		std::wstring MMTConfig::Dump() {
			JSON::json_config jsc;
			return jsc.Dump(this);
		}
		void MMTConfig::copysettings_(MMTConfig* cnf) {
			if (!cnf) return;
			try {
				config = std::wstring(cnf->config);
				auto_start = cnf->auto_start;
				midiconf.Copy(cnf->midiconf);
				mqttconf.Copy(cnf->mqttconf);
				mmkeyconf.Copy(cnf->mmkeyconf);
				lightconf.Copy(cnf->lightconf);

				units.clear();
				for (auto a : cnf->units)
					units.push_back(a);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

	}
}
