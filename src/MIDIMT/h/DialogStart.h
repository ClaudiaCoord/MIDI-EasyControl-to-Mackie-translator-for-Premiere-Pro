/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIDIMT {

		class DialogStart
		{
		private:
			handle_ptr<HWND> hwnd__;
			CbEvent mcb__;

			void Dispose();
			void BuildLangComboBox();
			void BuildDeviceComboBox(const std::wstring);
			void BuildProxyComboBox(const uint32_t);
			void BuildSmartHomeLogLevelComboBox(const int32_t);

			void SetConfigurationInfo(HWND, std::shared_ptr<Common::MIDI::MidiDevice>&, Common::common_config&);
			void SetSliderInfo(HWND, uint32_t, uint32_t);
			void SetSliderValues(HWND, uint32_t , uint32_t, uint32_t, uint32_t, uint32_t);
			uint32_t GetSliderValue(HWND, uint32_t );

		public:

			DialogStart();
			~DialogStart();
			const bool IsRunOnce();
			void SetFocus();

			void InitDialog(HWND);
			void EndDialog();

			void ChangeOnJogfilter();
			void ChangeOnLog();
			void ChangeOnSysAutoStart();
			void ChangeOnSysAutoRun();
			void ChangeOnAutoRunConfig();
			void ChangeOnMixerfastvalue();
			void ChangeOnMixeroldvalue();
			void ChangeOnMixerRightClick();
			void ChangeOnConfigFileOpen();
			void ChangeOnManualPort();
			void ChangeOnSliders();
			void ChangeOnProxy();
			void ChangeOnLang();
			void ChangeOnDevice();
			void ChangeOnMmkeyEnable();
			void ChangeOnMixerEnable();

			void ChangeOnSmartHouseEnable();
			void ChangeOnSmartHouseLogLevel();
			void ChangeOnSmartHouseSsl();
			void ChangeOnSmartHouseSelfSign();
			void ChangeOnSmartHouseIpAddress();
			void ChangeOnSmartHousePort();
			void ChangeOnSmartHouseLogin();
			void ChangeOnSmartHousePass();
			void ChangeOnSmartHousePsk();
			void ChangeOnSmartHouseCa();
			void ChangeOnSmartHousePrefix();

			void EventLog();
			void EventMonitor();

			void ConfigSave();
			void AutoStart();
			void StartFromUi();
			bool Start(std::wstring = L"");
			void Stop();
			bool IsStart();
		};
	}
}