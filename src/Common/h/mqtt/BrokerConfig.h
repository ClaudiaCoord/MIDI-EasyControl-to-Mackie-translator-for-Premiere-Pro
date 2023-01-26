/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	COMMON::MQTT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MQTT {

		template <typename T1>
		class FLAG_EXPORT BrokerConfig {
		private:
			T1 clr_();
			template <typename T2>
			T1 scnv_(T2& s);
		public:
			int32_t loglevel = 0;
			uint32_t port = 1883;
			T1 host;
			T1 login;
			T1 password;
			T1 sslpsk;
			T1 mqttprefix;
			T1 certcapath;
			bool isssl = false;
			bool isselfsigned = false;

			const bool empty();
			void clear();
			std::wstring Dump();

			template <typename T2>
			void FLAG_EXPORT Copy(BrokerConfig<T2>&);

		};
	}
}

