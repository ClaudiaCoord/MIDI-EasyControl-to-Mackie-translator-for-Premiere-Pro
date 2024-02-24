/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	COMMON::MQTT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace REMOTE {

		template <typename T1>
		class FLAG_EXPORT RemoteConfig {
		private:
			T1 clr_();
			template <typename T2>
			T1 scnv_(T2& s);
		public:
			int32_t timeoutreq{ 5 };
			int32_t timeoutidle{ 0 };
			uint32_t port{ 8888 };
			uint16_t loglevel{ 1 };
			T1 host{};
			T1 server_ua{};
			bool enable{ false };
			bool isipv6{ false };
			bool isreuseaddr{ true };
			bool isfastsocket{ false };

			const bool empty();
			void clear();
			std::wstring dump();

			template <typename T2>
			void FLAG_EXPORT copy(const RemoteConfig<T2>&);

		};
	}
}

