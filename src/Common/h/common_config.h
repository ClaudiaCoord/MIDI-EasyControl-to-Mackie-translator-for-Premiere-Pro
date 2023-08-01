/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {

	typedef std::function<void(std::shared_ptr<Common::MIDI::MidiDevice>&)> callConfigChangeCb;


	class FLAG_EXPORT common_config_local {
	private:
		std::atomic<bool> midibridgerun_;
		std::atomic<bool> audiomixerrun_;
		std::atomic<bool> mmkeyesrun_;
		std::atomic<bool> smarthomerun_;
	public:
		const bool IsMidiBridgeRun();
		void IsMidiBridgeRun(bool);

		const bool IsAudioMixerRun();
		void IsAudioMixerRun(bool);

		const bool IsMMKeyesRun();
		void IsMMKeyesRun(bool);

		const bool IsSmartHomeRun();
		void IsSmartHomeRun(bool);
	};

    class FLAG_EXPORT common_config {
	private:

		static common_config ctrlcommonconfig__;

		const wchar_t* LogTag = L"Configuration ";

		time_t config_last_write_time = 0;
		std::shared_ptr<Common::MIDI::MidiDevice> config_ptr__;
		Common::common_event<callConfigChangeCb> event__;

		void updateconf_();
		
    public:

		registry Registry;
		common_config_local Local;
		json_recent RecentConfig;
		ui_themes UiThemes;

		common_config();
		~common_config();
		static common_config& Get();
		std::shared_ptr<Common::MIDI::MidiDevice>& GetConfig();

		const bool		IsStart();
		const bool		IsConfig();
		const bool		IsNewConfig();
		const bool		IsConfigEmpty();
		const bool		IsProxy();

		const bool		IsJogSceneFilter();
		void			IsJogSceneFilter(bool);

		const bool		IsAutoStart();
		void			IsAutoStart(bool);

		void			NumProxy(uint32_t);
		const uint32_t	NumProxy();

		uint32_t		ButtonOnInterval();
		void			ButtonOnInterval(uint32_t);

		uint32_t		ButtonOnLongInterval();
		void			ButtonOnLongInterval(uint32_t);

		const bool Load(std::wstring = L"");
		const bool Save(std::wstring = L"");

		uint32_t add(callConfigChangeCb);
		void remove(callConfigChangeCb);
		void remove(uint32_t);
    };
}

