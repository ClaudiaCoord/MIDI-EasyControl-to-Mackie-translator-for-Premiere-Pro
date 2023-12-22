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

	class FLAG_EXPORT common_config_local {
	private:
		std::atomic<bool> midibridgerun_{ false };
		std::atomic<bool> audiomixerrun_{ false };

	public:
		const bool IsMidiDriverRun();
		void IsMidiDriverRun(bool);

		const bool IsAudioMixerRun();
		void IsAudioMixerRun(bool);
	};

    class FLAG_EXPORT common_config {
	private:

		static common_config ctrlcommonconfig_;

		const wchar_t* LogTag = L"Configuration ";

		time_t config_last_write_time{ 0 };
		std::shared_ptr<Common::JSON::MMTConfig> config_ptr_{};
		Common::common_event<callConfigCb_t> event_{};

		void update_conf_();

		template<typename T>
		void update_value_(T n, T& v) {
			if (v != n) {
				v = n;
				update_conf_();
			}
		}

	public:

		registry Registry{};
		common_config_local Local{};
		JSON::json_recent RecentConfig{};
		ui_themes UiThemes{};

		common_config();
		~common_config();
		static common_config& Get();
		std::shared_ptr<Common::JSON::MMTConfig>& GetConfig();

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

		std::wstring	GetConfigPath();

		const bool Load(std::wstring = L"");
		const bool Save(std::wstring = L"");

		uint32_t add(callConfigCb_t);
		void add(callConfigCb_t, uint32_t);
		void remove(callConfigCb_t);
		void remove(uint32_t);
    };
}

