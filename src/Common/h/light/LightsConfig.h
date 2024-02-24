/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol.
	+ Art-Net protocol.
	(c) CC 2023-2024, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace LIGHT {

		class FLAG_EXPORT LightsConfig {
		public:
			bool ispool{ false };
			LIGHT::ArtnetConfig		artnetconf{};
			LIGHT::SerialPortConfig	dmxconf{};

			void copy(const LightsConfig&);
			const bool empty();
			log_string dump();
		};
	}
}

