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

namespace Common {
	namespace MIXER {

		class FLAG_EXPORT AudioSessionItemChangeData {
		public:
			uint32_t     Id       = 0U;
			AudioAction  Action   = AudioAction::AUDIO_NONE;

			void Copy(AudioSessionItemChangeData&);
		};

		class FLAG_EXPORT AudioSessionItemChange
		{
		private:
			bool iscallaudiocb__ = false;
			OnChangeType actiontype__ = OnChangeType::OnChangeNone;

			const OnChangeType onchangetypefilter_(OnChangeType);
			const bool onchangecbfilter_(OnChangeType);
			bool set_(AudioSessionItem*);
			bool set_(AudioSessionItemChange&);
			bool checkaction_(uint8_t, AudioAction);
			AudioAction getaction_(uint8_t);

		public:
			constexpr static uint8_t DataSize = 3;

			AudioSessionItemChangeData  Data[AudioSessionItemChange::DataSize]{};
			TypeItems    Typeitem = TypeItems::TypeNone;
			float        Volume = 0.0f;
			bool         Mute = false;
			std::wstring Name;
			std::wstring Path;

			AudioSessionItemChange();
			AudioSessionItemChange(AudioSessionItem*, OnChangeType);
			const OnChangeType GetAction();
			const bool IsCallAudioCb() const;
			const bool IsValid();
			const bool IsKeyFound(AudioAction);
			[[maybe_unused]] const bool IsKeyFound(uint32_t);
			[[maybe_unused]] const bool IsKeyFound(uint32_t, AudioAction);
			const bool IsKeyFound(AudioSessionItemChangeData[]);
			void Copy(AudioSessionItemChange&);
			void Copy(AudioSessionItem*);
			log_string ToString();
		};
	}
}