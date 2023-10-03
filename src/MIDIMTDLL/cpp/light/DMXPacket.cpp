/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ using in DMX protocol.
	+ using in Art-Net protocol.
	(c) CC 2023, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace LIGHT {

		/* DmxUsbPro device scheme (NOT TESTED) */
		#if defined(DEV_DMXUSBPRO_ENABLE)
		constexpr uint8_t dmxhead[] = {
			0x7e,		// PACKET_START_CODE (0)
			0x06,		// DMX_TX_MODE		 (1)
			0x00, 0x00, // length			 (2,3)
			0x00,		// DMX start code
			0x00		// empty not include address slot
		};
		constexpr size_t dmxheadsize = std::size(dmxhead);

		size_t DMXPacket::build_size_(size_t z) {
			return z + dmxheadsize + 1;
		}
		uint16_t DMXPacket::build_length_(size_t z) {
			uint16_t length = static_cast<uint16_t>(z);
			length = length + (length % 2);
			return (length > 512) ? 512 : length;
		}
		#endif

		const bool DMXPacket::empty() const {
			return midx__ == 0U;
		}
		const size_t DMXPacket::size() const {
			return (midx__ == 0U) ? data__.size() : ((midx__ > 512) ? 512 : (midx__ + 1));
		}
		DMXData& DMXPacket::get_data() {
			return std::ref(data__);
		}
		size_t DMXPacket::get_index() {
			return midx__;
		}
		uint8_t DMXPacket::get_value(uint16_t idx) {
			if ((idx > 512U) || (idx == 0U)) return 0U;
			return data__[--idx];
		}
		void DMXPacket::set_value(uint16_t idx, uint8_t val) {
			if ((idx > 512U) || (idx == 0U)) return;
			--idx;
			data__[idx] = val;
			midx__ = (midx__ < idx) ? idx : midx__;
		}
		void DMXPacket::set_index(size_t val) {
			if (midx__ > 0) return;
			midx__ = (val >= 513) ? midx__ : val;
		}
		std::vector<byte> DMXPacket::create() {
			#if defined(DEV_DMXUSBPRO_ENABLE)
			/*
			 * DmxUsbPro device scheme (NOT TESTED)
			*/
			size_t szp = size(),
				   szv = build_size_(szp);
			uint16_t length = build_length_(szp);

			std::vector<byte> data(szv + 1);
			data.assign(&dmxhead[0], &dmxhead[5]);
			data[2] = (length & 0xff);
			data[3] = (length >> 8);
			data.insert(data.end(), &data__[0], &data__[szp]);
			data.push_back(0xe7);
			return data;
			#else
			//return std::vector<byte>(&data__[0], &data__[midx__]);
			return std::vector<byte>(data__.begin(), data__.end());
			#endif
		}
	}
}
