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

		class UdpPort : DeviceClassInterface {
		private:
			std::unique_ptr<asio::io_service> io__;
			std::unique_ptr<asio::ip::udp::socket> socket__;
			std::unique_ptr<asio::ip::udp::endpoint> endpoint__;

			void dispose_() override;

		public:
			std::atomic<bool> canceller{ false };

			UdpPort();
			~UdpPort();

			const bool is_open() const override;
			bool send(byte*, size_t) override;
			std::vector<byte> receive() override;
			void stop() override;
			bool start(ArtnetConfig&);
		};
	}
}

