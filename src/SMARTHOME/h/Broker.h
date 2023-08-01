/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	SMARTHOME DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MQTT {

		struct FLAG_EXPORT default_mosquitto_deleter {
			void operator()(void* m);
		};

		class FLAG_EXPORT Broker
		{
		private:
			std::atomic<bool> isrun__{ false };
			std::atomic<int32_t> ids__{ 0 };
			std::unique_ptr<void, default_mosquitto_deleter> broker__;
			BrokerConfig<std::string> config__;

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
			FLAG_EXPORT void send(T, MIDI::Mackie::Target);

			void settitle(std::vector<MIDI::Mackie::Target>&);
			std::wstring GetHost();
			int32_t GetLogLevel();
		};

		extern template FLAG_EXPORT void Broker::send<bool>(bool, MIDI::Mackie::Target);
		extern template FLAG_EXPORT void Broker::send<uint32_t>(uint32_t, MIDI::Mackie::Target);
		extern template FLAG_EXPORT void Broker::send<std::string>(std::string, MIDI::Mackie::Target);
		extern template FLAG_EXPORT void Broker::send<std::u8string>(std::u8string, MIDI::Mackie::Target);

	}
}
