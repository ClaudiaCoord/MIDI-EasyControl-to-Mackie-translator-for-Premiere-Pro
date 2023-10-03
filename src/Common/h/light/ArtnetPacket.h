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

		enum class ArtnetPacketType {
			DYNAMIC_SIZE = 0,
			FULL_SIZE
		};

		class FLAG_EXPORT ArtnetPacket {
		private:
			uint8_t seq__{ 0 };
			uint8_t build_seq_();
			uint16_t build_length_(size_t);

		public:

			ArtnetPacketType PacketType{ ArtnetPacketType::DYNAMIC_SIZE };

			std::vector<byte> create(DMXPacket&);
			std::vector<byte> create(DMXPacket&, uint32_t);
			std::vector<byte> create(DMXPacket&, uint32_t net, uint32_t subnet, uint32_t universe);
			std::vector<byte> create(DMXPacket&, uint8_t, uint32_t);
		};

	}
}
