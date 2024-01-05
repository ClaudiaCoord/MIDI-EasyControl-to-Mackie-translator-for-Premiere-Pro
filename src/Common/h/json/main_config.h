/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	COMMON::JSON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace JSON {

		class FLAG_EXPORT MMTConfig {
		private:
			void copy_settings_(MMTConfig*);
			void copy_units_(MMTConfig*);
		public:
			bool								auto_start{ false };
			std::wstring						config{};
			std::wstring						builder{};
			std::vector<MIDI::MidiUnit>			units{};
			MIDI::MidiConfig					midiconf{};
			MIDI::MMKeyConfig					mmkeyconf{};
			LIGHT::LightsConfig					lightconf{};
			MQTT::BrokerConfig<std::wstring>	mqttconf{};
			REMOTE::RemoteConfig<std::wstring>  remoteconf{};

			MMTConfig* get();

			MMTConfig();
			~MMTConfig();
			bool empty();
			void Init();
			void Add(MIDI::MidiUnit);
			void Clear();
			void Copy(MMTConfig*);
			std::wstring Dump();

			template <class T1>
			void CopySettings(T1& ptr) {
				copy_settings_(ptr.get());
			}
			template <class T1>
			void Copy(T1& ptr) {
				auto mmt = ptr.get();
				if (!mmt) return;

				copy_settings_(mmt);
				copy_units_(mmt);
			}
		};
	}
}

typedef std::function<void(std::shared_ptr<Common::JSON::MMTConfig>&)> callConfigCb_t;
