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

		class DMXSerial : public DMXClassInterface {
		private:
			uint32_t wbg_id = 0U;
			std::atomic<DWORD> pkt_id_{ 0U };
			std::atomic<bool> isrun_{ false };
			std::atomic<bool> isready_{ false };
			std::mutex mutex_{};

			LIGHT::SerialPort sp_{};
			LIGHT::SerialPortConfigs devices_{};
			LIGHT::SerialPortConfig  config_{};

			void dispose_() override;
			bool reconnect_() override;

		public:

			DMXSerial(std::function<DMXPacket()>);
			~DMXSerial();

			bool IsRun() override;
			bool Send(DMXPacket, bool) override;
			void Send(std::vector<byte>&) override;
			bool Send_async(DMXPacket, DWORD, bool) override;
			std::vector<byte> Receive() override;
			void Stop() override;
			bool Start(SerialPortConfig&);

			LIGHT::SerialPortConfig&  GetDivice();
			LIGHT::SerialPortConfigs& GetDivices(bool = false);
		};
	}
}

