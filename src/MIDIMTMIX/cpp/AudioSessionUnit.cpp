/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIXER {

		AudioSessionUnit::AudioSessionUnit(MIDI::MidiUnit& u, bool isinit)
			: unit_(u), type_(AudioSessionItemId::normalize_type(u.type)), id_((u.scene * 1000) + u.key) {
			if (isinit) {
				for (auto& a : u.apps)
					applist_.push_front(AudioSessionItemApp::hash(a));
			}
		}

		const MIDI::MidiUnitType AudioSessionUnit::GetType() const {
			return type_;
		}
		const uint32_t	AudioSessionUnit::GetId() const {
			return id_;
		}
		const uint8_t	AudioSessionUnit::GetVolume() {
			try {
				return unit_.value.value;
			} catch (...) {}
			return 0U;
		}
		const bool		AudioSessionUnit::GetMute() {
			try {
				return unit_.value.lvalue;
			} catch (...) {}
			return false;
		}
		ItemIdPair_t	AudioSessionUnit::GetData() {
			return std::make_pair(type_, id_);
		}
		const bool		AudioSessionUnit::AppFound(std::size_t h) {
			try {
				if (!applist_.empty()) {
					auto it = std::find_if(applist_.begin(), applist_.end(), [=](std::size_t& x) {
						#if defined(_DEBUG_AUDIOSESSION)
						if (x == h) {
							log_string ls{};
							ls << L"* AudioSessionUnit::AppFound + { hash=" << h << L", x=" << x << L", ? " << std::boolalpha << (bool)(x == h) << L" }\n";
							::OutputDebugStringW(ls.str().c_str());
						}
						#endif
						return x == h;
					});
					return (it != applist_.end());
				}
			} catch (...) {}
			return false;
		}

		void			AudioSessionUnit::UpdateVolume(uint8_t v, bool m) {
			try {
				unit_.value.value = v;
				unit_.value.lvalue = m;
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}

		log_string AudioSessionUnit::to_string() {
			try {
				log_string s{};
				s << L"{ id:" << GetId() << L", volume:" << (uint32_t)GetVolume() << L", mute:" << std::boolalpha << GetMute();
				s << L"\n\t[app:";
				for (auto& a : applist_)
					s << a << L", ";
				s << L"]}\n";
				return s;
			} catch (...) {}
			return log_string();
		}
	}
}
