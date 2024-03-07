/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIDI {

		#pragma region MidiUnitValue
		MidiUnitValue::MidiUnitValue()
			: value(255), time({}), lvalue(false), type(Mackie::ClickType::ClickUnknown) {}
		MidiUnitValue::MidiUnitValue(uint8_t v, uint32_t)
			: value(v), time(std::chrono::high_resolution_clock::now()), lvalue(false), type(Mackie::ClickType::ClickUnknown) {}
		const bool MidiUnitValue::empty() const {
			return (value == 255U) && !lvalue && (type == Mackie::ClickType::ClickUnknown) && (time == time.min());
		}
		std::wstring MidiUnitValue::dump() const {
			log_string ls;
			ls << L"\tclick type: " << MackieHelper::GetClickType(type)
				<< L"\n\tvalue: " << static_cast<int>(value)
				<< L", flag value: " << Utils::BOOL_to_string(lvalue)
				<< L"\n\ttime: " << Utils::MILLISECONDS_to_string(time);
			return ls.str();
		}
		void MidiUnitValue::copy(const MidiUnitValue& m) {
			value = m.value; time = m.time; lvalue = m.lvalue; type = m.type;
		}
		#pragma endregion

		#pragma region BaseUnit
		void BaseUnit::copy(const BaseUnit& u) {
			id = u.id;
			key = u.key;
			scene = u.scene;
			type = u.type;
			target = u.target;
			longtarget = u.longtarget;
			apps.assign(
				u.apps.begin(),
				u.apps.end()
			);
			value.copy(u.value);
		}
		std::wstring BaseUnit::dump() const {
			log_delimeter ld{};
			log_string ls_apps{};
			for (auto& s : apps)
				ls_apps << ld << s;

			log_string ls;
			ls 
				<< L"\n\thash:" << hash_
				<< L"\n\tscene:" << MidiHelper::GetScene(scene) << L"/" << scene << L", key:" << key
				<< L", control: |" << MidiHelper::GetType(type) << "|";

			switch (target) {
				using enum Mackie::Target;
				case MQTTKEY:
				case MEDIAKEY:
				case VMSCRIPT:
				case VOLUMEMIX:
				case LIGHTKEY8B:
				case LIGHTKEY16B: {
					ls << L"\n\t\tgroup: ";
					break;
				}
				default: {
					ls << L"\n\t\ttarget: ";
					break;
				}
			}

			ls << L"|" << MackieHelper::GetTarget(target) << L"|";

			switch (target) {
				using enum Mackie::Target;
				case MEDIAKEY: {
					ls << L", action: |" << MackieHelper::GetTranslateMMKey(longtarget) << L"|";
					break;
				}
				case MQTTKEY:
				case LIGHTKEY8B:
				case LIGHTKEY16B: {
					ls << L", action: |" << MackieHelper::GetSmartHomeTarget(longtarget) << L"|";
					break;
				}
				case VMSCRIPT: {
					ls << L", action: |" << MackieHelper::GetScriptTarget(longtarget) << L"|";
					break;
				}
				case VOLUMEMIX: {
					ls << L", action: |" << MackieHelper::GetTarget(longtarget) << L"|";
					break;
				}
				default: {
					ls << L", long press target: |" << MackieHelper::GetTarget(longtarget) << L"|";
					break;
				}
			}

			ls << L"\n\t\tclick type: " << MackieHelper::GetClickType(value.type)
				<< L"\n\t\tvalue: " << static_cast<int>(value.value)
				<< L", flag value: " << Utils::BOOL_to_string(value.lvalue)
				<< L"\n\t\ttime: " << Utils::MILLISECONDS_to_string(value.time)
				<< L"\n\t\tapps: [" << ls_apps.str() << L"]";
			return ls.str();
		}
		const bool BaseUnit::empty() const {
			return ((scene == 255U) && (key == 255U)) || (type == MidiUnitType::UNITNONE) ||
				((target == Mackie::Target::NOTARGET) || (longtarget == Mackie::Target::NOTARGET));
		}
		#pragma endregion

		#pragma region MidiUnit
		MidiUnit::MidiUnit() {
			hash_ = Utils::random_hash(this);
		}
		MidiUnit::MidiUnit(const MidiUnit& m) {
			copy(m);
			hash_ = m.hash_;
		}

		bool MidiUnit::operator!=(const MidiUnit& m) {
			return !equals(m);
		}
		bool MidiUnit::operator==(const MidiUnit& m) {
			return equals(m);
		}

		void MidiUnit::toNull(bool b) {
			key = scene = 0U;
			target = longtarget = static_cast<Mackie::Target>(0U);
			id = static_cast<uint32_t>(b);
		}
		const bool MidiUnit::equals(const MidiUnit& u) const {
			return (scene == u.scene) && (key == u.key) && (target == u.target) && (longtarget == u.longtarget);
		}
		const bool MidiUnit::equalsOR(const MidiUnit& u) const {
			if ((key > 0U) && (u.key == key)) return true;
			else if ((scene > 0U) && (u.scene == scene)) return true;
			else if ((target > Mackie::Target::MAV) && (u.target == target)) return true;
			else if ((longtarget > Mackie::Target::MAV) && (u.longtarget == longtarget)) return true;
			return false;
		}
		const bool MidiUnit::equalsAND(const MidiUnit& u) const {
			if ((key > 0U) && (u.key != key)) return false;
			else if ((scene > 0U) && (u.scene != scene)) return false;
			else if ((target > Mackie::Target::MAV) && (u.target != target)) return false;
			else if ((longtarget > Mackie::Target::MAV) && (u.longtarget != longtarget)) return false;
			return true;
		}
		const bool MidiUnit::equalsMIDI(const MidiUnit& u) const {
			return (scene == u.scene) && (key == u.key);
		}
		const bool MidiUnit::compareSortLeft(const MidiUnit& a) const {
			return (scene < a.scene) ? true : (((scene == a.scene) && (key < a.key)) ? true : false);
		}
		const bool MidiUnit::compareSortRight(const MidiUnit& a) const {
			return (a.scene < scene) ? true : (((a.scene == scene) && (a.key < key)) ? true : false);
		}

		const uint32_t MidiUnit::getHash() const {
			return hash_;
		}
		const uint32_t MidiUnit::getMixerId() const {
			return (scene * 1000U) + key;
		}

		const bool MidiUnit::empty() const {
			return BaseUnit::empty();
		}
		void MidiUnit::copy(const MidiUnit& m) {
			BaseUnit::copy(static_cast<const BaseUnit&>(m));
		}
		std::wstring MidiUnit::dump() const {
			return BaseUnit::dump();
		}
		#pragma endregion

		#pragma region MidiUnitRef
		MidiUnitRef::MidiUnitRef() : m_(common_static::unit_empty) {}
		MidiUnitRef::MidiUnitRef(MidiUnit& m) : m_(m) {
		}

		MidiUnit& MidiUnitRef::get() const {
			return m_;
		}
		void MidiUnitRef::set(MidiUnit& m, IO::PluginClassTypes t) {
			m_ = m;
			type_ = t;
		}
		void MidiUnitRef::begin() {
			isbegin_ = true;
		}
		const IO::PluginClassTypes MidiUnitRef::type() const {
			return type_;
		}
		const bool MidiUnitRef::isbegin() const {
			return isbegin_;
		}
		const bool MidiUnitRef::isvalid() const {
			return isbegin_ && !m_.empty();
		}
		#pragma endregion

		#pragma region MidiConfig
		const bool MidiConfig::empty() const {
			return midi_in_devices.empty();
		}
		void MidiConfig::copysettings_(const MidiConfig& mc) {
			try {
				enable = mc.enable;
				out_system_port = mc.out_system_port;
				jog_scene_filter = mc.jog_scene_filter;
				out_count = mc.out_count;
				proxy_count = mc.proxy_count;
				btn_interval = mc.btn_interval;
				btn_long_interval = mc.btn_long_interval;
				midi_in_devices.assign(mc.midi_in_devices.begin(), mc.midi_in_devices.end());
				midi_out_devices.assign(mc.midi_out_devices.begin(), mc.midi_out_devices.end());
			} catch(...) {}
		}
		void MidiConfig::copy(const MidiConfig& mc) {
			copysettings_(mc);
		}
		std::wstring MidiConfig::dump() const {
			std::wstring s_in{}, s_out{};
			{
				{
					log_delimeter ld{};
					log_string ls{};
					for (auto& s : midi_in_devices)
						ls << ld << s;
					s_in = ls.str();
				}
				{
					log_delimeter ld{};
					log_string ls{};
					for (auto& s : midi_out_devices)
						ls << ld << s;
					s_out = ls.str();
				}
			}

			return (log_string() << L"\tenable module:" << Utils::BOOL_to_string(enable)
				<< L"\n\tuse system output port: " << Utils::BOOL_to_string(out_system_port)
				<< L"\n\tuse scene filter: " << Utils::BOOL_to_string(jog_scene_filter)
				<< L"\n\tMIDI Mackie count: " << out_count
				<< L"\n\tMIDI Proxy count: " << proxy_count
				<< L"\n\tbutton interval: " << btn_interval
				<< L"\n\tbutton long interval: " << btn_long_interval
				<< L"\n\tMIDI IN devices: [ " << s_in << L" ]"
				<< L"\n\tMIDI OUT devices: [ " << s_out << L" ]");
		}
		std::chrono::milliseconds MidiConfig::get_interval() const {
			return std::chrono::milliseconds(btn_interval);
		}
		std::chrono::milliseconds MidiConfig::get_long_interval() const {
			return std::chrono::milliseconds(btn_long_interval);
		}
		#pragma endregion

		#pragma region MMKeyConfig
		const bool MMKeyConfig::empty() const {
			return !enable;
		}
		void MMKeyConfig::copy(const MMKeyConfig& mc) {
			enable = mc.enable;
		}
		std::wstring MMKeyConfig::dump() const {
			return (log_string() << L"\tenable module:" << Utils::BOOL_to_string(enable));
		}
		#pragma endregion

		#pragma region MidiSetter
		bool MidiSetter::ÑhatterButton(MidiUnit& u, Mackie::MIDIDATA& m, const std::chrono::milliseconds& btninterval) {
			return ÑhatterButton(u, m, std::chrono::high_resolution_clock::now(), btninterval);
		}
		bool MidiSetter::ÑhatterButton(MidiUnit& u, Mackie::MIDIDATA& m, const std::chrono::steady_clock::time_point t, const std::chrono::milliseconds& btninterval) {
			try {
				if (m.value() > 0) {
					u.value.time = t;
					return false;
				}
				if (std::chrono::duration_cast<std::chrono::milliseconds>(t - u.value.time) < btninterval) {
					u.value.time = t;
					return false;
				}
				u.value.time = t;
				return true;

			} catch (...) {}
			return false;
		}
		void MidiSetter::SetButton(MidiUnit& u) {
			u.value.type = Mackie::ClickType::ClickOnce;
			u.value.lvalue = !u.value.lvalue;
			u.value.value = u.value.lvalue ? 127U : 0U;
		}
		bool MidiSetter::SetVolume(MidiUnit& u, uint8_t val, bool ischeck) {
			return SetVolume(u, std::chrono::high_resolution_clock::now(), val, ischeck);
		}
		bool MidiSetter::SetVolume(MidiUnit& u, const std::chrono::steady_clock::time_point t, uint8_t val, bool ischeck) {
			try {
				u.value.time = t;
				bool b = (u.type == MidiUnitType::SLIDERINVERT) || (u.type == MidiUnitType::FADERINVERT) || (u.type == MidiUnitType::KNOBINVERT);
				uint8_t v = b ? (127 - val) : val;
				if (ischeck && (u.value.value == v)) return false;
				u.value.lvalue = b ? (val < u.value.value) : (val > u.value.value);
				u.value.value = v;
				return true;

			} catch (...) {}
			return false;
		}
		bool MidiSetter::ValidTarget(MidiUnit& u) {
			switch (u.target) {
				using enum Mackie::Target;
				case LIGHTKEY8B:
				case LIGHTKEY16B: return u.longtarget > Mackie::Target::MAV; /* u.longtarget = numeric DMX device */
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
							case SYS_Right: return true;
							default: break;
						}
					}
					return false;
				}
				case VMSCRIPT: {
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
							case B39:
							case SYS_Stop:
							case SYS_Rewind: return true;
							default: break;
						}
					}
					return false;
				}
			}
			return false;
		}
		#pragma endregion
	}
}
