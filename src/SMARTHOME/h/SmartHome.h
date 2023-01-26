/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	SMARTHOME DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MQTT {

		class FLAG_EXPORT SmartHome : public Common::MIDI::MidiInstance
		{
		private:
			std::atomic<DWORD> time__;
			std::unique_ptr<Broker> broker__;
			const bool InCallBack(Common::MIDI::MidiUnit&, DWORD&);

		public:
			SmartHome();
			~SmartHome();

			bool Start();
			void Stop();

			static SmartHome& Get();
			void SetTitle(std::vector<MIDI::Mackie::Target>&);

		};
	}
}

