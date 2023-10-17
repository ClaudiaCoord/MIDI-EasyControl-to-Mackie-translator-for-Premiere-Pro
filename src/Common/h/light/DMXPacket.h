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

		typedef std::array<uint8_t, 512> DMXData;

		class FLAG_EXPORT DMXPacket {
		private:
			DMXData data__{};
			size_t midx__{ 0 };
			bool blackout{ false };

			#if defined(DEV_DMXUSBPRO_ENABLE)
			size_t   build_size_(size_t);
			uint16_t build_length_(size_t);
			#endif

		public:

			const bool empty() const;
			const size_t size() const;
			const bool is_blackout();

			size_t get_index();
			uint8_t get_value(uint16_t);
			DMXData& get_data();

			void set_blackout(bool);
			void set_index(size_t);
			void set_value8(uint16_t, uint8_t);
			void set_value16(uint16_t, uint8_t);

			std::vector<byte> create();
		};
	}
}

