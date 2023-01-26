/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	SMARTHOME DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MQTT {

		using namespace std::placeholders;
		using namespace std::string_view_literals;

		constexpr std::wstring_view strStartOk = L": The Smart-Home control service is running."sv;
		constexpr std::wstring_view strStopOk = L": The Smart-Home control service has stopped."sv;

		static SmartHome smarthome__;

		SmartHome::SmartHome() {
			type__ = Common::MIDI::ClassTypes::ClassMqttKey;
			out1__ = [](Common::MIDI::Mackie::MIDIDATA&, DWORD&) { return false; };
			out2__ = std::bind(static_cast<const bool(SmartHome::*)(Common::MIDI::MidiUnit&, DWORD&)>(&SmartHome::InCallBack), this, _1, _2);
			id__   = Utils::random_hash(this);
			broker__ = std::make_unique<Broker>();
		}
		SmartHome::~SmartHome() {
			broker__.reset();
		}

		bool SmartHome::Start() {
			try {
				common_config& cnf = common_config::Get();
				auto& config = cnf.GetConfig();
				if (broker__->init(config->mqttconf)) {
					cnf.Local.IsSmartHomeRun(true);
					to_log::Get() << (log_string() << __FUNCTIONW__ << strStartOk);
				}
				return broker__->isrunning();
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		void SmartHome::Stop() {
			try {
				broker__->release();
				common_config::Get().Local.IsSmartHomeRun(false);
				to_log::Get() << (log_string() << __FUNCTIONW__ << strStopOk);
			}
			catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		SmartHome& SmartHome::Get() {
			return std::ref(smarthome__);
		}
		void SmartHome::SetTitle(std::vector<MIDI::Mackie::Target>& v) {
			if (!broker__->isrunning()) return;
			broker__->settitle(v);
		}

		/* Private */

		const bool SmartHome::InCallBack(Common::MIDI::MidiUnit& m, DWORD& d) {
			try {
				if (d <= time__.load()) return false;
				if (!broker__->isrunning() || (m.target != MIDI::Mackie::Target::MQTTKEY))
					return false;
				time__ = d;

				switch (m.type) {
					case Common::MIDI::MidiUnitType::BTN:
					case Common::MIDI::MidiUnitType::BTNTOGGLE: {
						broker__->send<bool>(m.value.lvalue, m.longtarget);
						return true;
					}
					case Common::MIDI::MidiUnitType::FADER:
					case Common::MIDI::MidiUnitType::FADERINVERT:
					case Common::MIDI::MidiUnitType::SLIDER:
					case Common::MIDI::MidiUnitType::SLIDERINVERT:
					case Common::MIDI::MidiUnitType::KNOB:
					case Common::MIDI::MidiUnitType::KNOBINVERT: {
						broker__->send<uint32_t>(static_cast<uint32_t>(m.value.value), m.longtarget);
						return true;
					}
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
	}
}
