/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ using in DMX protocol.
	+ using in Art-Net protocol.
	(c) CC 2023-2024, MIT

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

		DMXPacket::DMXPacket(const DMXPacket& p) {
			data_ = p.data_;
			midx_ = p.midx_;
			blackout = p.blackout;
		}

		const bool DMXPacket::empty() const {
			return midx_ == 0U;
		}
		const size_t DMXPacket::size() const {
			return (midx_ == 0U) ? data_.size() : ((midx_ > 512) ? 512 : (midx_ + 1));
		}
		DMXData& DMXPacket::get_data() {
			return std::ref(data_);
		}
		size_t DMXPacket::get_index() {
			return midx_;
		}
		uint8_t DMXPacket::get_value(uint16_t idx) {
			if ((idx > 512U) || (idx == 0U)) return 0U;
			return data_[--idx];
		}
		void DMXPacket::set_value8(uint16_t idx, const uint8_t val) {
			if ((idx > 512U) || (idx == 0U)) return;

			--idx;
			data_[idx] = val;
			midx_ = (midx_ < idx) ? idx : midx_;
		}
		void DMXPacket::set_value16(uint16_t idx, const uint8_t val) {
			if ((idx > 511U) || (idx == 0U)) return;

			uint16_t v = (UINT16_MAX / 256) * val;
			data_[(idx - 1)] = (v & 0xff);
			data_[idx] = (v >> 8);
			midx_ = (midx_ < idx) ? idx : midx_;
		}
		void DMXPacket::set_index(const size_t val) {
			if (midx_ > 0) return;
			midx_ = (val >= 513) ? midx_ : val;
		}
		void DMXPacket::set_blackout(bool b) {
			blackout = b;
		}
		const bool DMXPacket::is_blackout() {
			return blackout;
		}
		std::vector<byte> DMXPacket::create() {
			if (blackout) return std::vector<byte>(512);
			return std::vector<byte>(data_.begin(), data_.end());
		}
	}
}
