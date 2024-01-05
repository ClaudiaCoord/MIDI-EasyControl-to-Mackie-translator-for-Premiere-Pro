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
		
		template<typename T>
		OnChangeType set_(AudioSessionItemValue* clz, T val, bool b) {
			OnChangeType t1, t2;
			clz->set(val);
			t1 = clz->get<OnChangeType>();
			clz->set(b);
			t2 = clz->get<OnChangeType>();

			if ((t1 != OnChangeType::OnNoChange) && (t2 != OnChangeType::OnNoChange))
				return OnChangeType::OnChangeUpdateAllValues;
			else if (t1 != OnChangeType::OnNoChange)
				return t1;
			else if (t2 != OnChangeType::OnNoChange)
				return t2;
			return OnChangeType::OnNoChange;
		}

		void AudioSessionItemValue::setall(uint8_t val, bool b) {
			state = set_(this, val, b);
		}
		void AudioSessionItemValue::setall(float val, bool b) {
			state = set_(this, val, b);
		}
		void AudioSessionItemValue::setall(uint8_t val, bool b, OnChangeType t, bool isup) {
			set(val, isup); set(b, isup); state = t;
		}
		void AudioSessionItemValue::setall(float val, bool b, OnChangeType t, bool isup) {
			set(val, isup); set(b, isup); state = t;
		}

		void AudioSessionItemValue::set_onchangecb(valuesOnValueChange cb, bool isup) {
			if (cb == nullptr) return;
			OnChangeCb = cb;
			OnChangeCb(isup, OnChangeType::OnChangeUpdateAllValues, volu, volf, mute);
		}
		void AudioSessionItemValue::set_onchangecb_defsault() {
			OnChangeCb = [](bool, OnChangeType, uint8_t, float, bool) {};
		}

		int32_t AudioSessionItemValue::sprite_index() {
			int32_t idx = static_cast<int32_t>(std::abs(30 - (volf * 30)));
			return (idx < 0) ? 0 : ((idx > 30) ? 30 : idx);
		}
		log_string AudioSessionItemValue::to_string() {
			log_string s{};
			s << L"[volume:" << (uint32_t)volu << L"/" << (double)volf << L", mute:" << std::boolalpha << mute << L"]";
			return s;
		}

		void AudioSessionItemValue::copy(AudioSessionItemValue& val, OnChangeType type, bool isup) {
			volu = val.volu;
			volf = val.volf;
			mute = val.mute;
			state = OnChangeType::OnNoChange;
			OnChangeCb(isup, type, volu, volf, mute);
		}
		void AudioSessionItemValue::copy(MIDI::MidiUnit& val) {
			if (val.empty()) return;
			copy(val.value, val.type);
		}
		void AudioSessionItemValue::copy(MIDI::MidiUnitValue& val, MIDI::MidiUnitType t) {
			switch (AudioSessionItemId::normalize_type(t)) {
				case MIDI::MidiUnitType::BTN: set(val.lvalue); break;
				case MIDI::MidiUnitType::SLIDER: set(val.value); break;
				default: return;
			}
		}

		template<typename T>
		T AudioSessionItemValue::get() {
				 if constexpr (std::is_same_v<uint8_t, T>) return volu;
			else if constexpr (std::is_same_v<float, T>) return volf;
			else if constexpr (std::is_same_v<bool, T>) return mute;
			else if constexpr (std::is_same_v<OnChangeType, T>) return state;
		}
		template uint8_t AudioSessionItemValue::get<uint8_t>();
		template float AudioSessionItemValue::get<float>();
		template bool AudioSessionItemValue::get<bool>();
		template OnChangeType AudioSessionItemValue::get<OnChangeType>();

		template<typename T>
		void AudioSessionItemValue::set(T val, bool isup) {
			if constexpr (std::is_same_v<uint8_t, T> || std::is_same_v<int32_t, T> || std::is_same_v<uint32_t, T>) {
				uint8_t u = (val > 127) ? 127U : val;
				if (u == volu) {
					state = OnChangeType::OnNoChange;
					return;
				}

				volu = u;
				volf = static_cast<float>(volu * KEY_DEVIDER);
				state = OnChangeType::OnChangeUpdateVolume;
				OnChangeCb(isup, state, volu, volf, false);
			}
			else if constexpr (std::is_same_v<float, T>) {
				float f = (val > 1.0f) ? 1.0f : (((val < 0.0f) || std::isnan(val)) ? 0.0f : val);
				if (f == volf) {
					state = OnChangeType::OnNoChange;
					return;
				}

				volf = f;
				uint32_t u = static_cast<uint32_t>(f * 128);
				volu = (u > 127) ? 127U : u;
				state = OnChangeType::OnChangeUpdateVolume;
				OnChangeCb(isup, state, volu, volf, false);
			}
			else if constexpr (std::is_same_v<bool, T>) {
				if (mute == val) {
					state = OnChangeType::OnNoChange;
					return;
				}

				mute = val;
				state = OnChangeType::OnChangeUpdateMute;
				OnChangeCb(isup, state, volu, volf, val);
			}
		}
		template void AudioSessionItemValue::set<int32_t>(int32_t, bool);
		template void AudioSessionItemValue::set<uint32_t>(uint32_t, bool);
		template void AudioSessionItemValue::set<uint8_t>(uint8_t, bool);
		template void AudioSessionItemValue::set<float>(float, bool);
		template void AudioSessionItemValue::set<bool>(bool, bool);
	}
}
