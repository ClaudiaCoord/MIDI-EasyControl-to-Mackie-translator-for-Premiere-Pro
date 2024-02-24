/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(m) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace SCRIPT {

		#pragma region Macro Group
		MacroGroup::MacroGroup() {
		}

		void MacroGroup::ApplyValues() {
			try {
				if (!ActionConstant::IsUpdate()) return;

				worker_background::Get().to_async(std::async(std::launch::async, [=](const std::vector<UnitDef>& v) {

					for (auto& m : v) {
						if (m.empty()) continue;
						switch (m.GetType()) {
							using enum MIDI::MidiUnitType;
							case BTN:
							case BTNTOGGLE: {
								ActionConstant::UpdateButton(m.GetScene(), m.GetKey());
								break;
							}
							default: {
								ActionConstant::UpdateSlider(m.GetScene(), m.GetKey(), m.GetValue());
								break;
							}
						}
					}
				}, std::ref(macros_)));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void MacroGroup::UpdateValues() {
			try {
				if (empty()) return;

				worker_background::Get().to_async(
					std::async(std::launch::async, [=](const std::vector<UnitDef>& v) {
					try {
						if (!v.size()) return;

						auto& mmt = common_config::Get().GetConfig();
						for (auto& a : mmt->units) {
							for (auto& m : v) {
								if (m.empty()) continue;
								if ((a.scene == m.GetScene()) && (a.key == m.GetKey()) && (a.type == m.GetType())) {
									a.value.value = m.GetValue();
									switch (m.GetType()) {
										using enum MIDI::MidiUnitType;
										case BTN:
										case BTNTOGGLE: {
											a.value.lvalue = m.GetValue() > 0U;
											break;
										}
										default: break;
									}
									break;
								}
							}
						}
					} catch (...) {}

				}, std::ref(macros_)));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void MacroGroup::add(UnitDef u) {
			macros_.push_back(std::move(u));
		}
		void MacroGroup::clear() {
			macros_.clear();
		}

		const bool MacroGroup::empty() const {
			return macros_.empty();
		}
		std::wstring MacroGroup::dump() const {
			log_string ls{};
			ls << L"Macros: ";
			for (auto& a : macros_)
				ls << a.dump() << L", ";
			return ls.str();
		}
		std::string MacroGroup::dump_s() const {
			return Utils::from_string(dump());
		}
		#pragma endregion
	}
}