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

		template<typename T1, typename T2>
		inline bool enum_compare_(const T1& t1, const T2& t2) {
			return t1 == static_cast<T1>(t2);
		}

		template<typename T1, T1 ENUM_MIN, T1 ENUM_MAX>
		inline T1 enum_increment_(const T1& t) {
			const auto val = std::to_underlying(t);
			return (val > std::to_underlying(ENUM_MAX)) ? ENUM_MIN : static_cast<T1>(val + 1);
		}

		template<typename T1, T1 ENUM_MIN, T1 ENUM_MAX>
		inline T1 enum_decrement_(const T1& t) {
			const auto val = std::to_underlying(t);
			return (val > static_cast<std::underlying_type_t<T1>>(ENUM_MIN)) ? static_cast<T1>(val - 1) : ENUM_MIN;
		}

		template<typename T1, T1 ENUM_MIN, T1 ENUM_MAX>
		inline void build_enum_(std::shared_ptr<chaiscript::Module>& m, const std::string& name) {

			m->add(chaiscript::user_type<T1>(), name);
			m->add(chaiscript::constructor<T1()>(), name);
			if (std::is_enum_v<T1>)
				m->add(chaiscript::constructor<T1(const T1&)>(), name);

			m->add(chaiscript::fun([](const T1& t1, const T1& t2) -> bool { return t1 > t2; }), ">");
			m->add(chaiscript::fun([](const T1& t1, const T1& t2) -> bool { return t1 < t2; }), "<");
			m->add(chaiscript::fun([](const T1& t1, const T1& t2) -> bool { return t1 >= t2; }), ">=");
			m->add(chaiscript::fun([](const T1& t1, const T1& t2) -> bool { return t1 <= t2; }), "<=");
			m->add(chaiscript::fun([](const T1& t1, const T1& t2) -> bool { return t1 == t2; }), "==");
			m->add(chaiscript::fun([](const T1& t1, const std::underlying_type_t<T1>& ut) -> bool { return enum_compare_<T1, std::underlying_type_t<T1>>(t1, ut); }), "==");
			m->add(chaiscript::fun([](const std::underlying_type_t<T1>& ut, const T1& t1) -> bool { return enum_compare_<T1, std::underlying_type_t<T1>>(t1, ut); }), "==");

			m->add(chaiscript::fun([](T1& v, const T1& t) -> void { v = t; }), "=");
			m->add(chaiscript::fun([](T1& t) -> void {
				t = enum_increment_<T1, ENUM_MIN, ENUM_MAX>(t);
			}), "++");
			m->add(chaiscript::fun([](T1& t) -> void {
				t = enum_decrement_<T1, ENUM_MIN, ENUM_MAX>(t);
			}), "--");
			m->add(chaiscript::fun([](const T1& t) -> std::underlying_type_t<T1> { return std::to_underlying(t); }), "to_int");
		}

		ScriptEntry::ScriptEntry(const std::string& n, const std::string& b, uint32_t h, uint64_t t)
			: name(n), body(b), hash(h), time(t) {
			if (!hash) hash = static_cast<uint32_t>(std::hash<std::string>{}(n));
		}
		std::wstring ScriptEntry::namew() {
			return Utils::to_string(name);
		}

		#pragma region Scripts Bootstrap
		std::shared_ptr<chaiscript::Module> ScriptBootstrap::script_module() {
			try {
				auto bmod = std::make_shared<chaiscript::Module>();

				/* MidiUnit + MixerUnit + MidiUnitValue + MidiUnitType + Mackie::Target + Mackie::ClickType */
				#pragma region EnumTarget
				build_enum_<
					MIDI::Mackie::Target,
					MIDI::Mackie::Target::MAV,
					MIDI::Mackie::Target::SYS_Scrub>(std::ref(bmod), "EnumTarget");

				bmod->add(chaiscript::fun([](const MIDI::Mackie::Target& mt) -> std::string { return Utils::from_string(MIDI::MackieHelper::GetTarget(mt).data()); }), "to_string");
				#pragma endregion

				#pragma region EnumClickType
				build_enum_<
					MIDI::Mackie::ClickType,
					MIDI::Mackie::ClickType::ClickOnce,
					MIDI::Mackie::ClickType::ClickSlider>(std::ref(bmod), "EnumClickType");

				bmod->add(chaiscript::fun([](const MIDI::Mackie::ClickType& ct) -> std::string { return Utils::from_string(MIDI::MackieHelper::GetClickType(ct).data()); }), "to_string");
				#pragma endregion

				#pragma region EnumUnitType
				build_enum_<
					MIDI::MidiUnitType,
					MIDI::MidiUnitType::FADER,
					MIDI::MidiUnitType::SLIDERINVERT>(std::ref(bmod), "EnumUnitType");

				bmod->add(chaiscript::fun([](const MIDI::MidiUnitType& ut) -> std::string { return Utils::from_string(MIDI::MidiHelper::GetType(ut).data()); }), "to_string");
				#pragma endregion

				#pragma region UnitValue
				bmod->add(chaiscript::user_type<MIDI::MidiUnitValue>(), "UnitValue");
				bmod->add(chaiscript::constructor<MIDI::MidiUnitValue()>(), "UnitValue");
				bmod->add(chaiscript::constructor<MIDI::MidiUnitValue(const MIDI::MidiUnitValue&)>(), "UnitValue");
				bmod->add(chaiscript::fun(&MIDI::MidiUnitValue::value), "value");
				bmod->add(chaiscript::fun(&MIDI::MidiUnitValue::lvalue), "onoff");
				bmod->add(chaiscript::fun(&MIDI::MidiUnitValue::type), "type");
				bmod->add(chaiscript::fun(&MIDI::MidiUnitValue::empty), "empty");
				bmod->add(chaiscript::fun(&MIDI::MidiUnitValue::dump), "dump");
				bmod->add(chaiscript::fun([](const MIDI::MidiUnitValue& mu) -> std::string { return Utils::from_string(mu.dump()); }), "to_string");
				#pragma endregion

				#pragma region BaseUnit
				bmod->add(chaiscript::user_type<MIDI::BaseUnit>(), "BaseUnit");
				bmod->add(chaiscript::constructor<MIDI::BaseUnit()>(), "BaseUnit");
				bmod->add(chaiscript::constructor<MIDI::BaseUnit(const MIDI::BaseUnit&)>(), "BaseUnit");
				bmod->add(chaiscript::fun(&MIDI::BaseUnit::id), "id");
				bmod->add(chaiscript::fun(&MIDI::BaseUnit::key), "key");
				bmod->add(chaiscript::fun(&MIDI::BaseUnit::scene), "scene");
				bmod->add(chaiscript::fun(&MIDI::BaseUnit::target), "group");
				bmod->add(chaiscript::fun(&MIDI::BaseUnit::longtarget), "target");
				bmod->add(chaiscript::fun(&MIDI::BaseUnit::value), "value");
				bmod->add(chaiscript::fun(&MIDI::BaseUnit::type), "type");
				bmod->add(chaiscript::fun(&MIDI::BaseUnit::empty), "empty");
				bmod->add(chaiscript::fun(&MIDI::BaseUnit::dump), "dump");
				bmod->add(chaiscript::fun([](const MIDI::BaseUnit& mu) -> std::string { return Utils::from_string(mu.dump()); }), "to_string");
				#pragma endregion

				#pragma region MidiUnit
				bmod->add(chaiscript::user_type<MIDI::MidiUnit>(), "MidiUnit");
				bmod->add(chaiscript::base_class<MIDI::BaseUnit, MIDI::MidiUnit>());
				bmod->add(chaiscript::constructor<MIDI::MidiUnit()>(), "MidiUnit");
				bmod->add(chaiscript::constructor<MIDI::MidiUnit(const MIDI::MidiUnit&)>(), "MidiUnit");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::id), "id");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::key), "key");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::scene), "scene");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::target), "group");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::longtarget), "target");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::value), "value");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::type), "type");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::getMixerId), "GetMixerId");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::getHash), "GetHash");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::equalsMIDI), "EqualsGroup");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::equals), "Equals");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::empty), "empty");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::dump), "dump");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::operator==), "==");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::operator!=), "!=");
				bmod->add(chaiscript::fun([](const MIDI::MidiUnit& mu) -> std::string { return Utils::from_string(mu.dump()); }), "to_string");
				#pragma endregion

				/* JSON::MMTConfig */
				#pragma region MMTConfig (JSON::MMTConfig)
				bmod->add(chaiscript::user_type<JSON::MMTConfig>(), "MMTConfig");

				bmod->add(chaiscript::fun(static_cast<MIDI::MidiUnit&(JSON::MMTConfig::*)(const uint8_t)>(&JSON::MMTConfig::find)), "Find");
				bmod->add(chaiscript::fun(static_cast<MIDI::MidiUnit&(JSON::MMTConfig::*)(const uint8_t, const uint8_t)>(&JSON::MMTConfig::find)), "Find");
				bmod->add(chaiscript::fun(static_cast<MIDI::MidiUnit&(JSON::MMTConfig::*)(const uint8_t, const uint8_t, MIDI::MidiUnitType)>(&JSON::MMTConfig::find)), "Find");
				bmod->add(chaiscript::fun(static_cast<MIDI::MidiUnit&(JSON::MMTConfig::*)(const MIDI::Mackie::Target)>(&JSON::MMTConfig::find)), "Find");
				bmod->add(chaiscript::fun(static_cast<void(JSON::MMTConfig::*)(MIDI::MidiUnit)>(&JSON::MMTConfig::add)), "Add");

				bmod->add(chaiscript::fun(&JSON::MMTConfig::auto_start), "AutoStart");
				bmod->add(chaiscript::fun(&JSON::MMTConfig::config), "ConfigName");
				bmod->add(chaiscript::fun(&JSON::MMTConfig::builder), "BuilderVersion");

				bmod->add(chaiscript::fun(&JSON::MMTConfig::clear), "clear");
				bmod->add(chaiscript::fun(&JSON::MMTConfig::empty), "empty");
				bmod->add(chaiscript::fun(&JSON::MMTConfig::dump), "dump");
				bmod->add(chaiscript::fun([](const JSON::MMTConfig& cnf) -> std::string {
					return Utils::from_string(cnf.builder);
				}), "to_string");
				#pragma endregion

				/* MIDI::Mackie::MIDIDATA */
				#pragma region MidiData (MIDI::Mackie::MIDIDATA)
				bmod->add(chaiscript::user_type<MIDI::Mackie::MIDIDATA>(), "MidiData");
				bmod->add(chaiscript::constructor<MIDI::Mackie::MIDIDATA()>(), "MidiData");
				bmod->add(chaiscript::constructor<MIDI::Mackie::MIDIDATA(const MIDI::Mackie::MIDIDATA&)>(), "MidiData");

				bmod->add(chaiscript::fun(&MIDI::Mackie::MIDIDATA::Set), "SetData");
				bmod->add(chaiscript::fun(&MIDI::Mackie::MIDIDATA::SetValue), "SetValue");
				bmod->add(chaiscript::fun(&MIDI::Mackie::MIDIDATA::scene), "GetScene");
				bmod->add(chaiscript::fun(&MIDI::Mackie::MIDIDATA::key), "GetKey");
				bmod->add(chaiscript::fun(&MIDI::Mackie::MIDIDATA::value), "GetValue");
				bmod->add(chaiscript::fun(&MIDI::Mackie::MIDIDATA::target), "GetTarget");
				bmod->add(chaiscript::fun(&MIDI::Mackie::MIDIDATA::equals), "Equals");
				bmod->add(chaiscript::fun(&MIDI::Mackie::MIDIDATA::clear), "clear");
				bmod->add(chaiscript::fun(&MIDI::Mackie::MIDIDATA::empty), "empty");
				bmod->add(chaiscript::fun(&MIDI::Mackie::MIDIDATA::dump), "dump");
				bmod->add(chaiscript::fun([](const MIDI::Mackie::MIDIDATA& md) -> std::string { return Utils::from_string(md.dump()); }), "to_string");
				#pragma endregion

				/* W String */
				chaiscript::bootstrap::standard_library::string_type<std::wstring>("wstring", *bmod);

				/* Vectors */
				#pragma region Vectors
				chaiscript::bootstrap::standard_library::vector_type<std::vector<MIDI::MidiUnit>>("UnitVector", *bmod);
				bmod->add(chaiscript::vector_conversion<std::vector<MIDI::MidiUnit>>());
				chaiscript::bootstrap::standard_library::vector_type<std::vector<uint8_t>>("U8Vector", *bmod);
				bmod->add(chaiscript::vector_conversion<std::vector<uint8_t>>());
				chaiscript::bootstrap::standard_library::vector_type<std::vector<uint16_t>>("U16Vector", *bmod);
				bmod->add(chaiscript::vector_conversion<std::vector<uint16_t>>());
				chaiscript::bootstrap::standard_library::vector_type<std::vector<uint32_t>>("U32Vector", *bmod);
				bmod->add(chaiscript::vector_conversion<std::vector<uint32_t>>());
				chaiscript::bootstrap::standard_library::vector_type<std::vector<int32_t>>("I32Vector", *bmod);
				bmod->add(chaiscript::vector_conversion<std::vector<int32_t>>());
				chaiscript::bootstrap::standard_library::vector_type<std::vector<std::string>>("VectorString", *bmod);
				bmod->add(chaiscript::vector_conversion<std::vector<std::string>>());
				chaiscript::bootstrap::standard_library::vector_type<std::vector<std::wstring>>("VectorWstring", *bmod);
				bmod->add(chaiscript::vector_conversion<std::vector<std::wstring>>());
				#pragma endregion

				/* ColorControl */
				#pragma region base class LIGHT::UTILS::ColorControl (to EnumColorIndex, RGBWColor)
				bmod->add(chaiscript::user_type<LIGHT::UTILS::ColorControl>(), "ColorControlBase");
				bmod->add(chaiscript::constructor<LIGHT::UTILS::ColorControl(const LIGHT::UTILS::ColorControl&)>(), "ColorControlBase");
				bmod->add(chaiscript::fun([](const LIGHT::UTILS::ColorControl& c) -> std::string { return "ColorControlBase"; }), "to_string");
				#pragma endregion

				/* ColorGroup */
				#pragma region EnumColorIndex (LIGHT::UTILS::ColorControl::ColorGroup)
				build_enum_<
					LIGHT::UTILS::ColorControl::ColorGroup,
					LIGHT::UTILS::ColorControl::ColorGroup::RED,
					LIGHT::UTILS::ColorControl::ColorGroup::WHITE>(std::ref(bmod), "EnumColorIndex");
				bmod->add(chaiscript::fun([](const LIGHT::UTILS::ColorControl::ColorGroup& cg) -> std::string { return Utils::from_string(LIGHT::UTILS::ColorControl::ColorHelper(cg)); }), "to_string");
				#pragma endregion

				/* ColorsGroup */
				#pragma region EnumColorsIndex (LIGHT::UTILS::ColorControl::ColorsGroup)
				build_enum_<
					LIGHT::UTILS::ColorControl::ColorsGroup,
					LIGHT::UTILS::ColorControl::ColorsGroup::RED,
					LIGHT::UTILS::ColorControl::ColorsGroup::PURPLE>(std::ref(bmod), "EnumColorsIndex");
				bmod->add(chaiscript::fun([](const LIGHT::UTILS::ColorControl::ColorsGroup& cg) -> std::string { return Utils::from_string(LIGHT::UTILS::ColorControl::ColorHelper(cg)); }), "to_string");
				#pragma endregion

				/* UnitDef */
				#pragma region Unit (SCRIPT::UnitDef)
				bmod->add(chaiscript::user_type<SCRIPT::UnitDef>(), "Unit");
				bmod->add(chaiscript::constructor<SCRIPT::UnitDef()>(), "Unit");
				bmod->add(chaiscript::constructor<SCRIPT::UnitDef(const UnitDef&)>(), "Unit");
				bmod->add(chaiscript::constructor<SCRIPT::UnitDef(const std::vector<uint8_t>&, const uint8_t)>(), "Unit");
				bmod->add(chaiscript::constructor<SCRIPT::UnitDef(const uint8_t, const uint8_t, const uint8_t)>(), "Unit");
				bmod->add(chaiscript::constructor<SCRIPT::UnitDef(const uint8_t, const uint8_t, const uint8_t, const MIDI::MidiUnitType)>(), "Unit");

				bmod->add(chaiscript::fun(&SCRIPT::UnitDef::SetValue), "SetValue");
				bmod->add(chaiscript::fun(&SCRIPT::UnitDef::GetValue), "GetValue");
				bmod->add(chaiscript::fun(&SCRIPT::UnitDef::GetType), "GetType");
				bmod->add(chaiscript::fun(&SCRIPT::UnitDef::GetScene), "GetScene");
				bmod->add(chaiscript::fun(&SCRIPT::UnitDef::GetKey), "GetKey");
				bmod->add(chaiscript::fun(&SCRIPT::UnitDef::empty), "empty");
				bmod->add(chaiscript::fun(&SCRIPT::UnitDef::dump), "dump");
				bmod->add(chaiscript::fun([](const SCRIPT::UnitDef& u) -> std::string { return u.dump_s(); }), "to_string");
				#pragma endregion

				/* ColorCorrector */
				#pragma region ColorCorrector (SCRIPT::ColorCorrector)
				bmod->add(chaiscript::user_type<ColorCorrector>(), "ColorCorrector");
				bmod->add(chaiscript::constructor<ColorCorrector(const ColorCorrector&)>(), "ColorCorrector");
				bmod->add(chaiscript::constructor<ColorCorrector(const LIGHT::UTILS::color_rgbw_corrector_t&)>(), "ColorCorrector");
				bmod->add(chaiscript::constructor<ColorCorrector(const int8_t, const int8_t, const int8_t, const int8_t)>(), "ColorCorrector");
				bmod->add(chaiscript::fun(static_cast<LIGHT::UTILS::color_rgbw_corrector_t& (ColorCorrector::*)()>(&ColorCorrector::get)), "Get");
				bmod->add(chaiscript::fun(&ColorCorrector::empty), "empty");
				bmod->add(chaiscript::fun(&ColorCorrector::dump), "dump");
				bmod->add(chaiscript::fun([](const SCRIPT::ColorCorrector& c) -> std::string { return c.dump_s(); }), "to_string");
				#pragma endregion

				/* RGBWColor */
				#pragma region RGBW (SCRIPT::RGBWColor)
				bmod->add(chaiscript::user_type<RGBWColor>(), "RGBW");
				bmod->add(chaiscript::base_class<LIGHT::UTILS::ColorControl, RGBWColor>());
				bmod->add(chaiscript::constructor<RGBWColor(const RGBWColor&)>(), "RGBW");
				bmod->add(chaiscript::constructor<RGBWColor(const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&)>(), "RGBW");
				bmod->add(chaiscript::constructor<RGBWColor(const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&)>(), "RGBW");

				bmod->add(chaiscript::fun(static_cast<RGBWColor& (RGBWColor::*)(const uint8_t, const uint8_t, const uint8_t, const uint8_t)>(&RGBWColor::SetColor)), "SetColor");
				bmod->add(chaiscript::fun(static_cast<RGBWColor& (RGBWColor::*)(const LIGHT::UTILS::ColorControl::ColorsGroup&)>(&RGBWColor::SetColor)), "SetColor");
				bmod->add(chaiscript::fun(static_cast<RGBWColor& (RGBWColor::*)(const uint16_t, const uint8_t, const uint8_t)>(&RGBWColor::SetColor)), "SetColor");

				bmod->add(chaiscript::fun(static_cast<RGBWColor& (RGBWColor::*)(const uint16_t)>(&RGBWColor::SetHue)), "SetHue");
				bmod->add(chaiscript::fun(static_cast<RGBWColor& (RGBWColor::*)(const uint8_t)>(&RGBWColor::SetSaturation)), "SetSaturation");
				bmod->add(chaiscript::fun(static_cast<RGBWColor& (RGBWColor::*)(const uint8_t)>(&RGBWColor::SetBrightness)), "SetBrightness");

				bmod->add(chaiscript::fun(static_cast<RGBWColor& (RGBWColor::*)(ColorCorrector&)>(&RGBWColor::SetColorCorrector)), "SetColorCorrector");
				
				bmod->add(chaiscript::fun(static_cast<void(RGBWColor::*)(const size_t, const size_t, const size_t)>(&RGBWColor::FadeIn)), "FadeIn");
				bmod->add(chaiscript::fun(static_cast<void(RGBWColor::*)(const size_t, const size_t, const size_t)>(&RGBWColor::FadeOut)), "FadeOut");

				bmod->add(chaiscript::fun(&RGBWColor::R), "R");
				bmod->add(chaiscript::fun(&RGBWColor::G), "G");
				bmod->add(chaiscript::fun(&RGBWColor::B), "B");
				bmod->add(chaiscript::fun(&RGBWColor::W), "W");
				bmod->add(chaiscript::fun(&RGBWColor::On), "On");
				bmod->add(chaiscript::fun(&RGBWColor::Off), "Off");
				bmod->add(chaiscript::fun(&RGBWColor::GetColor), "GetColor");
				bmod->add(chaiscript::fun(&RGBWColor::GetHue), "GetHue");
				bmod->add(chaiscript::fun(&RGBWColor::GetSaturation), "GetSaturation");
				bmod->add(chaiscript::fun(&RGBWColor::GetBrightness), "GetBrightness");
				bmod->add(chaiscript::fun(&RGBWColor::GetGroup), "GetGroup");
				bmod->add(chaiscript::fun(&RGBWColor::ApplyValues), "ApplyValues");
				bmod->add(chaiscript::fun(&RGBWColor::UpdateValues), "UpdateValues");
				bmod->add(chaiscript::fun(&RGBWColor::empty), "empty");
				bmod->add(chaiscript::fun(&RGBWColor::dump), "dump");
				bmod->add(chaiscript::fun([](const SCRIPT::RGBWColor& c) -> std::string { return c.dump_s(); }), "to_string");
				#pragma endregion

				/* Macro */
				#pragma region Macro (SCRIPT::MacroGroup)
				bmod->add(chaiscript::user_type<MacroGroup>(), "Macro");
				bmod->add(chaiscript::constructor<MacroGroup()>(), "Macro");
				bmod->add(chaiscript::constructor<MacroGroup(const MacroGroup&)>(), "Macro");
				bmod->add(chaiscript::fun(static_cast<void(MacroGroup::*)(UnitDef)>(&MacroGroup::add)), "Add");

				bmod->add(chaiscript::fun(&MacroGroup::ApplyValues), "ApplyValues");
				bmod->add(chaiscript::fun(&MacroGroup::UpdateValues), "UpdateValues");
				bmod->add(chaiscript::fun(&MacroGroup::clear), "clear");
				bmod->add(chaiscript::fun(&MacroGroup::empty), "empty");
				bmod->add(chaiscript::fun(&MacroGroup::dump), "dump");
				bmod->add(chaiscript::fun([](const SCRIPT::MacroGroup& c) -> std::string { return c.dump_s(); }), "to_string");
				#pragma endregion

				/* callback + utils */
				#pragma region callback + utils
				bmod->add(chaiscript::fun([](const uint8_t grp, const uint8_t r, const uint8_t g, const uint8_t b) -> RGBWColor {
					return RGBWColor(
						std::vector<uint8_t>{ grp, r },
						std::vector<uint8_t>{ grp, g },
						std::vector<uint8_t>{ grp, b });
				}), "CreateRGB");
				bmod->add(chaiscript::fun([](const uint8_t grp, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t w) -> RGBWColor {
					return RGBWColor(
						std::vector<uint8_t>{ grp, r },
						std::vector<uint8_t>{ grp, g },
						std::vector<uint8_t>{ grp, b },
						std::vector<uint8_t>{ grp, w });
				}), "CreateRGBW");
				bmod->add(chaiscript::fun([](const int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }), "sleep");

				bmod->add(chaiscript::fun([](const uint8_t s, const uint8_t k, const uint8_t v) {
					ActionConstant::UpdateSlider(s, k, v);
				}), "UpdateControlSlider");
				bmod->add(chaiscript::fun([](const uint8_t s, const uint8_t k) {
					ActionConstant::UpdateButton(s, k);
				}), "UpdateControlButton");
				bmod->add(chaiscript::fun([](const SCRIPT::UnitDef& ud) -> MIDI::MidiUnit& {
					if (ud.GetType() != MIDI::MidiUnitType::UNITNONE)
						return common_config::Get().GetConfig()->find(ud.GetScene(), ud.GetKey(), ud.GetType());
					return common_config::Get().GetConfig()->find(ud.GetScene(), ud.GetKey());
				}), "FindMidiUnit");
				#pragma endregion

				return chaiscript::ModulePtr(bmod);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return std::shared_ptr<chaiscript::Module>();
		}
		void ScriptBootstrap::script_enum_unittype(std::shared_ptr<chaiscript::ChaiScript>& cs) {
			try {
				chaiscript::dispatch::Dynamic_Object eclr{};
				eclr["FADER"] = chaiscript::const_var(MIDI::MidiUnitType::FADER);
				eclr["SLIDER"] = chaiscript::const_var(MIDI::MidiUnitType::SLIDER);
				eclr["KNOB"] = chaiscript::const_var(MIDI::MidiUnitType::KNOB);
				eclr["BTN"] = chaiscript::const_var(MIDI::MidiUnitType::BTN);
				eclr["BTNTOGGLE"] = chaiscript::const_var(MIDI::MidiUnitType::BTNTOGGLE);
				eclr["KNOBINVERT"] = chaiscript::const_var(MIDI::MidiUnitType::KNOBINVERT);
				eclr["FADERINVERT"] = chaiscript::const_var(MIDI::MidiUnitType::FADERINVERT);
				eclr["SLIDERINVERT"] = chaiscript::const_var(MIDI::MidiUnitType::SLIDERINVERT);
				eclr["UNITNONE"] = chaiscript::const_var(MIDI::MidiUnitType::SLIDERINVERT);

				cs->add_global(chaiscript::var(std::move(eclr)), "UnitTypeIndex");

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void ScriptBootstrap::script_enum_clicktype(std::shared_ptr<chaiscript::ChaiScript>& cs) {
			try {
				chaiscript::dispatch::Dynamic_Object eclr{};
				eclr["ClickOnce"] = chaiscript::const_var(MIDI::Mackie::ClickType::ClickOnce);
				eclr["ClickLong"] = chaiscript::const_var(MIDI::Mackie::ClickType::ClickLong);
				eclr["ClickTrigger"] = chaiscript::const_var(MIDI::Mackie::ClickType::ClickTrigger);
				eclr["ClickSlider"] = chaiscript::const_var(MIDI::Mackie::ClickType::ClickSlider);
				eclr["ClickUnknown"] = chaiscript::const_var(MIDI::Mackie::ClickType::ClickUnknown);

				cs->add_global(chaiscript::var(std::move(eclr)), "ClickTypeIndex");

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void ScriptBootstrap::script_enum_colorgroups(std::shared_ptr<chaiscript::ChaiScript>& cs) {
			try {
				chaiscript::dispatch::Dynamic_Object eclr{};
				eclr["RED"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorGroup::RED);
				eclr["GREEN"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorGroup::GREEN);
				eclr["BLUE"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorGroup::BLUE);
				eclr["WHITE"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorGroup::WHITE);

				cs->add_global(chaiscript::var(std::move(eclr)), "ColorIndex");

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void ScriptBootstrap::script_enum_colorsgroups(std::shared_ptr<chaiscript::ChaiScript>& cs) {
			try {
				chaiscript::dispatch::Dynamic_Object eclr{};
				eclr["OFF"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::OFF);
				eclr["ON"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::ON);
				eclr["RED"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::RED);
				eclr["MAROON"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::MAROON);
				eclr["ORANGE"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::ORANGE);
				eclr["YELLOW"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::YELLOW);
				eclr["OLIVE"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::OLIVE);
				eclr["LIME"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::LIME);
				eclr["GREEN"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::GREEN);
				eclr["AQUA"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::AQUA);
				eclr["TEAL"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::TEAL);
				eclr["BLUE"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::BLUE);
				eclr["MAGENTA"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::MAGENTA);
				eclr["PURPLE"] = chaiscript::const_var(LIGHT::UTILS::ColorControl::ColorsGroup::PURPLE);

				cs->add_global(chaiscript::var(std::move(eclr)), "ColorsIndex");

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void ScriptBootstrap::script_enum_mackietarget(std::shared_ptr<chaiscript::ChaiScript>& cs) {
			try {
				chaiscript::dispatch::Dynamic_Object eclr{};
				eclr["MAV"] = chaiscript::const_var(MIDI::Mackie::Target::MAV);
				eclr["MAP"] = chaiscript::const_var(MIDI::Mackie::Target::MAP);
				eclr["MAM"] = chaiscript::const_var(MIDI::Mackie::Target::MAM);
				eclr["MAS"] = chaiscript::const_var(MIDI::Mackie::Target::MAS);
				eclr["JOG"] = chaiscript::const_var(MIDI::Mackie::Target::JOG);
				eclr["AV1"] = chaiscript::const_var(MIDI::Mackie::Target::AV1);
				eclr["AV2"] = chaiscript::const_var(MIDI::Mackie::Target::AV2);
				eclr["AV3"] = chaiscript::const_var(MIDI::Mackie::Target::AV3);
				eclr["AV4"] = chaiscript::const_var(MIDI::Mackie::Target::AV4);
				eclr["AV5"] = chaiscript::const_var(MIDI::Mackie::Target::AV5);
				eclr["AV6"] = chaiscript::const_var(MIDI::Mackie::Target::AV6);
				eclr["AV7"] = chaiscript::const_var(MIDI::Mackie::Target::AV7);
				eclr["AV8"] = chaiscript::const_var(MIDI::Mackie::Target::AV8);
				eclr["XV9"] = chaiscript::const_var(MIDI::Mackie::Target::XV9);
				eclr["AP1"] = chaiscript::const_var(MIDI::Mackie::Target::AP1);
				eclr["AP2"] = chaiscript::const_var(MIDI::Mackie::Target::AP2);
				eclr["AP3"] = chaiscript::const_var(MIDI::Mackie::Target::AP3);
				eclr["AP4"] = chaiscript::const_var(MIDI::Mackie::Target::AP4);
				eclr["AP5"] = chaiscript::const_var(MIDI::Mackie::Target::AP5);
				eclr["AP6"] = chaiscript::const_var(MIDI::Mackie::Target::AP6);
				eclr["AP7"] = chaiscript::const_var(MIDI::Mackie::Target::AP7);
				eclr["AP8"] = chaiscript::const_var(MIDI::Mackie::Target::AP8);
				eclr["XP9"] = chaiscript::const_var(MIDI::Mackie::Target::XP9);
				eclr["B11"] = chaiscript::const_var(MIDI::Mackie::Target::B11);
				eclr["B12"] = chaiscript::const_var(MIDI::Mackie::Target::B12);
				eclr["B13"] = chaiscript::const_var(MIDI::Mackie::Target::B13);
				eclr["B14"] = chaiscript::const_var(MIDI::Mackie::Target::B14);
				eclr["B15"] = chaiscript::const_var(MIDI::Mackie::Target::B15);
				eclr["B16"] = chaiscript::const_var(MIDI::Mackie::Target::B16);
				eclr["B17"] = chaiscript::const_var(MIDI::Mackie::Target::B17);
				eclr["B18"] = chaiscript::const_var(MIDI::Mackie::Target::B18);
				eclr["B19"] = chaiscript::const_var(MIDI::Mackie::Target::B19);
				eclr["B21"] = chaiscript::const_var(MIDI::Mackie::Target::B21);
				eclr["B22"] = chaiscript::const_var(MIDI::Mackie::Target::B22);
				eclr["B23"] = chaiscript::const_var(MIDI::Mackie::Target::B23);
				eclr["B24"] = chaiscript::const_var(MIDI::Mackie::Target::B24);
				eclr["B25"] = chaiscript::const_var(MIDI::Mackie::Target::B25);
				eclr["B26"] = chaiscript::const_var(MIDI::Mackie::Target::B26);
				eclr["B27"] = chaiscript::const_var(MIDI::Mackie::Target::B27);
				eclr["B28"] = chaiscript::const_var(MIDI::Mackie::Target::B28);
				eclr["B29"] = chaiscript::const_var(MIDI::Mackie::Target::B29);
				eclr["B31"] = chaiscript::const_var(MIDI::Mackie::Target::B31);
				eclr["B32"] = chaiscript::const_var(MIDI::Mackie::Target::B32);
				eclr["B33"] = chaiscript::const_var(MIDI::Mackie::Target::B33);
				eclr["B34"] = chaiscript::const_var(MIDI::Mackie::Target::B34);
				eclr["B35"] = chaiscript::const_var(MIDI::Mackie::Target::B35);
				eclr["B36"] = chaiscript::const_var(MIDI::Mackie::Target::B36);
				eclr["B37"] = chaiscript::const_var(MIDI::Mackie::Target::B37);
				eclr["B38"] = chaiscript::const_var(MIDI::Mackie::Target::B38);
				eclr["B39"] = chaiscript::const_var(MIDI::Mackie::Target::B39);
				eclr["FUN11"] = chaiscript::const_var(MIDI::Mackie::Target::FUN11);
				eclr["FUN12"] = chaiscript::const_var(MIDI::Mackie::Target::FUN12);
				eclr["FUN13"] = chaiscript::const_var(MIDI::Mackie::Target::FUN13);
				eclr["FUN14"] = chaiscript::const_var(MIDI::Mackie::Target::FUN14);
				eclr["FUN15"] = chaiscript::const_var(MIDI::Mackie::Target::FUN15);
				eclr["FUN16"] = chaiscript::const_var(MIDI::Mackie::Target::FUN16);
				eclr["FUN17"] = chaiscript::const_var(MIDI::Mackie::Target::FUN17);
				eclr["FUN18"] = chaiscript::const_var(MIDI::Mackie::Target::FUN18);
				eclr["FUN21"] = chaiscript::const_var(MIDI::Mackie::Target::FUN21);
				eclr["FUN22"] = chaiscript::const_var(MIDI::Mackie::Target::FUN22);
				eclr["FUN23"] = chaiscript::const_var(MIDI::Mackie::Target::FUN23);
				eclr["FUN24"] = chaiscript::const_var(MIDI::Mackie::Target::FUN24);
				eclr["FUN25"] = chaiscript::const_var(MIDI::Mackie::Target::FUN25);
				eclr["FUN26"] = chaiscript::const_var(MIDI::Mackie::Target::FUN26);
				eclr["FUN27"] = chaiscript::const_var(MIDI::Mackie::Target::FUN27);
				eclr["FUN28"] = chaiscript::const_var(MIDI::Mackie::Target::FUN28);
				eclr["SYS_Rewind"] = chaiscript::const_var(MIDI::Mackie::Target::SYS_Rewind);
				eclr["SYS_Forward"] = chaiscript::const_var(MIDI::Mackie::Target::SYS_Forward);
				eclr["SYS_Stop"] = chaiscript::const_var(MIDI::Mackie::Target::SYS_Stop);
				eclr["SYS_Play"] = chaiscript::const_var(MIDI::Mackie::Target::SYS_Play);
				eclr["SYS_Record"] = chaiscript::const_var(MIDI::Mackie::Target::SYS_Record);
				eclr["SYS_Up"] = chaiscript::const_var(MIDI::Mackie::Target::SYS_Up);
				eclr["SYS_Down"] = chaiscript::const_var(MIDI::Mackie::Target::SYS_Down);
				eclr["SYS_Left"] = chaiscript::const_var(MIDI::Mackie::Target::SYS_Left);
				eclr["SYS_Right"] = chaiscript::const_var(MIDI::Mackie::Target::SYS_Right);
				eclr["SYS_Zoom"] = chaiscript::const_var(MIDI::Mackie::Target::SYS_Zoom);
				eclr["SYS_Scrub"] = chaiscript::const_var(MIDI::Mackie::Target::SYS_Scrub);
				eclr["VMSCRIPT"] = chaiscript::const_var(MIDI::Mackie::Target::VMSCRIPT);
				eclr["LIGHTKEY16B"] = chaiscript::const_var(MIDI::Mackie::Target::LIGHTKEY16B);
				eclr["LIGHTKEY8B"] = chaiscript::const_var(MIDI::Mackie::Target::LIGHTKEY8B);
				eclr["MQTTKEY"] = chaiscript::const_var(MIDI::Mackie::Target::MQTTKEY);
				eclr["MEDIAKEY"] = chaiscript::const_var(MIDI::Mackie::Target::MEDIAKEY);
				eclr["VOLUMEMIX"] = chaiscript::const_var(MIDI::Mackie::Target::VOLUMEMIX);
				eclr["NOTARGET"] = chaiscript::const_var(MIDI::Mackie::Target::NOTARGET);

				cs->add_global(chaiscript::var(std::move(eclr)), "TargetIndex");

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion
	}
}