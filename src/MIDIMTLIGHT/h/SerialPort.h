/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol, support USB Open DMX and USB RS485 dongle.
	+ Art-Net protocol, support UDP local network broadcast.
	= serial compatible ENTTEC OPEN DMX == (standart RS-485 USB Chinese Dongle).
	(c) CC 2023, MIT

	MIDIMTLIGHT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace LIGHT {

		class SerialPort : DeviceClassInterface {
		private:
			
			std::unique_ptr<asio::io_service> io__;
			std::unique_ptr<asio::serial_port> serial__;

			void dispose_() override;
			void open_DMX_usb_RS485_HELLO_(auto h);

		public:
			std::atomic<bool> canceller{ false };

			SerialPort();
			~SerialPort();

			const bool is_open() const override;
			bool send(byte*, size_t) override;
			std::vector<byte> receive() override;
			void stop() override;
			bool start(SerialPortConfig&);
		};
	}
}

