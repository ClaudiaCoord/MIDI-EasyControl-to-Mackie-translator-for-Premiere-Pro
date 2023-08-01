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

		constexpr wchar_t FIELD_MQTT[] = L"mqtt";
		constexpr wchar_t FIELD_MQTT_HOST[] = L"host";
		constexpr wchar_t FIELD_MQTT_PORT[] = L"port";
		constexpr wchar_t FIELD_MQTT_LOGIN[] = L"login";
		constexpr wchar_t FIELD_MQTT_PASS[] = L"pass";
		constexpr wchar_t FIELD_MQTT_SSLPSK[] = L"sslpsk";
		constexpr wchar_t FIELD_MQTT_ISSSL[] = L"isssl";
		constexpr wchar_t FIELD_MQTT_PREF[] = L"prefix";
		constexpr wchar_t FIELD_MQTT_CAPATH[] = L"certcapath";
		constexpr wchar_t FIELD_MQTT_SSIGN[] = L"selfsigned";
		constexpr wchar_t FIELD_MQTT_LLEVEL[] = L"loglevel";
		
		bool json_config::Read(MidiDevice* md, std::wstring cnfpath)
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
							to_log::Get() << log_string().to_log_fomat(
								__FUNCTIONW__,
								common_error_code::Get().get_error(common_error_id::err_JSONCONF_READ_ACCESS),
								cnfpath
							);
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
						to_log::Get() << log_string().to_log_fomat(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_JSONCONF_READ_ACCESS),
							cnfpath
						);
						break;
					}
					if (jtxt.empty()) {
						to_log::Get() << log_string().to_log_fomat(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_JSONCONF_EMPTY),
							cnfpath
						);
						break;
					}

					TinyJson rjson;
					rjson.ReadJson(jtxt);

					md->name = rjson.Get<std::wstring>(FIELD_NAME);
					if (md->name.empty()) {
						to_log::Get() << log_string().to_log_string(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_JSONCONF_NO_DEVICE)
						);
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

					xobject mdata = rjson.Get<xobject>(FIELD_MQTT);
					if (mdata.Count() > 0) {
						mdata.Enter(0);
						md->mqttconf.host = mdata.Get<std::wstring>(FIELD_MQTT_HOST);
						md->mqttconf.login = mdata.Get<std::wstring>(FIELD_MQTT_LOGIN);
						md->mqttconf.password = mdata.Get<std::wstring>(FIELD_MQTT_PASS);
						md->mqttconf.mqttprefix = mdata.Get<std::wstring>(FIELD_MQTT_PREF);
						md->mqttconf.sslpsk = mdata.Get<std::wstring>(FIELD_MQTT_SSLPSK);
						md->mqttconf.certcapath = mdata.Get<std::wstring>(FIELD_MQTT_CAPATH);
						md->mqttconf.port = mdata.Get<uint32_t>(FIELD_MQTT_PORT, 0);
						md->mqttconf.loglevel = mdata.Get<int32_t>(FIELD_MQTT_LLEVEL, 0);
						md->mqttconf.isssl = mdata.Get<bool>(FIELD_MQTT_ISSSL, true);
						md->mqttconf.isselfsigned = mdata.Get<bool>(FIELD_MQTT_SSIGN, true);
					}

					xarray cdata = rjson.Get<xarray>(FIELD_UNITS);
					size_t cnt1 = cdata.Count();
					if (cnt1 <= 0) {
						to_log::Get() << log_string().to_log_string(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_JSONCONF_EMPTY_KEYS)
						);
						break;
					}
					md->Init();

					for (size_t i = 0; i < cnt1; i++) {

						MidiUnit mu{};

						cdata.Enter(static_cast<int>(i));
						mu.scene = cdata.Get<int>(FIELD_SCENE, 0);
						mu.key = cdata.Get<int>(FIELD_ID);
						mu.type = static_cast<MidiUnitType>(cdata.Get<int>(FIELD_TYPE));
						mu.target = static_cast<Mackie::Target>(cdata.Get<uint16_t>(FIELD_TARGET));
						mu.longtarget = static_cast<Mackie::Target>(cdata.Get<uint16_t>(FIELD_LONGTARGET));
						if ((mu.key >= 255) || (mu.type >= 255U) || (mu.target > 255U))
							continue;

						mu.value.lvalue = rjson.Get<bool>(FIELD_ONOF, false);
						#pragma warning( push )
						#pragma warning( disable : 4244 )
						mu.value.value = static_cast<uint8_t>(rjson.Get<uint16_t>(FIELD_VALUE, 64U));
						#pragma warning( pop )

						try {
							xarray apps = cdata.Get<xarray>(FIELD_APPS);
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

		bool json_config::Write(MidiDevice* md, std::wstring cnfpath, bool issetreg)
		{
			try {
				do {
					if ((md == nullptr) || md->name.empty()) break;

					if (md->units.empty())
						to_log::Get() << log_string().to_log_string(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_JSONCONF_WRITE_EMPTY)
						);

					TinyJson rjson;
					TinyJson itemsjson;
					rjson[FIELD_NAME].Set(md->name.c_str());
					rjson[FIELD_CONF].Set(md->config.c_str());
					rjson[FIELD_MPORT].Set<bool>(md->manualport);
					rjson[FIELD_ASTART].Set<bool>(md->autostart);
					rjson[FIELD_JOGFILTER].Set<bool>(md->jogscenefilter);
					rjson[FIELD_PROXY].Set<uint32_t>(md->proxy);
					rjson[FIELD_BTNINTERVAL].Set<uint32_t>(md->btninterval);
					rjson[FIELD_BTNLONGINTERVAL].Set<uint32_t>(md->btnlonginterval);

					if (!md->mqttconf.empty()) {
						TinyJson mjson;
						mjson[FIELD_MQTT_HOST].Set(md->mqttconf.host);
						mjson[FIELD_MQTT_LOGIN].Set(md->mqttconf.login);
						mjson[FIELD_MQTT_PASS].Set(md->mqttconf.password);
						mjson[FIELD_MQTT_PREF].Set(md->mqttconf.mqttprefix);
						mjson[FIELD_MQTT_SSLPSK].Set(md->mqttconf.sslpsk);
						mjson[FIELD_MQTT_CAPATH].Set(md->mqttconf.certcapath);
						mjson[FIELD_MQTT_PORT].Set<uint32_t>(md->mqttconf.port);
						mjson[FIELD_MQTT_LLEVEL].Set<int32_t>(md->mqttconf.loglevel);
						mjson[FIELD_MQTT_ISSSL].Set<bool>(md->mqttconf.isssl);
						mjson[FIELD_MQTT_SSIGN].Set<bool>(md->mqttconf.isselfsigned);
						rjson[FIELD_MQTT].Set(std::move(mjson));
					}

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
					rjson[FIELD_UNITS].Set(std::move(itemsjson));

					std::wofstream cnf(cnfpath.c_str(), std::ios::trunc);
					if (!cnf.is_open()) {
						to_log::Get() << log_string().to_log_fomat(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_JSONCONF_WRITE_ACCESS),
							cnfpath
						);
						break;
					}
					cnf << rjson.WriteJson();
					cnf.close();
					if (issetreg)
						common_config::Get().Registry.SetConfPath(cnfpath);
					return true;
				} while (0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}

		std::wstring json_config::Dump(MidiDevice* md) {
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

			ss << L"\t" << FIELD_MQTT << L" -> " << FIELD_MQTT_HOST << L"=" << Utils::to_string(md->mqttconf.host) << L"\n";
			ss << L"\t" << FIELD_MQTT << L" -> " << FIELD_MQTT_PORT << L"=" << md->mqttconf.port << L"\n";
			ss << L"\t" << FIELD_MQTT << L" -> " << FIELD_MQTT_LOGIN << L"=" << Utils::to_string(md->mqttconf.login) << L"\n";
			ss << L"\t" << FIELD_MQTT << L" -> " << FIELD_MQTT_PASS << L"=" << Utils::to_string(md->mqttconf.password) << L"\n";
			ss << L"\t" << FIELD_MQTT << L" -> " << FIELD_MQTT_PREF << L"=" << Utils::to_string(md->mqttconf.mqttprefix) << L"\n";
			ss << L"\t" << FIELD_MQTT << L" -> " << FIELD_MQTT_SSLPSK << L"=" << Utils::to_string(md->mqttconf.sslpsk) << L"\n";
			ss << L"\t" << FIELD_MQTT << L" -> " << FIELD_MQTT_ISSSL << L"=" << Utils::BOOL_to_string(md->mqttconf.isssl) << L"\n";
			ss << L"\t" << FIELD_MQTT << L" -> " << FIELD_MQTT_LLEVEL << L"=" << md->mqttconf.loglevel << L"\n";

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
