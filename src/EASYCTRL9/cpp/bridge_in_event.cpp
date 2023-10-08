/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	EASYCTRL9

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
using namespace std::placeholders;

namespace Common {
    namespace MIDI {

		bridge_in_event::bridge_in_event(std::unique_ptr<bridge_out_event>& evout)
			: outevent__(evout), jogscenefilter__(true), wb__(Common::worker_background::Get()) {
		}
        bridge_in_event::~bridge_in_event() {
		}

		const bool bridge_in_event::IsJogSceneFilter() {
			return jogscenefilter__;
		}
		void       bridge_in_event::IsJogSceneFilter(bool b) {
			jogscenefilter__ = b;
		}

		void bridge_in_event::add(MidiInstance& mi) {
			mi.InCallbackSet(
                std::bind(&bridge_in_event::cbInCall, this, _1, _2)
            );
        }
		void bridge_in_event::add(MidiControllerBase& clz) {
			clz.InCallbackSet(
				std::bind(&bridge_in_event::cbInCall, this, _1, _2)
			);
		}
		void bridge_in_event::add_ptr(MidiInstance* mi) {
			if (!mi) return;
			mi->InCallbackSet(
				std::bind(&bridge_in_event::cbInCall, this, _1, _2)
			);
		}
		void bridge_in_event::add_ptr(MidiControllerBase* clz) {
			if (!clz) return;
			clz->InCallbackSet(
				std::bind(&bridge_in_event::cbInCall, this, _1, _2)
			);
		}

		void bridge_in_event::remove(MidiControllerBase& clz) {
            clz.InCallbackRemove();
        }
		void bridge_in_event::remove(MidiInstance& mi) {
			mi.InCallbackRemove();
		}
		void bridge_in_event::remove_ptr(MidiControllerBase* clz) {
			if (!clz) return;
			clz->InCallbackRemove();
		}
		void bridge_in_event::remove_ptr(MidiInstance* mi) {
			if (!mi) return;
			mi->InCallbackRemove();
		}

		void bridge_in_event::cbInCall(DWORD d1, DWORD d2) {
			wb__.to_async(std::async(std::launch::async, [=](DWORD dw1, DWORD dw2) {
				cbIn(dw1, dw2);
				}, d1, d2));
		}
        void bridge_in_event::cbIn(DWORD& d1, DWORD& d2) {
            try {
				Mackie::MIDIDATA m{};

				if (jogscenefilter__) {

					uint8_t scene = (d1 & 0x000000ff);
					switch (scene) {
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
				outevent__->CallCbOut(m, d2);
            } catch (...) {}
        }
	}
}