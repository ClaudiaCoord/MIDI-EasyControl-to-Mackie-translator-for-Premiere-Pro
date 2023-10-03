/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol.
	+ Art-Net protocol.
	(c) CC 2023, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace LIGHT {

		class FLAG_EXPORT SerialPortConfig {
		public:
			bool enable{ false };
			int32_t port{ -1 };
			uint32_t baudrate{ 250000 };
			uint8_t stop_bits{ 2 };
			uint8_t timeout{ 3 }; /* send DMX packet timeout */
			std::wstring name;

			SerialPortConfig();
			SerialPortConfig(int32_t, std::string);
			SerialPortConfig(int32_t, std::wstring);
			void Copy(SerialPortConfig&);

			const bool empty();
			std::string GetPort();

			log_string dump();
		};

		class FLAG_EXPORT SerialPortConfigs {
		private:
			std::vector<SerialPortConfig> devices__{};
		public:

			void clear();
			uint32_t count();
			const bool empty();
			SerialPortConfig& get(uint16_t);
			std::vector<SerialPortConfig>& get();

			log_string dump();

			template <typename T1>
			void add(int32_t p, T1 s) {
				devices__.push_back(SerialPortConfig(p, s));
			}
		};
	}
}

