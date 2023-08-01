/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIDI {

		MidiUnit MidiUnitRef::midiunitdefault__{};

		MidiUnitValue::MidiUnitValue() : value(255), time(0), lvalue(false), type(Mackie::ClickType::ClickUnknown) {}
		MidiUnitValue::MidiUnitValue(uint8_t v, uint32_t t) { value = v; time = t; lvalue = false; type = Mackie::ClickType::ClickUnknown; }
		void MidiUnitValue::Copy(MidiUnitValue& muv) {
			value = muv.value; time = muv.time; lvalue = muv.lvalue; type = muv.type;
		}

		MidiUnit::MidiUnit() : scene(255), key(255), type(MidiUnitType::UNITNONE), target(Mackie::Target::NOTARGET), longtarget(Mackie::Target::NOTARGET) {}
		const bool MidiUnit::IsEmpty() const {
			return ((scene == 255U) && (key == 255)) || (type == MidiUnitType::UNITNONE) ||
				((target == Mackie::Target::NOTARGET) || (longtarget == Mackie::Target::NOTARGET));
		}
		void MidiUnit::Copy(MidiUnit& mu) {
			key = mu.key;
			scene = mu.scene;
			type = mu.type;
			target = mu.target;
			longtarget = mu.longtarget;
			appvolume.assign(mu.appvolume.begin(), mu.appvolume.end());
			value.Copy(mu.value);
		}
		void MidiUnit::Copy(MixerUnit& mu) {
			key = mu.key;
			scene = mu.scene;
			type = mu.type;
			target = mu.target;
			longtarget = mu.longtarget;
			for (auto& s : mu.appvolume)
				appvolume.push_back(std::wstring(s.cbegin(), s.cend()));
			value.Copy(mu.value);
		}
		const uint32_t MidiUnit::GetMixerId() {
			return (scene * 1000) + key;
		}
		MixerUnit MidiUnit::GetMixerUnit() {
			MixerUnit mu{};
			mu.Copy(*this);
			return mu;
		}
		std::wstring MidiUnit::Dump() {
			std::wstringstream ws;
			for (auto& s : appvolume)
				ws << s << L", ";

			log_string ls;
			ls << L"\tscene:" << MidiHelper::GetScene(scene) << L"/" << scene << L", key:" << key
				<< L", control: |" << MidiHelper::GetType(type) << "|";

			if (target == Mackie::Target::MEDIAKEY)
				ls << L"\n\t\ttype: |" << MackieHelper::GetTarget(target)
				   << L"|, target: |" << MackieHelper::GetTarget(longtarget)
				   << L" [" << MackieHelper::GetTranslateMMKey(longtarget) << L"]|";
			else
				ls << L"\n\t\ttarget: |" << MackieHelper::GetTarget(target) << L"|"
				   << L", long press target: |" << MackieHelper::GetTarget(longtarget) << L"|";

			ls  << L"\n\t\tclick type: " << MackieHelper::GetClickType(value.type)
				<< L"\n\t\tvalue: " << static_cast<int>(value.value)
				<< L", flag value: " << Utils::BOOL_to_string(value.lvalue)
				<< L"\n\t\ttime: " << Utils::MILLISECONDS_to_string(value.time)
				<< L"\n\t\tapps: [" << ws.str().c_str() << L"]";
			return ls.str();
		}

		MidiUnitRef::MidiUnitRef() : isbegin__(false), type__(ClassTypes::ClassNone), m__(midiunitdefault__) {}
		MidiUnit& MidiUnitRef::get() {
			return m__;
		}
		void MidiUnitRef::set(MidiUnit& m, ClassTypes t) {
			m__ = m;
			type__ = t;
		}
		void MidiUnitRef::begin() {
			isbegin__ = true;
		}
		const ClassTypes MidiUnitRef::type() {
			return type__;
		}
		const bool MidiUnitRef::isbegin() {
			return isbegin__;
		}
		const bool MidiUnitRef::isvalid() {
			return isbegin__ && !m__.IsEmpty();
		}

		MixerUnit::MixerUnit() : id(0U), key(255U), scene(255U), type(UNITNONE), target(Mackie::Target::NOTARGET), longtarget(Mackie::Target::NOTARGET) {
		}
		void MixerUnit::Copy(MidiUnit& mu) {
			id = mu.GetMixerId();
			key = mu.key;
			scene = mu.scene;
			type = mu.type;
			target = mu.target;
			longtarget = mu.longtarget;
			appvolume.clear();
			for (auto& s : mu.appvolume)
				appvolume.push_back(std::wstring(s.cbegin(), s.cend()));
			value.Copy(mu.value);
		}
		void MixerUnit::Copy(MixerUnit& mu) {
			id = mu.id;
			key = mu.key;
			scene = mu.scene;
			type = mu.type;
			target = mu.target;
			longtarget = mu.longtarget;
			appvolume.assign(mu.appvolume.begin(), mu.appvolume.end());
			value.Copy(mu.value);
		}
		std::wstring MixerUnit::Dump() {
			std::wstringstream ws;
			for (auto& s : appvolume)
				ws << s << L", ";

			return (log_string() << L"\tid:" << id
				<< L"\n\t\ttype: " << MidiHelper::GetType(type)
				<< L"\n\t\ttarget type: |" << MackieHelper::GetTarget(target) << L"|"
				<< L", target: |" << MackieHelper::GetTarget(longtarget) << L"|"
				<< L"\n\t\tvalue - click type: " << MackieHelper::GetClickType(value.type)
				<< L"\n\t\tvalue - values: " << static_cast<int>(value.value)
				<< L", flag value: " << Utils::BOOL_to_string(value.lvalue)
				<< L"\n\t\tvalue - time: " << Utils::MILLISECONDS_to_string(value.time)
				<< L"\n\t\tapps: [" << ws.str() << L"]");
		}

		MidiDevice::MidiDevice() : name({}), config({}), autostart(false), manualport(false), proxy(0), btninterval(100U), btnlonginterval(1500U), jogscenefilter(true) {}
		MidiDevice::~MidiDevice() { Clear(); }
		bool MidiDevice::IsEmpty() { return units.empty(); }
		void MidiDevice::Init() { Clear(); }
		void MidiDevice::Clear() {
			if (!units.empty()) units.clear();
		}
		void MidiDevice::Add(MidiUnit mu) {
			units.push_back(mu);
		}
		MidiDevice* MidiDevice::get() {
			return this;
		}
		std::wstring MidiDevice::Dump() {
			json_config jsc;
			return jsc.Dump(this);
		}
		void MidiDevice::copysettings__(MidiDevice* cnf) {
			if (cnf == nullptr) return;
			name = std::wstring(cnf->name);
			config = std::wstring(cnf->config);
			autostart = cnf->autostart;
			manualport = cnf->manualport;
			jogscenefilter = cnf->jogscenefilter;
			proxy = cnf->proxy;
			btninterval = cnf->btninterval;
			btnlonginterval = cnf->btnlonginterval;
			mqttconf.Copy(cnf->mqttconf);
		}

		bool MidiSetter::ÑhatterButton(MidiUnit& u, Mackie::MIDIDATA& m, DWORD& t, const uint32_t& btninterval) {
			do {
				if (m.value() > 0) {
					u.value.time = t;
					break;
				}
				uint32_t ctime = (t - u.value.time);
				if (ctime < btninterval) {
					u.value.time = t;
					break;
				}
				u.value.time = t;
				return true;

			} while (0);
			return false;
		}
		void MidiSetter::SetButton(MidiUnit& u) {
			u.value.type = Mackie::ClickType::ClickOnce;
			u.value.lvalue = !u.value.lvalue;
			u.value.value = u.value.lvalue ? 127U : 0U;
		}
		bool MidiSetter::SetVolume(MidiUnit& u, DWORD& t, uint8_t val) {
			u.value.time = t;
			bool b = (u.type == MidiUnitType::SLIDERINVERT) || (u.type == MidiUnitType::FADERINVERT) || (u.type == MidiUnitType::KNOBINVERT);
			uint8_t v = b ? (127 - val) : val;
			if (u.value.value == v)
				return false;
			u.value.lvalue = b ? (val < u.value.value) : (val > u.value.value);
			u.value.value = v;
			return true;
		}
		bool MidiSetter::ValidTarget(MidiUnit& u) {
			switch (u.target) {
				using enum Mackie::Target;
				case MQTTKEY:
				case VOLUMEMIX: {
					if ((u.type == MidiUnitType::BTN) || (u.type == MidiUnitType::BTNTOGGLE)) {
						switch (u.longtarget) {
							using enum Mackie::Target;
							case B11:
							case B12:
							case B13:
							case B14:
							case B15:
							case B16:
							case B17:
							case B18:
							case B19:
							case B21:
							case B22:
							case B23:
							case B24:
							case B25:
							case B26:
							case B27:
							case B28:
							case B29:
							case B31:
							case B32:
							case B33:
							case B34:
							case B35:
							case B36:
							case B37:
							case B38:
							case B39: return true;
							default: break;
						}
					} else {
						switch (u.type) {
							using enum MidiUnitType;
							case SLIDER:
							case SLIDERINVERT:
							case FADER:
							case FADERINVERT:
							case KNOB:
							case KNOBINVERT: {
								switch (u.longtarget) {
									using enum Mackie::Target;
									case AV1:
									case AV2:
									case AV3:
									case AV4:
									case AV5:
									case AV6:
									case AV7:
									case AV8:
									case XV9:
									case AP1:
									case AP2:
									case AP3:
									case AP4:
									case AP5:
									case AP6:
									case AP7:
									case AP8:
									case XP9: return true;
									default: break;
								}
								break;
							}
							default: break;
						}
					}
					return false;
				}
				case MEDIAKEY: {
					if ((u.type == MidiUnitType::BTN) || (u.type == MidiUnitType::BTNTOGGLE)) {
						switch (u.longtarget) {
							using enum Mackie::Target;
							case SYS_Scrub:
							case SYS_Zoom:
							case SYS_Record:
							case SYS_Rewind:
							case SYS_Forward:
							case SYS_Stop:
							case SYS_Play:
							case SYS_Up:
							case SYS_Down:
							case SYS_Left:
							case SYS_Right:return true;
							default: break;
						}
					}
					return false;
				}
			}
			return false;
		}

	}
}
