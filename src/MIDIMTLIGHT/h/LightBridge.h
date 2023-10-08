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

		typedef std::function<uint32_t(std::wstring&)> callGetPid;

		class FLAG_EXPORT LightBridge : public Common::MIDI::MidiInstance
		{
		private:
			const bool InCallBack(Common::MIDI::MidiUnit&, DWORD&);
			callGetPid pid_cb__;
			std::atomic<bool> dmx_pool_active__{ false };
			std::atomic<bool> dmx_pause__{ false };
			std::atomic<DWORD> packet_id__{ 0U };
			std::unique_ptr<DMXArtnet> artnet__;
			std::unique_ptr<DMXSerial> dmxport__;
			std::shared_ptr<locker_awaiter> lock__{};
			DMXPacket dmx_packet__{};

			DMXPacket getDMXPacket_();
			void poolDMXPacket_();

		public:


			LightBridge();
			~LightBridge();

			void Stop();
			bool Start();
			void SetPidCb(callGetPid);
			void BlackOut(bool);
			bool IsBlackOut();
			void DmxPause(bool);
			bool IsDmxPause();

			static LightBridge& Get();

			SerialPortConfigs& GetDivices(bool = false);
			ArtnetConfigs& GetInterfaces(bool = false);
		};
	}
}

