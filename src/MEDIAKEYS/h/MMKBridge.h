/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MEDIAKEYS DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MMKey {

		typedef std::function<uint32_t(std::wstring&)> callGetPid;

		class FLAG_EXPORT MMKBridge : public Common::MIDI::MidiInstance
		{
		private:
			const bool InCallBack(Common::MIDI::MidiUnit&, DWORD&);
			callGetPid pid_cb__;

		public:
			MMKBridge();
			~MMKBridge();

			void Stop();
			void SetPidCb(callGetPid);

			static MMKBridge& Get();

		};
	}
}

