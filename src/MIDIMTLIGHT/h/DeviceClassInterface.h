/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol, support USB Open DMX and USB RS485 dongle.
	+ Art-Net protocol, support UDP local network broadcast.
	(c) CC 2023, MIT

	MIDIMTLIGHT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace LIGHT {

		class DeviceClassInterface {
		protected:
			virtual void dispose_() = 0;

		public:

			virtual const bool is_open() const = 0;
			virtual bool send(byte*, size_t) = 0;
			virtual std::vector<byte> receive() = 0;
			virtual void stop() = 0;
		};
	}
}

