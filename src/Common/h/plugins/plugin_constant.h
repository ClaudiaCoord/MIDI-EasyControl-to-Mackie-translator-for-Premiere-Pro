/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL + MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace IO {

		enum class PluginCbType : uint32_t {
			None   = 0x0,
			Out1Cb = (1u << 1),
			Out2Cb = (1u << 2),
			In1Cb  = (1u << 3),
			In2Cb  = (1u << 4),
			LogCb  = (1u << 5),
			LogsCb = (1u << 6),
			LogoCb = (1u << 7),
			PidCb  = (1u << 8),
			ConfCb = (1u << 9),
			IdCb   = (1u << 0)
		};
		enum class PluginClassTypes : int {
			ClassNone = 0,
			ClassSys,       // system class (log, pid etc)
			ClassIn,        // bridge IN (system) class
			ClassOut,       // bridge OUT (system) class
			ClassProxy,
			ClassMixer,
			ClassMonitor,
			ClassMediaKey,
			ClassMqttKey,
			ClassLightKey,
			ClassRemote,
			ClassInMidi,
			ClassOutMidi,
			ClassOutMidiMackie,
			ClassVirtualMidi,
			ClassOut1,
			ClassOut2,
		};
		enum class PluginTypeId : int {
			BY_NONE = 0,
			BY_CONFIG_ID,
			BY_DIALOG_ID,
			BY_NAME,
			BY_GUID,
			BY_PATH,
			BY_FILE,
		};
	}
}

