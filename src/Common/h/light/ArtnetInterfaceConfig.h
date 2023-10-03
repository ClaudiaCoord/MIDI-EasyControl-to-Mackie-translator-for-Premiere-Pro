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

		class FLAG_EXPORT ArtnetConfig {
		public:
			bool enable{ false };
			uint32_t port{ 0x1936 };
			uint32_t universe{ 1 };
			std::wstring ip;
			std::wstring mask;
			std::wstring broadcast;

			ArtnetConfig();
			ArtnetConfig(std::string, std::string, std::string);
			void Copy(ArtnetConfig&);

			const bool empty();

			log_string dump();
		};

		class FLAG_EXPORT ArtnetConfigs {
		private:
			std::vector<ArtnetConfig> interfaces__{};
		public:

			void clear();
			uint32_t count();
			const bool empty();
			std::vector<ArtnetConfig>& get();
			ArtnetConfig& get(uint16_t);

			void add(std::string, std::string, std::string);

			log_string dump();
		};
	}
}

