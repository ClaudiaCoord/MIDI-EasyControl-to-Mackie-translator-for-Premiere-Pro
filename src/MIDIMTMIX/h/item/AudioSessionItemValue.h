/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIXER {

		typedef std::function<void(bool, OnChangeType, uint8_t, float, bool)> valuesOnValueChange;

		class FLAG_EXPORT AudioSessionItemValue
		{
		private:
			uint8_t volu{ 0U };
			float   volf{ 0.0f };
			bool    mute{ false };

			OnChangeType state = OnChangeType::OnChangeNone;
			valuesOnValueChange OnChangeCb = [](bool, OnChangeType, uint8_t, float, bool) {};

		public:

			AudioSessionItemValue() = default;

			void set_onchangecb(valuesOnValueChange, bool = false);
			void set_onchangecb_defsault();

			int32_t sprite_index();
			log_string to_string();

			void copy(MIDI::MidiUnit&);
			void copy(MIDI::MidiUnitValue&, MIDI::MidiUnitType);
			void copy(AudioSessionItemValue&, OnChangeType = OnChangeType::OnNoChange, bool = false);

			void setall(uint8_t, bool);
			void setall(uint8_t, bool, OnChangeType, bool);
			void setall(float, bool);
			void setall(float, bool, OnChangeType, bool);

			template<typename T>
			FLAG_EXPORT void set(T, bool = false);

			template<typename T>
			FLAG_EXPORT T get();

		};
		extern template FLAG_EXPORT void	AudioSessionItemValue::set<int32_t>(int32_t, bool);
		extern template FLAG_EXPORT void	AudioSessionItemValue::set<uint32_t>(uint32_t, bool);
		extern template FLAG_EXPORT void	AudioSessionItemValue::set<uint8_t>(uint8_t, bool);
		extern template FLAG_EXPORT void	AudioSessionItemValue::set<float>(float, bool);
		extern template FLAG_EXPORT void	AudioSessionItemValue::set<bool>(bool, bool);
		extern template FLAG_EXPORT uint8_t AudioSessionItemValue::get<uint8_t>();
		extern template FLAG_EXPORT float	AudioSessionItemValue::get<float>();
		extern template FLAG_EXPORT bool	AudioSessionItemValue::get<bool>();
		extern template FLAG_EXPORT OnChangeType AudioSessionItemValue::get<OnChangeType>();
	}
}

