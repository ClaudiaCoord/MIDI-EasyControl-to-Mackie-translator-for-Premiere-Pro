/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (SmartHomeKeysPlugin) MQTT Broker

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MQTT {

		struct default_mosquitto_deleter {
			void operator()(void* m);
		};

		class Broker {
		private:
			std::atomic<bool> isrun_{ false };
			std::atomic<int32_t> ids_{ 0 };
			std::unique_ptr<void, default_mosquitto_deleter> broker_;
			BrokerConfig<std::string> config_;

			void dispose_();
			std::u8string get_target_topic_(MIDI::Mackie::Target);
			std::u8string get_target_ttitle_(MIDI::Mackie::Target);
			std::string build_topic_(std::u8string, const std::u8string_view&);
		public:
			
			Broker();
			~Broker();

			const bool isrunning();
			bool init(BrokerConfig<std::wstring>&);
			void release();
			void reset();

			template <typename T>
			void send(T, MIDI::Mackie::Target);

			void settitle(MIDI::Mackie::Target);
			void settitle(std::vector<MIDI::Mackie::Target>&);
			std::wstring GetHost();
			int32_t GetLogLevel();
		};

		extern template void Broker::send<bool>(bool, MIDI::Mackie::Target);
		extern template void Broker::send<uint32_t>(uint32_t, MIDI::Mackie::Target);
		extern template void Broker::send<std::string>(std::string, MIDI::Mackie::Target);
		extern template void Broker::send<std::u8string>(std::u8string, MIDI::Mackie::Target);

	}
}
