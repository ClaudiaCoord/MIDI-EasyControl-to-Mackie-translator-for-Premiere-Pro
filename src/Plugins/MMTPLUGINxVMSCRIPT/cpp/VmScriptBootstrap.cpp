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
				bmod->add(chaiscript::user_type<MIDI::Mackie::Target>(), "EnumTarget");
				bmod->add(chaiscript::constructor<MIDI::Mackie::Target()>(), "EnumTarget");
				bmod->add(chaiscript::constructor<MIDI::Mackie::Target(const MIDI::Mackie::Target&)>(), "EnumTarget");
				bmod->add(chaiscript::fun([](const MIDI::Mackie::Target& mt1, const MIDI::Mackie::Target& mt2) -> bool { return mt1 == mt2; }), "==");
				bmod->add(chaiscript::fun([](const MIDI::Mackie::Target& mt, const int& i) -> bool { return enum_compare_<MIDI::Mackie::Target, int>(mt, i); }), "==");
				bmod->add(chaiscript::fun([](const MIDI::Mackie::Target& mt, const uint8_t& u) -> bool { return enum_compare_<MIDI::Mackie::Target, uint8_t>(mt, u); }), "==");
				bmod->add(chaiscript::fun([](const MIDI::Mackie::Target& mt) -> uint8_t { return static_cast<uint8_t>(mt); }), "to_int");
				bmod->add(chaiscript::fun([](const MIDI::Mackie::Target& mt) -> std::string { return Utils::from_string(MIDI::MackieHelper::GetTarget(mt).data()); }), "to_string");
				#pragma endregion

				#pragma region EnumClickType
				bmod->add(chaiscript::user_type<MIDI::Mackie::ClickType>(), "EnumClickType");
				bmod->add(chaiscript::constructor<MIDI::Mackie::ClickType()>(), "EnumClickType");
				bmod->add(chaiscript::constructor<MIDI::Mackie::ClickType(const MIDI::Mackie::ClickType&)>(), "EnumClickType");
				bmod->add(chaiscript::fun([](const MIDI::Mackie::ClickType& ct1, const MIDI::Mackie::ClickType& ct2) -> bool { return ct1 == ct2; }), "==");
				bmod->add(chaiscript::fun([](const MIDI::Mackie::ClickType& ct, const int& i) -> bool { return enum_compare_<MIDI::Mackie::ClickType, int>(ct, i); }), "==");
				bmod->add(chaiscript::fun([](const MIDI::Mackie::ClickType& ct, const uint8_t& u) -> bool { return enum_compare_<MIDI::Mackie::ClickType, uint8_t>(ct, u); }), "==");
				bmod->add(chaiscript::fun([](const MIDI::Mackie::ClickType& ct) -> uint8_t { return static_cast<uint8_t>(ct); }), "to_int");
				bmod->add(chaiscript::fun([](const MIDI::Mackie::ClickType& ct) -> std::string { return Utils::from_string(MIDI::MackieHelper::GetClickType(ct).data()); }), "to_string");
				#pragma endregion

				#pragma region EnumUnitType
				bmod->add(chaiscript::user_type<MIDI::MidiUnitType>(), "EnumUnitType");
				bmod->add(chaiscript::constructor<MIDI::MidiUnitType()>(), "EnumUnitType");
				bmod->add(chaiscript::constructor<MIDI::MidiUnitType(const MIDI::Mackie::ClickType&)>(), "EnumUnitType");
				bmod->add(chaiscript::fun([](const MIDI::MidiUnitType& ut1, const MIDI::Mackie::ClickType& ut2) -> bool { return ut1 == ut2; }), "==");
				bmod->add(chaiscript::fun([](const MIDI::MidiUnitType& ut, const int& i) -> bool { return enum_compare_<MIDI::MidiUnitType, int>(ut, i); }), "==");
				bmod->add(chaiscript::fun([](const MIDI::MidiUnitType& ut, const uint8_t& u) -> bool { return enum_compare_<MIDI::MidiUnitType, uint8_t>(ut, u); }), "==");
				bmod->add(chaiscript::fun([](const MIDI::MidiUnitType& ut) -> uint8_t { return static_cast<uint8_t>(ut); }), "to_int");
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

				#pragma region MixerUnit
				bmod->add(chaiscript::user_type<MIDI::MixerUnit>(), "MixerUnit");
				bmod->add(chaiscript::constructor<MIDI::MixerUnit()>(), "MixerUnit");
				bmod->add(chaiscript::constructor<MIDI::MixerUnit(const MIDI::MixerUnit&)>(), "MixerUnit");
				bmod->add(chaiscript::fun(&MIDI::MixerUnit::ToNull), "ToNull");
				bmod->add(chaiscript::fun(&MIDI::MixerUnit::EqualsOR), "EqualsOR");
				bmod->add(chaiscript::fun(&MIDI::MixerUnit::EqualsAND), "EqualsAND");
				bmod->add(chaiscript::fun(&MIDI::MixerUnit::dump), "dump");
				bmod->add(chaiscript::fun([](const MIDI::MixerUnit& mu) -> std::string { return Utils::from_string(mu.dump()); }), "to_string");
				#pragma endregion

				#pragma region MidiUnit
				bmod->add(chaiscript::user_type<MIDI::MidiUnit>(), "MidiUnit");
				bmod->add(chaiscript::constructor<MIDI::MidiUnit()>(), "MidiUnit");
				bmod->add(chaiscript::constructor<MIDI::MidiUnit(const MIDI::MidiUnit&)>(), "MidiUnit");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::GetMixerId), "GetMixerId");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::key), "key");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::scene), "scene");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::target), "group");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::longtarget), "target");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::value), "value");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::empty), "empty");
				bmod->add(chaiscript::fun(&MIDI::MidiUnit::dump), "dump");
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
				bmod->add(chaiscript::fun([](const JSON::MMTConfig& cnf) -> std::string { return Utils::from_string(const_cast<JSON::MMTConfig&>(cnf).dump()); }), "to_string");
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

				/* ColorGroup */
				#pragma region EnumColorIndex
				bmod->add(chaiscript::user_type<SCRIPT::ColorGroup>(), "EnumColorIndex");
				bmod->add(chaiscript::constructor<SCRIPT::ColorGroup(const SCRIPT::ColorGroup&)>(), "EnumColorIndex");
				bmod->add(chaiscript::fun([](const SCRIPT::ColorGroup& cg1, const SCRIPT::ColorGroup& cg2) -> bool { return cg1 == cg2; }), "==");
				bmod->add(chaiscript::fun([](const SCRIPT::ColorGroup& cg, const int& i) -> bool { return enum_compare_<SCRIPT::ColorGroup, int>(cg, i); }), "==");
				bmod->add(chaiscript::fun([](const SCRIPT::ColorGroup& cg, const uint8_t& u) -> bool { return enum_compare_<SCRIPT::ColorGroup, uint8_t>(cg, u); }), "==");
				bmod->add(chaiscript::fun([](const SCRIPT::ColorGroup& cg) -> uint16_t { return static_cast<uint16_t>(cg); }), "to_int");
				bmod->add(chaiscript::fun([](const SCRIPT::ColorGroup& cg) -> std::string { return Utils::from_string(ColorConstant::ColorHelper(cg)); }), "to_string");
				#pragma endregion

				/* UnitDef */
				#pragma region Unit (UnitDef)
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

				/* RGBWColor */
				#pragma region RGBW (RGBWColor)
				bmod->add(chaiscript::user_type<RGBWColor>(), "RGBW");
				bmod->add(chaiscript::constructor<RGBWColor(const RGBWColor&)>(), "RGBW");
				bmod->add(chaiscript::constructor<RGBWColor(const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&)>(), "RGBW");
				bmod->add(chaiscript::constructor<RGBWColor(const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&)>(), "RGBW");

				bmod->add(chaiscript::fun(static_cast<void(RGBWColor::*)(const uint8_t, const uint8_t, const uint8_t, const uint8_t)>(&RGBWColor::SetColor)), "SetColor");
				bmod->add(chaiscript::fun(static_cast<void(RGBWColor::*)(const uint8_t, const uint8_t, const uint8_t)>(&RGBWColor::SetColor)), "SetColor");
				bmod->add(chaiscript::fun(static_cast<void(RGBWColor::*)(const uint8_t)>(&RGBWColor::SetColor)), "SetColor");

				bmod->add(chaiscript::fun(static_cast<void(RGBWColor::*)()>(&RGBWColor::SetLight)), "SetLight");
				bmod->add(chaiscript::fun(static_cast<void(RGBWColor::*)(const uint8_t)>(&RGBWColor::SetLight)), "SetLight");
				bmod->add(chaiscript::fun(static_cast<void(RGBWColor::*)(const uint8_t, const bool)>(&RGBWColor::SetLight)), "SetLight");

				bmod->add(chaiscript::fun(static_cast<void(RGBWColor::*)(const uint8_t, const uint8_t)>(&RGBWColor::SetValues)), "SetValues");

				bmod->add(chaiscript::fun(&RGBWColor::R), "R");
				bmod->add(chaiscript::fun(&RGBWColor::G), "G");
				bmod->add(chaiscript::fun(&RGBWColor::B), "B");
				bmod->add(chaiscript::fun(&RGBWColor::W), "W");
				bmod->add(chaiscript::fun(&RGBWColor::GetColor), "GetColor");
				bmod->add(chaiscript::fun(&RGBWColor::GetLight), "GetLight");
				bmod->add(chaiscript::fun(&RGBWColor::GetGroup), "GetGroup");
				bmod->add(chaiscript::fun(&RGBWColor::ApplyValues), "ApplyValues");
				bmod->add(chaiscript::fun(&RGBWColor::UpdateValues), "UpdateValues");
				bmod->add(chaiscript::fun(&RGBWColor::empty), "empty");
				bmod->add(chaiscript::fun(&RGBWColor::dump), "dump");
				bmod->add(chaiscript::fun([](const SCRIPT::RGBWColor& c) -> std::string { return c.dump_s(); }), "to_string");
				#pragma endregion

				/* Macro */
				#pragma region Macro (MacroGroup)
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
					std::vector<uint8_t> vr{ grp, r };
					std::vector<uint8_t> vg{ grp, g };
					std::vector<uint8_t> vb{ grp, b };
					return RGBWColor(vr, vg, vb);
				}), "CreateRGB");
				bmod->add(chaiscript::fun([](const uint8_t grp, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t w) -> RGBWColor {
					std::vector<uint8_t> vr{ grp, r };
					std::vector<uint8_t> vg{ grp, g };
					std::vector<uint8_t> vb{ grp, b };
					std::vector<uint8_t> vw{ grp, w };
					return RGBWColor(vr, vg, vb, vw);
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
				eclr["RED"] = chaiscript::const_var(ColorGroup::RED);
				eclr["GREEN"] = chaiscript::const_var(ColorGroup::GREEN);
				eclr["BLUE"] = chaiscript::const_var(ColorGroup::BLUE);
				eclr["WHITE"] = chaiscript::const_var(ColorGroup::WHITE);

				cs->add_global(chaiscript::var(std::move(eclr)), "ColorIndex");

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