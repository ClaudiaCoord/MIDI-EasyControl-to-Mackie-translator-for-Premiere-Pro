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

		class DMXArtnet : public DMXClassInterface {
		private:
			uint32_t wbg_id = 0U;
			std::atomic<DWORD> pkt_id_{ 0U };
			std::atomic<bool> isrun_{ false };
			std::atomic<bool> isready_{ false };
			std::mutex mutex_{};

			LIGHT::UdpPort udp_{};
			LIGHT::ArtnetPacket  packet_{};
			LIGHT::ArtnetConfigs interfaces_{};
			LIGHT::ArtnetConfig  config_{};

			void dispose_() override;
			bool reconnect_() override;

		public:

			DMXArtnet(std::function<DMXPacket()>);
			~DMXArtnet();

			ArtnetPacketType GetPacketType() const;
			void SetPacketType(ArtnetPacketType);
			std::vector<byte> CreateArtnetPacket(DMXPacket&);

			bool IsRun() override;
			void Send(std::vector<byte>&) override;
			void Send_async(std::vector<byte>&) override;
			std::vector<byte> Receive() override;
			void Stop() override;
			bool Start(ArtnetConfig&);

			LIGHT::ArtnetConfig&  GetInterface();
			LIGHT::ArtnetConfigs& GetInterfaces(bool = false);
		};
	}
}
