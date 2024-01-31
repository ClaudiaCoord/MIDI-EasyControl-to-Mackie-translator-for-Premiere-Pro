/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Joystick support.
	+ GamePad support.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (GamePad/Joistick Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace GAMEPAD {

		#pragma region JoistickContainer
		JoistickContainer::JoistickContainer(MIDI::MidiUnit& u) : unit_(u) {
		}
		MIDI::MidiUnit& JoistickContainer::get() const {
			return unit_;
		}
		#pragma endregion

		#pragma region JoistickData
		JoistickData::JoistickData(uint16_t i) : id(i) {
		}

		const std::wstring JoistickData::name() const {
			size_t len = std::wcslen(caps.szPname);
			if (!len) return std::wstring();
			return std::wstring(caps.szPname, caps.szPname + len);
		}
		const std::wstring JoistickData::oem() const {
			size_t len = std::wcslen(caps.szOEMVxD);
			if (!len) {
				len = std::wcslen(caps.szRegKey);
				if (!len) return std::wstring();
				return std::wstring(caps.szRegKey, caps.szRegKey + len);
			}
			return std::wstring(caps.szOEMVxD, caps.szOEMVxD + len);
		}
		const std::wstring JoistickData::label() const {
			std::wstring name_ = name();
			std::wstring oem_ = oem();
			if (name_.empty() && oem_.empty())
				return (log_string() << (is_offline ? L"-" : L"+") << L" [" << (uint16_t)(id + 1) << L"] gamepad/joistick not named").str();
			else if (oem_.empty())
				return (log_string() << (is_offline ? L"-" : L"+") << L" [" << (uint16_t)(id + 1) << L"] " << name().c_str()).str();
			else
				return (log_string() << (is_offline ? L"-" : L"+") << L" [" << (uint16_t)(id + 1) << L"] " << name().c_str() << L" (" << oem().c_str() << L")").str();
		}
		const bool JoistickData::enable() const {
			return !is_error && !is_offline;
		}
		#pragma endregion
	}
}