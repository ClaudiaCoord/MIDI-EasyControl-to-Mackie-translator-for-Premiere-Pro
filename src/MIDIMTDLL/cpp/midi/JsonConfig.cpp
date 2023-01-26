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

		using namespace Tiny;
		using namespace std::string_view_literals;

		constexpr wchar_t FIELD_NAME[] = L"name";
		constexpr wchar_t FIELD_CONF[] = L"config";
		constexpr wchar_t FIELD_ASTART[] = L"autostart";
		constexpr wchar_t FIELD_MPORT[] = L"manualport";
		constexpr wchar_t FIELD_JOGFILTER[] = L"jogscenefilter";
		constexpr wchar_t FIELD_PROXY[] = L"proxy";
		constexpr wchar_t FIELD_BTNINTERVAL[] = L"btninterval";
		constexpr wchar_t FIELD_BTNLONGINTERVAL[] = L"btnlonginterval";
		constexpr wchar_t FIELD_UNITS[] = L"units";
		constexpr wchar_t FIELD_SCENE[] = L"scene";
		constexpr wchar_t FIELD_ID[] = L"id";
		constexpr wchar_t FIELD_TYPE[] = L"type";
		constexpr wchar_t FIELD_TARGET[] = L"target";
		constexpr wchar_t FIELD_LONGTARGET[] = L"longtarget";
		constexpr wchar_t FIELD_APPS[] = L"apps";
		constexpr wchar_t FIELD_VALUE[] = L"value";
		constexpr wchar_t FIELD_ONOF[] = L"onoff";

		constexpr std::wstring_view R_ERR1 = L": configuration file missing or access denied - "sv;
		constexpr std::wstring_view R_ERR2 = L": configuration file is empty - "sv;
		constexpr std::wstring_view R_ERR3 = L": MIDI device name not set in ñonfiguration.."sv;
		constexpr std::wstring_view R_ERR4 = L": this configuration does not have configured MIDI keys"sv;
		constexpr std::wstring_view W_ERR1 = L": WARNING - configuration units is empty! writed this configuration will not work."sv;
		constexpr std::wstring_view W_ERR2 = L": write access to the configuration file is denied - "sv;

		bool JsonConfig::Read(MidiDevice* md, std::wstring cnfpath)
		{
			try {
				do {
					std::wstring jtxt;
					std::wifstream cnf(cnfpath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
					if (cnf.is_open()) {
						#pragma warning( push )
						#pragma warning( disable : 4244 )
						std::streampos size = cnf.tellg();
						if (size == 0U) {
							Common::to_log::Get() << (Common::log_string() << __FUNCTIONW__ << R_ERR1 << cnfpath);
							break;
						}
						size_t bsz = 1 + size;
						#pragma warning( pop )

						wchar_t* text = new wchar_t[bsz] {};
						try {
							cnf.seekg(0, std::ios::beg);
							cnf.read(text, size);
							cnf.close();
							jtxt = std::wstring(text);
						}
						catch (...) {
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						}
						delete[] text;
					} else {
						Common::to_log::Get() << (Common::log_string() << __FUNCTIONW__ << R_ERR1 << cnfpath);
						break;
					}
					if (jtxt.empty()) {
						Common::to_log::Get() << (Common::log_string() << __FUNCTIONW__ << R_ERR2 << cnfpath);
						break;
					}

					TinyJson rjson;
					rjson.ReadJson(jtxt);

					md->name = rjson.Get<std::wstring>(FIELD_NAME);
					if (md->name.empty()) {
						Common::to_log::Get() << (Common::log_string() << __FUNCTIONW__ << R_ERR3);
						break;
					}

					md->config = rjson.Get<std::wstring>(FIELD_CONF);
					if (md->config.empty())
						md->config = Utils::cofig_name(cnfpath);

					md->proxy = rjson.Get<uint32_t>(FIELD_PROXY, 0U);
					md->autostart = rjson.Get<bool>(FIELD_ASTART, false);
					md->manualport = rjson.Get<bool>(FIELD_MPORT, false);
					md->jogscenefilter = rjson.Get<bool>(FIELD_JOGFILTER, true);
					md->btninterval = rjson.Get<uint32_t>(FIELD_BTNINTERVAL, 100U);
					md->btnlonginterval = rjson.Get<uint32_t>(FIELD_BTNLONGINTERVAL, 500U);

					xarray data = rjson.Get<xarray>(FIELD_UNITS);
					size_t cnt1 = data.Count();
					if (cnt1 <= 0) {
						Common::to_log::Get() << (Common::log_string() << __FUNCTIONW__ << R_ERR4);
						break;
					}
					md->Init();

					for (size_t i = 0; i < cnt1; i++) {

						MidiUnit mu{};

						data.Enter(static_cast<int>(i));
						mu.scene = data.Get<int>(FIELD_SCENE, 0);
						mu.key = data.Get<int>(FIELD_ID);
						mu.type = static_cast<MidiUnitType>(data.Get<int>(FIELD_TYPE));
						mu.target = static_cast<Mackie::Target>(data.Get<uint16_t>(FIELD_TARGET));
						mu.longtarget = static_cast<Mackie::Target>(data.Get<uint16_t>(FIELD_LONGTARGET));
						if ((mu.key >= 255) || (mu.type >= 255U) || (mu.target > 255U))
							continue;

						mu.value.lvalue = rjson.Get<bool>(FIELD_ONOF, false);
						#pragma warning( push )
						#pragma warning( disable : 4244 )
						mu.value.value = static_cast<uint8_t>(rjson.Get<uint16_t>(FIELD_VALUE, 64U));
						#pragma warning( pop )

						try {
							xarray apps = data.Get<xarray>(FIELD_APPS);
							size_t cnt2 = apps.Count();

							if (cnt2 > 0) {
								for (size_t n = 0; n < cnt2; n++) {
									apps.Enter(static_cast<int>(n));
									std::wstring a = apps.Get<std::wstring>();
									if (!a.empty()) mu.appvolume.push_back(a);
								}
							}
						} catch (...) {}
						md->Add(std::move(mu));
					}
					Common::common_config::Get().Registry.SetConfPath(cnfpath);
					return true;
				} while (0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}

		bool JsonConfig::Write(MidiDevice* md, std::wstring cnfpath)
		{
			try {
				do {
					if ((md == nullptr) || md->name.empty()) break;

					if (md->units.empty())
						Common::to_log::Get() << (Common::log_string() << __FUNCTIONW__ << W_ERR1);

					TinyJson mjson;
					TinyJson itemsjson;
					mjson[FIELD_NAME].Set(md->name.c_str());
					mjson[FIELD_CONF].Set(md->config.c_str());
					mjson[FIELD_MPORT].Set<bool>(md->manualport);
					mjson[FIELD_ASTART].Set<bool>(md->autostart);
					mjson[FIELD_JOGFILTER].Set<bool>(md->jogscenefilter);
					mjson[FIELD_PROXY].Set<uint32_t>(md->proxy);
					mjson[FIELD_BTNINTERVAL].Set<uint32_t>(md->btninterval);
					mjson[FIELD_BTNLONGINTERVAL].Set<uint32_t>(md->btnlonginterval);

					for (auto& unit : md->units) {
						TinyJson subj;
						subj[FIELD_SCENE].Set(static_cast<uint16_t>(unit.scene));
						subj[FIELD_ID].Set(static_cast<uint16_t>(unit.key));
						subj[FIELD_TYPE].Set(static_cast<uint16_t>(unit.type));
						subj[FIELD_TARGET].Set(static_cast<uint16_t>(unit.target));
						subj[FIELD_LONGTARGET].Set(static_cast<uint16_t>(unit.longtarget));
						subj[FIELD_ONOF].Set<bool>(unit.value.lvalue);
						subj[FIELD_VALUE].Set(static_cast<uint16_t>(unit.value.value));

						if (!unit.appvolume.empty()) {
							TinyJson appjson;
							for (auto& app : unit.appvolume)
								appjson[L""].Set(app);

							TinyJson ajson;
							ajson.Push(std::move(appjson));
							subj[FIELD_APPS].Set(std::move(ajson));
						}
						itemsjson.Push(std::move(subj));
					}
					mjson[FIELD_UNITS].Set(std::move(itemsjson));

					std::wofstream cnf(cnfpath.c_str(), std::ios::trunc);
					if (!cnf.is_open()) {
						Common::to_log::Get() << (Common::log_string() << __FUNCTIONW__ << W_ERR2 << cnfpath);
						break;
					}
					cnf << mjson.WriteJson();
					cnf.close();
					Common::common_config::Get().Registry.SetConfPath(cnfpath);
					return true;
				} while (0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}

		std::wstring JsonConfig::Dump(MidiDevice* md) {
			std::wstringstream ss{};
			if ((md == nullptr) || md->name.empty())
				return L"";

			ss << L"\t" << FIELD_NAME << L"=" << Utils::to_string(md->name) << L"\n";
			ss << L"\t" << FIELD_CONF << L"=" << Utils::to_string(md->config) << L"\n";
			ss << L"\t" << FIELD_ASTART << L"=" << Utils::BOOL_to_string(md->autostart) << L"\n";
			ss << L"\t" << FIELD_MPORT << L"=" << Utils::BOOL_to_string(md->manualport) << L"\n";
			ss << L"\t" << FIELD_JOGFILTER << L"=" << Utils::BOOL_to_string(md->jogscenefilter) << L"\n";
			ss << L"\t" << FIELD_PROXY << L"=" << md->proxy << L"\n";
			ss << L"\t" << FIELD_BTNINTERVAL << L"=" << md->btninterval << L"\n";
			ss << L"\t" << FIELD_BTNLONGINTERVAL << L"=" << md->btnlonginterval << L"\n";

			for (auto& unit : md->units) {
				ss << L"\t" << FIELD_ID << L"=" << static_cast<int>(unit.key) << ", ";
				ss << FIELD_SCENE << L"=" << MidiHelper::GetScene(unit.scene) << L"/" << static_cast<int>(unit.scene) << ", ";
				ss << FIELD_TYPE << L"=" << MidiHelper::GetType(unit.type) << ", ";
				ss << FIELD_TARGET << L"=" << MackieHelper::GetTarget(unit.target) << ", ";
				ss << FIELD_LONGTARGET << L"=" << MackieHelper::GetTarget(unit.longtarget) << L"\n";
				if (!unit.appvolume.empty()) {
					ss << L"\t\t" << FIELD_APPS << L"=[";
					for (auto& app : unit.appvolume)
						ss << L"\"" << app << "\", ";
					ss << L"]";
				}
				ss << std::endl;
			}
			return ss.str().c_str();
		}
	}
}
