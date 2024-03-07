/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol.
	+ Art-Net protocol.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace LIGHT {

		constexpr uint8_t arthead[] = {
			0x41, 0x72, 0x74, 0x2d, 0x4e, 0x65, 0x74, 0x00, // Art-Net (0,7)
			0x00, 0x50, // opcode OP_POLL:0x20 / OP_OUTPUT:0x50	(8,9)
			0x00, 0x0e, // version 14						(10,11)
			0x00,       // sequence							(12)
			0x00,       // physical							(13)
			0x00,       // subnet universe					(14)
			0x00,       // net: 0-127						(15)
			0x00, 0x02, // length hi + lenght				(16,17)
			0x00		// empty not include address slot
		};
		constexpr size_t artheadsize = (std::size(arthead) - 1);

		uint8_t ArtnetPacket::build_seq_() {
			seq__++;
			seq__ %= 256;
			return seq__;
		}
		uint16_t ArtnetPacket::build_length_(size_t z) const {
			uint16_t length = (PacketType == ArtnetPacketType::DYNAMIC_SIZE) ? static_cast<uint16_t>(z) : 512;
			length = length + (length % 2);
			return (length > 512) ? 512 : length;
		}

		std::vector<byte> ArtnetPacket::create(DMXPacket& pdmx) {
			return create(pdmx, build_seq_(), 1);
		}
		std::vector<byte> ArtnetPacket::create(DMXPacket& pdmx, uint32_t portddress) {
			return create(pdmx, build_seq_(), portddress);
		}
		std::vector<byte> ArtnetPacket::create(DMXPacket& pdmx, uint32_t net, uint32_t subnet, uint32_t universe) {
			uint32_t portddress = (net << 8) + (subnet << 4) + universe;
			return create(pdmx, build_seq_(), portddress);
		}
		std::vector<byte> ArtnetPacket::create(DMXPacket& pdmx, uint8_t seq, uint32_t portddress) {
			uint16_t length = build_length_(pdmx.size());

			std::vector<byte> data(artheadsize + length);
			data.assign(&arthead[0], &arthead[18]);
			data[12] = seq;
			data[14] = (portddress & 0xff);
			data[15] = (portddress >> 8);
			data[16] = (length >> 8);
			data[17] = (length & 0xff);

			if (PacketType == ArtnetPacketType::DYNAMIC_SIZE) {
				uint8_t* d = pdmx.get_data().data();
				data.insert(data.end(), &d[0], &d[length]);
			} else {
				DMXData d = pdmx.get_data();
				data.insert(data.end(), d.begin(), d.end());
			}
			return data;
		}
	}
}
