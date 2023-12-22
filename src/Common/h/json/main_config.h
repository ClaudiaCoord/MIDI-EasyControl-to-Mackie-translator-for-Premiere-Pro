/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	COMMON::JSON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace JSON {

		class FLAG_EXPORT MMTConfig {
		private:
			void copysettings_(MMTConfig*);
		public:
			bool								auto_start{ false };
			std::wstring						config{};
			std::wstring						builder{};
			std::vector<MIDI::MidiUnit>			units{};
			MIDI::MidiConfig					midiconf{};
			MIDI::MMKeyConfig					mmkeyconf{};
			MQTT::BrokerConfig<std::wstring>	mqttconf{};
			LIGHT::LightsConfig					lightconf{};

			MMTConfig* get();

			MMTConfig();
			~MMTConfig();
			bool empty();
			void Init();
			void Add(MIDI::MidiUnit);
			void Clear();
			std::wstring Dump();

			template <class T1>
			void CopySettings(T1& ptr) {
				copysettings_(ptr.get());
			}
		};
	}
}

typedef std::function<void(std::shared_ptr<Common::JSON::MMTConfig>&)> callConfigCb_t;
