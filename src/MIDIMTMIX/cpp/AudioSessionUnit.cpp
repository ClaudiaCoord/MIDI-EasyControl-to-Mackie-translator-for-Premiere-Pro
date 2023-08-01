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

		AudioSessionUnit::AudioSessionUnit(MIDI::MidiUnit& u, bool isinit)
			: unit__(u), type__(AudioSessionItemId::normalize_type(u.type)), id__((u.scene * 1000) + u.key) {
			if (isinit) {
				for (auto& a : u.appvolume)
					applist__.push_front(AudioSessionItemApp::hash(a));
			}
		}

		const MIDI::MidiUnitType AudioSessionUnit::GetType() const {
			return type__;
		}
		const uint32_t	AudioSessionUnit::GetId() const {
			return id__;
		}
		const uint8_t	AudioSessionUnit::GetVolume() {
			try {
				return unit__.value.value;
			} catch (...) {}
			return 0U;
		}
		const bool		AudioSessionUnit::GetMute() {
			try {
				return unit__.value.lvalue;
			} catch (...) {}
			return false;
		}
		ItemIdPair		AudioSessionUnit::GetData() {
			return std::make_pair(type__, id__);
		}
		const bool		AudioSessionUnit::AppFound(std::size_t h) {
			try {
				if (!applist__.empty()) {
					auto it = std::find_if(applist__.begin(), applist__.end(), [=](std::size_t& x) {
						#if defined(_DEBUG_AUDIOSESSION)
						if (x == h) {
							log_string ls{};
							ls << L"* AudioSessionUnit::AppFound + { hash=" << h << L", x=" << x << L", ? " << std::boolalpha << (bool)(x == h) << L" }\n";
							::OutputDebugStringW(ls.str().c_str());
						}
						#endif
						return x == h;
					});
					return (it != applist__.end());
				}
			} catch (...) {}
			return false;
		}

		void			AudioSessionUnit::UpdateVolume(uint8_t v, bool m) {
			try {
				unit__.value.value = v;
				unit__.value.lvalue = m;
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}

		log_string AudioSessionUnit::to_string() {
			try {
				log_string s{};
				s << L"{ id:" << GetId() << L", volume:" << (uint32_t)GetVolume() << L", mute:" << std::boolalpha << GetMute();
				s << L"\n\t[app:";
				for (auto& a : applist__)
					s << a << L", ";
				s << L"]}\n";
				return s;
			} catch (...) {}
			return log_string();
		}

	}
}
