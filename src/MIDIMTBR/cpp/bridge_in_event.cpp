/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	EASYCTRL9

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
    namespace IO {

		using namespace std::placeholders;

		bridge_in_event::bridge_in_event(std::shared_ptr<bridge_out_event>& evout)
			: outevent_(evout), jogscenefilter_(true), wb_(Common::worker_background::Get()) {

			PluginCb::id_ = Utils::random_hash(this);
			PluginCb::type_ = PluginClassTypes::ClassIn;
			PluginCb::using_ = (PluginCbType::In1Cb | PluginCbType::In2Cb | PluginCbType::ConfCb);

			PluginCb::in1_cb_ = std::bind(static_cast<void(bridge_in_event::*)(DWORD, DWORD)>(&bridge_in_event::cbincall1_), this, _1, _2);
			PluginCb::in2_cb_ = std::bind(static_cast<void(bridge_in_event::*)(MIDI::Mackie::MIDIDATA, DWORD)>(&bridge_in_event::cbincall2_), this, _1, _2);
			PluginCb::cnf_cb_ = std::bind(static_cast<void(bridge_in_event::*)(std::shared_ptr<JSON::MMTConfig>&)>(&bridge_in_event::set_config_cb_), this, _1);
		}
		bridge_in_event::~bridge_in_event() {
		}

		DWORD bridge_in_event::get_timeid_() {
			if (tmid_ == ULONG_MAX - 1) tmid_ = 1U;
			else tmid_ += 1U;
			return static_cast<DWORD>(tmid_.load());
		}
		void bridge_in_event::set_config_cb_(std::shared_ptr<JSON::MMTConfig>& cnf) {
			try {
				if (!cnf->empty()) UseJogSceneFilter(cnf->midiconf.jog_scene_filter);
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}

		PluginCb& bridge_in_event::GetCb() {
			return *static_cast<PluginCb*>(this);
		}

		const bool bridge_in_event::IsJogSceneFilter() {
			return jogscenefilter_;
		}
		void       bridge_in_event::UseJogSceneFilter(bool b) {
			jogscenefilter_ = b;
		}

		void bridge_in_event::cbincall1_(DWORD d1, DWORD d2) {
			wb_.to_async(std::async(std::launch::async, [=](DWORD dw1, DWORD dw2) {
				cbin_(dw1, dw2);
			}, d1, d2));
		}
		void bridge_in_event::cbincall2_(MIDI::Mackie::MIDIDATA m, DWORD d2) {
			wb_.to_async(std::async(std::launch::async, [=](MIDI::Mackie::MIDIDATA m_) {
				DWORD tmid = get_timeid_();
				outevent_->CbInCall(m_, tmid);
			}, std::move(m)));
		}
		void bridge_in_event::cbin_(DWORD& d1, DWORD&) {
            try {
				MIDI::Mackie::MIDIDATA m{};

				if (jogscenefilter_) {

					uint8_t scene = (d1 & 0x000000ff);
					switch (scene) {
						using enum MIDI::MidiUnitScene;
						case SC1KNOB: // Main Rotary (scene 1)
						case SC2KNOB: // Main Rotary (scene 2)
						case SC3KNOB: // Main Rotary (scene 3)
						case SC4KNOB: // Main Rotary (scene 4)
						{
							switch (scene) {
								case SC1KNOB: { scene = SC1; break; }
								case SC2KNOB: { scene = SC2; break; }
								case SC3KNOB: { scene = SC3; break; }
								case SC4KNOB: { scene = SC4; break; }
								default: break;
							}
							m.Set(
								scene,
								static_cast<uint8_t>(10),
								static_cast<uint8_t>((d1 & 0x0000ff00) >> 8)
							);
							break;
						}
						default: {
							if (scene == SC4BTN)
								scene = SC4; // Main Rotary button left / right (scene 4)

							#pragma warning( push )
							#pragma warning( disable : 4244 )
							m.Set(
								scene,
								static_cast<uint8_t>((d1 & 0x0000ff00) >> 8),
								static_cast<uint8_t>((d1 & 0x00ff0000) >> 16)
							);
							#pragma warning( pop )
							break;
						}
					}
				} else {
					m.Set(
						static_cast<uint8_t>(d1  & 0x000000ff),
						static_cast<uint8_t>((d1 & 0x0000ff00) >> 8),
						static_cast<uint8_t>((d1 & 0x00ff0000) >> 16)
					);
				}
				DWORD tmid = get_timeid_();
				outevent_->CbInCall(std::move(m), tmid);
			} catch (...) {}
        }
	}
}