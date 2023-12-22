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

		class FLAG_EXPORT AudioSessionItemChange
		{
		private:
			bool isinit_{ false };
			bool isevent_{ true };
			bool iscallaudiocb_{ false };
			OnChangeType actiontype_{ OnChangeType::OnChangeNone };

			template<typename T>
			void copychangedata_(T&);
			template<typename T>
			void copychangevalue_(T&);
			template<typename T>
			void copychangeremove_(T&);

		public:

			
			AudioSessionItemBase Item{};

			AudioSessionItemChange() = default;
			AudioSessionItemChange(AudioSessionItem*, OnChangeType, bool = true);
			~AudioSessionItemChange();

			const bool IsEvent();
			const bool IsValid();
			const bool IsUpdateValid();
			const bool IsCallAudioCb();
			const OnChangeType GetAction();

			void Copy(AudioSessionItemChange&, bool = false);
			void Update(AudioSessionItemChange&);

			/* proxy method */

			const GUID GetGUID();
			const std::size_t GetAppId();
			std::wstring GetAppName();
			uint8_t GetVolume();
			bool GetMute();
			void SetVolume(const uint8_t);
			void SetMute(const bool);

			/* other */

			log_string to_string();

			static OnChangeType selector_type(OnChangeType);
			static OnChangeType filter_type(OnChangeType);
			static bool filter_cb(OnChangeType);
		};
	}
}