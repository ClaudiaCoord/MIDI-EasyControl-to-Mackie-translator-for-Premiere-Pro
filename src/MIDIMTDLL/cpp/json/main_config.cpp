/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace JSON {

		using namespace Common::MIDI;

		MMTConfig::MMTConfig() {}
		MMTConfig::~MMTConfig() { clear(); }
		bool MMTConfig::empty() { return units.empty(); }
		void MMTConfig::init() { clear(); }
		void MMTConfig::clear() {
			if (!units.empty()) {
				std::unique_lock<std::shared_mutex> lock(mtx_);
				units.clear();
			}
		}
		void MMTConfig::add(MidiUnit mu) {
			units.push_back(std::move(mu));
		}
		MMTConfig* MMTConfig::get() {
			return this;
		}
		std::wstring MMTConfig::dump() {
			JSON::json_config jsc;
			std::unique_lock<std::shared_mutex> lock(mtx_);
			return jsc.Dump(this);
		}

		MIDI::MidiUnit& MMTConfig::find(const uint8_t s) {
			return find_(
				[=](const MIDI::MidiUnit& u) {
					return u.scene == s;
				}
			);
		}
		MIDI::MidiUnit& MMTConfig::find(const uint8_t s, const uint8_t k) {
			return find_(
				[=](const MIDI::MidiUnit& u) {
					return (u.scene == s) && (u.key == k);
				}
			);
		}
		MIDI::MidiUnit& MMTConfig::find(const uint8_t s, const uint8_t k, const MIDI::MidiUnitType t) {
			return find_(
				[=](const MIDI::MidiUnit& u) {
					return (u.scene == s) && (u.key == k) && (u.type == t);
				}
			);
		}
		MIDI::MidiUnit& MMTConfig::find(const MIDI::Mackie::Target t) {
			return find_(
				[=](const MIDI::MidiUnit& u) {
					return u.target == t;
				}
			);
		}

		void MMTConfig::copy_units_(MMTConfig* cnf) {
			if (!cnf) return;
			try {
				units.clear();
				for (auto a : cnf->units)
					units.push_back(a);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void MMTConfig::copy_settings_(MMTConfig* cnf) {
			if (!cnf) return;
			try {
				config = std::wstring(cnf->config);
				auto_start = cnf->auto_start;
				midiconf.copy(cnf->midiconf);
				mqttconf.copy(cnf->mqttconf);
				mmkeyconf.copy(cnf->mmkeyconf);
				lightconf.copy(cnf->lightconf);
				remoteconf.copy(cnf->remoteconf);
				gamepadconf.copy(cnf->gamepadconf);
				vmscript.copy(cnf->vmscript);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		MIDI::MidiUnit& MMTConfig::find_(std::function<bool(const MIDI::MidiUnit&)> f) {
			try {
				auto entry = std::find_if(
					units.begin(),
					units.end(),
					f
				);
				if (entry != units.end())
					return reinterpret_cast<MIDI::MidiUnit&>(*entry);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return std::ref(common_static::unit_empty);
		}

	}
}
