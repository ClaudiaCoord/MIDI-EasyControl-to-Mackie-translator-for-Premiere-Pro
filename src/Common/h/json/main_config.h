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
			std::shared_mutex mtx_{};

			void copy_settings_(MMTConfig*);
			void copy_units_(MMTConfig*);
			MIDI::MidiUnit& find_(std::function<bool(const MIDI::MidiUnit&)>);

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
			GAMEPAD::JoystickConfig				gamepadconf{};
			SCRIPT::VmScriptConfig				vmscript{};

			MMTConfig* get();

			MMTConfig();
			~MMTConfig();

			bool empty();
			void init();
			void add(MIDI::MidiUnit);
			void clear();
			std::wstring dump();

			MIDI::MidiUnit& find(const uint8_t);
			MIDI::MidiUnit& find(const uint8_t, const uint8_t);
			MIDI::MidiUnit& find(const uint8_t, const uint8_t, const MIDI::MidiUnitType);
			MIDI::MidiUnit& find(const MIDI::Mackie::Target);

			template <class T1>
			void copy_settings(T1& ptr) {

				if (!ptr) return;
				std::unique_lock<std::shared_mutex> lock(mtx_);

				auto mmt = ptr.get();
				if (!mmt) return;

				copy_settings_(mmt);
			}
			template <class T1>
			void copy(T1& ptr) {

				if (!ptr) return;
				std::unique_lock<std::shared_mutex> lock(mtx_);

				auto mmt = ptr.get();
				if (!mmt) return;

				copy_settings_(mmt);
				copy_units_(mmt);
			}
		};
	}
}

typedef std::function<void(std::shared_ptr<Common::JSON::MMTConfig>&)> callConfigCb_t;
