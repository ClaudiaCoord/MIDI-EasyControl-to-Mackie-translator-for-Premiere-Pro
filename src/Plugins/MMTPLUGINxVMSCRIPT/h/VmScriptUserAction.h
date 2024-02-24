/*
	MIDI-MT is a high level application driver for USB MIDI control surface.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace SCRIPT {

		class ActionConstant {
		private:
			static std::function<void(const uint8_t, const uint8_t, const uint8_t)> update_cb_;
			static const bool update_once_(const uint8_t, const uint8_t, const uint8_t);

		public:
			static void SetUpdateCb(std::function<void(const uint8_t, const uint8_t, const uint8_t)> = nullptr);
			static std::function<void(const uint8_t, const uint8_t, const uint8_t)> GetUpdateCb();

			static void UpdateButton(const uint8_t, const uint8_t);
			static void UpdateSlider(const uint8_t, const uint8_t, const uint8_t);
			static const bool IsUpdate();
		};
	}
}