/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace SCRIPT {


		#pragma region Action Constant
		std::function<void(const uint8_t, const uint8_t, const uint8_t)> ActionConstant::update_cb_ = nullptr;

		const bool ActionConstant::update_once_(const uint8_t s, const uint8_t k, const uint8_t v) {
			try {
				auto& f = ActionConstant::update_cb_;
				if (!f) return false;
				f(s, k, v);
				return true;

			} catch (...) {}
			return false;
		}
		void ActionConstant::UpdateButton(const uint8_t s, const uint8_t k) {
			if (!update_once_(s, k, 0U)) return;
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
			(void) update_once_(s, k, 127U);
		}
		void ActionConstant::UpdateSlider(const uint8_t s, const uint8_t k, const uint8_t v) {
			(void)update_once_(s, k, v);
		}
		void ActionConstant::SetUpdateCb(std::function<void(const uint8_t, const uint8_t, const uint8_t)> cb) {
			ActionConstant::update_cb_ = cb;
		}
		std::function<void(const uint8_t, const uint8_t, const uint8_t)> ActionConstant::GetUpdateCb() {
			return (ActionConstant::update_cb_) ? ActionConstant::update_cb_ : [](const uint8_t, const uint8_t, const uint8_t) {};
		}
		const bool ActionConstant::IsUpdate() {
			return (ActionConstant::update_cb_ != nullptr);
		}
		#pragma endregion
	}
}