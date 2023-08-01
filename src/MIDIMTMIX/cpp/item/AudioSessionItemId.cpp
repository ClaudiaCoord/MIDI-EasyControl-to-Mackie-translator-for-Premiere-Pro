/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIXER {

		typedef std::function<bool(ItemIdPair&)> compareId;

		static bool list_compare_(ItemIdPair& a, ItemIdPair& b) {
			return a.second == b.second;
		}
		static MIDI::MidiUnitType found_(ItemIdList& list, compareId f) {
			try {
				if (!list.empty()) {
					auto it = std::find_if(list.begin(), list.end(), f);
					return (it == list.end()) ? MIDI::MidiUnitType::UNITNONE : it->first;
				}
			} catch (...) {}
			return MIDI::MidiUnitType::UNITNONE;
		}
		void AudioSessionItemId::copy_(AudioSessionItemId& a, bool isclear) {
			try {
				if (isclear) keys.clear();

				ItemIdList& list = a.get();
				if (list.empty()) return;

				for (auto& p : list)
					keys.push_front(std::make_pair(p.first, p.second));
				keys.unique(list_compare_);
			} catch (...) {}
		}

		AudioSessionItemId::operator bool() {
			return !empty();
		}

		MIDI::MidiUnitType AudioSessionItemId::found(uint32_t k) {
			return found_(keys, [k](auto& x) {
				return (x.second == k);
			});
		}
		MIDI::MidiUnitType AudioSessionItemId::found(uint32_t k, MIDI::MidiUnitType t) {
			t = AudioSessionItemId::normalize_type(t);
			return found_(keys, [k, t](auto& x) {
				return (x.first == t) && (x.second == k);
			});
		}

		const bool AudioSessionItemId::add(MIDI::MidiUnit& m) {
			return add(m.type, m.GetMixerId());
		}
		const bool AudioSessionItemId::add(MIDI::MidiUnitType t, uint32_t k) {
			try {
				t = AudioSessionItemId::normalize_type(t);
				return add(std::make_pair(t, k));
			} catch (...) {}
			return false;
		}
		const bool AudioSessionItemId::add(ItemIdPair p) {
			try {
				if (!keys.empty()) {
					MIDI::MidiUnitType m = found(p.second, p.first);
					if (m != MIDI::MidiUnitType::UNITNONE) return false;
				}
				keys.push_front(p);
				return true;
			} catch (...) {}
			return false;
		}
		void AudioSessionItemId::remove(uint32_t k) {
			try {
				if (keys.empty()) return;
				keys.remove_if([=](auto& x) { return x.second == k; });
			} catch (...) {}
		}
		ItemIdList& AudioSessionItemId::get() {
			return std::ref(keys);
		}

		const bool AudioSessionItemId::empty() {
			return keys.empty();
		}
		void AudioSessionItemId::copy(AudioSessionItem& a) {
			copy_(a.Item.Id, true);
		}
		void AudioSessionItemId::copy(AudioSessionItemId& a) {
			copy_(a, true);
		}
		void AudioSessionItemId::copy(AudioSessionItemChange& a) {
			copy_(a.Item.Id, false);
		}
		log_string AudioSessionItemId::to_string() {
			log_string s;
			for (auto& a : keys)
				s << L"\t\t[type:" << (int32_t)a.first << L", key:" << a.second << L"],\n";
			return s;
		}
		MIDI::MidiUnitType AudioSessionItemId::normalize_type(MIDI::MidiUnitType m) {
			switch (m) {
				case MIDI::MidiUnitType::KNOB:
				case MIDI::MidiUnitType::FADER:
				case MIDI::MidiUnitType::SLIDER:
				case MIDI::MidiUnitType::KNOBINVERT:
				case MIDI::MidiUnitType::FADERINVERT:
				case MIDI::MidiUnitType::SLIDERINVERT: return MIDI::MidiUnitType::SLIDER;
				case MIDI::MidiUnitType::BTN:
				case MIDI::MidiUnitType::BTNTOGGLE: return MIDI::MidiUnitType::BTN;
				default: return MIDI::MidiUnitType::UNITNONE;
			}
		}
	}
}
