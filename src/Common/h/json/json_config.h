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

		class FLAG_EXPORT json_config {
		public:

			json_config();
			~json_config() = default;

			const bool Read(MMTConfig* md, std::wstring filepath, bool = false);
			const bool ReadFile(std::wstring& filepath, std::function<bool(Tiny::TinyJson&, std::wstring&)>);

			const bool ReadUnitConfig(Tiny::TinyJson&, MMTConfig*, std::wstring&);
			void ReadMidiConfig(Tiny::TinyJson&, MIDI::MidiConfig&);
			void ReadMqttConfig(Tiny::TinyJson&, MQTT::BrokerConfig<std::wstring>&);
			void ReadMMkeyConfig(Tiny::TinyJson&, MIDI::MMKeyConfig&);
			void ReadLightConfig(Tiny::TinyJson&, LIGHT::LightsConfig&);
			void ReadRemoteConfig(Tiny::TinyJson&, REMOTE::RemoteConfig<std::wstring>&);
			void ReadGamepadConfig(Tiny::TinyJson&, GAMEPAD::JoystickConfig&);
			void ReadLightDmxConfig(Tiny::TinyJson&, LIGHT::SerialPortConfig&);
			void ReadLightArtnetConfig(Tiny::TinyJson&, LIGHT::ArtnetConfig&);
			void ReadVmScriptConfig(Tiny::TinyJson&, SCRIPT::VmScriptConfig&);

			const bool Write(MMTConfig* md, std::wstring filepath, bool = false);
			const bool WriteFile(std::wstring filepath, std::function<bool(Tiny::TinyJson&, std::wstring&)>);

			const bool WriteUnitConfig(Tiny::TinyJson&, MMTConfig*);
			void WriteMidiConfig(Tiny::TinyJson&, MIDI::MidiConfig&);
			void WriteMqttConfig(Tiny::TinyJson&, MQTT::BrokerConfig<std::wstring>&);
			void WriteMMkeyConfig(Tiny::TinyJson&, MIDI::MMKeyConfig&);
			void WriteRemoteConfig(Tiny::TinyJson&, REMOTE::RemoteConfig<std::wstring>&);
			void WriteGamepadConfig(Tiny::TinyJson&, GAMEPAD::JoystickConfig&);
			void WriteLightConfig(Tiny::TinyJson&, LIGHT::LightsConfig&);
			void WriteLightDmxConfig(Tiny::TinyJson&, LIGHT::SerialPortConfig&);
			void WriteLightArtnetConfig(Tiny::TinyJson&, LIGHT::ArtnetConfig&);
			void WriteVmScriptConfig(Tiny::TinyJson&, SCRIPT::VmScriptConfig&);

			std::wstring Dump(MMTConfig* md);

			template<class T1>
			bool Read(T1 & t, std::wstring filepath) {
				return Read(t.get(), filepath);
			}
			template<class T1>
			bool Write(T1& t, std::wstring filepath, bool issetreg = true) {
				return Write(t.get(), filepath, issetreg);
			}
			template<class T1>
			bool Dump(T1& t) {
				return dump(t.get());
			}
		};
	}
}
