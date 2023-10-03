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

		class DMXArtnet : DMXClassInterface {
		private:
			uint32_t wbg_id = 0U;
			std::atomic<DWORD> pkt_id__{ 0U };
			std::atomic<bool> isrun__{ false };
			std::atomic<bool> isready__{ false };
			std::mutex mutex__{};
			UdpPort udp__{};
			ArtnetPacket packet__{};
			ArtnetConfigs interfaces__{};
			ArtnetConfig  config__{};

			void dispose_() override;
			bool reconnect_() override;

		public:

			DMXArtnet(std::function<DMXPacket()>);
			~DMXArtnet();

			ArtnetPacketType GetPacketType();
			void SetPacketType(ArtnetPacketType);

			bool IsRun() override;
			bool Send(DMXPacket, bool = false) override;
			void Send(std::vector<byte>&) override;
			bool Send_async(DMXPacket, DWORD, bool = false) override;
			std::vector<byte> Receive() override;
			void Stop() override;
			bool Start(ArtnetConfig&);

			ArtnetConfig&  GetInterface();
			ArtnetConfigs& GetInterfaces(bool = false);
		};
	}
}
