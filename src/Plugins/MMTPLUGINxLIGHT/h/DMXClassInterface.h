/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol, support USB Open DMX and USB RS485 dongle.
	+ Art-Net protocol, support UDP local network broadcast.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (LightKeysPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace LIGHT {

		class DMXClassInterface {
		protected:
			std::function<DMXPacket()> get_dmx_packet_ = []() { return DMXPacket(); };

			virtual void dispose_() = 0;
			virtual bool reconnect_() = 0;

		public:

			DMXClassInterface(std::function<DMXPacket()> f) : get_dmx_packet_(f) {}
			virtual ~DMXClassInterface() = default;

			virtual bool IsRun() = 0;
			virtual void Send(std::vector<byte>&) = 0;
			virtual void Send_async(std::vector<byte>&) = 0;
			virtual std::vector<byte> Receive() = 0;
			virtual void Stop() = 0;
		};
	}
}
