/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMMT DLL - main config read/write

	See README.cnf for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include "..\..\Common\rc\resource_version.h"

namespace Common {
	namespace JSON {

		using namespace Tiny;
		using namespace std::string_view_literals;

		class JsonNames {
		public:
			#pragma region JsonNames
			static constexpr std::wstring_view EMPTY = L""sv;
			static constexpr std::wstring_view NAME = L"name"sv;
			static constexpr std::wstring_view HOST = L"host"sv;
			static constexpr std::wstring_view PORT = L"port"sv;
			static constexpr std::wstring_view CONF = L"config"sv;
			static constexpr std::wstring_view MASK = L"mask"sv;
			static constexpr std::wstring_view UNITS = L"units"sv;
			static constexpr std::wstring_view SCENE = L"scene"sv;
			static constexpr std::wstring_view ASTART = L"autostart"sv;
			static constexpr std::wstring_view ENABLE = L"enable"sv;
			static constexpr std::wstring_view BUILDER = L"build"sv;
			static constexpr std::wstring_view ID = L"id"sv;
			static constexpr std::wstring_view IP = L"ip"sv;
			static constexpr std::wstring_view TYPE = L"type"sv;
			static constexpr std::wstring_view TARGET = L"target"sv;
			static constexpr std::wstring_view LONGTARGET = L"longtarget"sv;
			static constexpr std::wstring_view APPS = L"apps"sv;
			static constexpr std::wstring_view VALUE = L"value"sv;
			static constexpr std::wstring_view ONOF = L"onoff"sv;

			static constexpr std::wstring_view MIDI = L"midi"sv;
			static constexpr std::wstring_view MIDICTRL = L"midictrl"sv;
			static constexpr std::wstring_view OUT_COUNT = L"out"sv;
			static constexpr std::wstring_view PROXY_COUNT = L"proxy"sv;
			static constexpr std::wstring_view OUTPORT_TYPE = L"outsystemport"sv;
			static constexpr std::wstring_view JOGFILTER = L"jogscenefilter"sv;
			static constexpr std::wstring_view BTNINTERVAL = L"btninterval"sv;
			static constexpr std::wstring_view BTNLONGINTERVAL = L"btnlonginterval"sv;
			static constexpr std::wstring_view MIDI_DEV_IN = L"midiin"sv;
			static constexpr std::wstring_view MIDI_DEV_OUT = L"midiout"sv;

			static constexpr std::wstring_view MMKEY = L"mmkey"sv;

			static constexpr std::wstring_view MQTT = L"mqtt"sv;
			static constexpr std::wstring_view MQTT_LOGIN = L"login"sv;
			static constexpr std::wstring_view MQTT_PASS = L"pass"sv;
			static constexpr std::wstring_view MQTT_SSLPSK = L"sslpsk"sv;
			static constexpr std::wstring_view MQTT_ISSSL = L"isssl"sv;
			static constexpr std::wstring_view MQTT_PREF = L"prefix"sv;
			static constexpr std::wstring_view MQTT_CAPATH = L"certcapath"sv;
			static constexpr std::wstring_view MQTT_SSIGN = L"selfsigned"sv;
			static constexpr std::wstring_view MQTT_LLEVEL = L"loglevel"sv;

			static constexpr std::wstring_view LIGHTS = L"lights"sv;
			static constexpr std::wstring_view LIGHTS_DMX = L"dmx"sv;
			static constexpr std::wstring_view LIGHTS_POOL = L"pool"sv;
			static constexpr std::wstring_view LIGHTS_DMX_BAUDRATE = L"baudrate"sv;
			static constexpr std::wstring_view LIGHTS_DMX_STOPBITS = L"stopbits"sv;
			static constexpr std::wstring_view LIGHTS_DMX_TIMEOUT = L"timeout"sv;

			static constexpr std::wstring_view LIGHTS_ARTNET = L"artnet"sv;
			static constexpr std::wstring_view LIGHTS_ARTNET_UNIVERSE = L"universe"sv;
			static constexpr std::wstring_view LIGHTS_ARTNET_BROADCAST = L"broadcast"sv;
			#pragma endregion
		};

		using JSON::JsonNames;

		json_config::json_config() {}

		#pragma region Read
		const bool json_config::Read(MMTConfig* mmt, std::wstring filepath, bool issetreg) {
			try {
				return ReadFile(filepath,
					[=](Tiny::TinyJson& pjson, std::wstring& confname) -> bool {
						if (!ReadUnitConfig(pjson, mmt, confname)) return false;
						ReadMidiConfig(pjson, mmt->midiconf);
						ReadMqttConfig(pjson, mmt->mqttconf);
						ReadMMkeyConfig(pjson, mmt->mmkeyconf);
						ReadLightConfig(pjson, mmt->lightconf);

						if (issetreg)
							common_config::Get().Registry.SetConfPath(confname);

						return true;
					}
				);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		const bool json_config::ReadFile(std::wstring& filepath, std::function<bool(Tiny::TinyJson&, std::wstring&)> f) {
			try {
				std::wstring txt;
				std::wifstream cnf(filepath, std::ios::in | std::ios::binary | std::ios::ate);
				if (cnf.is_open()) {
					#pragma warning( push )
					#pragma warning( disable : 4244 )
					std::streampos size = cnf.tellg();
					if (size == 0U) {
						to_log::Get() << log_string().to_log_format(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_JSONCONF_READ_ACCESS),
							filepath
						);
						return false;
					}
					size_t bsz = 1 + size;
					#pragma warning( pop )

					wchar_t* text = new wchar_t[bsz] {};
					try {
						cnf.seekg(0, std::ios::beg);
						cnf.read(text, size);
						cnf.close();
						txt = std::wstring(text);
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
					delete[] text;
				} else {
					to_log::Get() << log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_JSONCONF_READ_ACCESS),
						filepath
					);
					return false;
				}
				if (txt.empty()) {
					to_log::Get() << log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_JSONCONF_EMPTY),
						filepath
					);
					return false;
				}

				Tiny::TinyJson pjson{};
				pjson.ReadJson(txt);
				return f(pjson, filepath);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}

		const bool json_config::ReadUnitConfig(Tiny::TinyJson& root, MMTConfig* mmt, std::wstring& filepath) {
			try {
				{
					Tiny::xobject cdata = root.Get<Tiny::xobject>(JsonNames::CONF.data());
					if (!cdata.Count()) {
						to_log::Get() << log_string().to_log_string(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_CONFIG_EMPTY)
						);
						return false;
					}

					cdata.Enter();

					mmt->config = cdata.Get<std::wstring>(JsonNames::NAME.data());
					if (mmt->config.empty())
						mmt->config = Utils::cofig_name(filepath);

					mmt->auto_start = cdata.Get<bool>(JsonNames::ASTART.data(), false);
					mmt->builder = cdata.Get<std::wstring>(JsonNames::BUILDER.data());
				}

				Tiny::xarray mdata = root.Get<Tiny::xarray>(JsonNames::UNITS.data());
				size_t cnt = mdata.Count();
				if (!cnt) {
					to_log::Get() << log_string().to_log_string(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_JSONCONF_EMPTY_KEYS)
					);
					return false;
				}

				mmt->Init();

				for (uint16_t i = 0U; i < cnt; i++) {

					MIDI::MidiUnit mu{};

					mdata.Enter(i);
					mu.scene = mdata.Get<int>(JsonNames::SCENE.data(), 0);
					mu.key = mdata.Get<int>(JsonNames::ID.data());
					mu.type = static_cast<MIDI::MidiUnitType>(mdata.Get<int>(JsonNames::TYPE.data()));
					mu.target = static_cast<MIDI::Mackie::Target>(mdata.Get<uint16_t>(JsonNames::TARGET.data()));
					mu.longtarget = static_cast<MIDI::Mackie::Target>(mdata.Get<uint16_t>(JsonNames::LONGTARGET.data()));

					if ((mu.key >= 255) || (mu.type >= 255U) || (mu.target > 255U)) continue;

					mu.value.lvalue = mdata.Get<bool>(JsonNames::ONOF.data(), false);
					#pragma warning( push )
					#pragma warning( disable : 4244 )
					mu.value.value = static_cast<uint8_t>(mdata.Get<uint16_t>(JsonNames::VALUE.data(), 64U));
					#pragma warning( pop )

					try {
						xarray apps = mdata.Get<xarray>(JsonNames::APPS.data());
						size_t acnt = apps.Count();

						if (acnt) {
							for (uint16_t n = 0; n < acnt; n++) {
								apps.Enter(n);
								std::wstring a = apps.Get<std::wstring>();
								if (!a.empty()) mu.appvolume.push_back(a);
							}
						}
					} catch (...) {}
					mmt->Add(std::move(mu));
				}
				return (!mmt->units.empty());
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		void json_config::ReadMidiConfig(Tiny::TinyJson& root, MIDI::MidiConfig& cnf) {
			try {
				Tiny::xobject pdata = root.Get<Tiny::xobject>(JsonNames::MIDI.data());
				if (!pdata.Count()) return;

				pdata.Enter();

				Tiny::xobject mdata = pdata.Get<Tiny::xobject>(JsonNames::MIDICTRL.data());
				if (mdata.Count()) {

					mdata.Enter();

					cnf.enable = mdata.Get<bool>(JsonNames::ENABLE.data(), false);
					cnf.out_count = mdata.Get<uint32_t>(JsonNames::OUT_COUNT.data(), 1U);
					cnf.proxy_count = mdata.Get<uint32_t>(JsonNames::PROXY_COUNT.data(), 0U);
					cnf.out_system_port = mdata.Get<bool>(JsonNames::OUTPORT_TYPE.data(), false);
					cnf.jog_scene_filter = mdata.Get<bool>(JsonNames::JOGFILTER.data(), true);
					cnf.btn_interval = mdata.Get<uint32_t>(JsonNames::BTNINTERVAL.data(), 100U);
					cnf.btn_long_interval = mdata.Get<uint32_t>(JsonNames::BTNLONGINTERVAL.data(), 500U);
				}

				try {
					Tiny::xobject xdata = pdata.Get<Tiny::xobject>(JsonNames::MIDI_DEV_IN.data());
					if (xdata.Count()) {

						xdata.Enter();
						if (xdata.Get<bool>(JsonNames::ENABLE.data(), false)) {

							Tiny::xarray dev = xdata.Get<Tiny::xarray>(JsonNames::PORT.data());
							std::size_t cnt = dev.Count();

							if (cnt) {
								for (uint16_t n = 0; n < cnt; n++) {
									dev.Enter(n);
									std::wstring s = dev.Get<std::wstring>();
									if (!s.empty()) cnf.midi_in_devices.push_back(s);
								}
							}
						}
					}
				} catch (...) {}
				try {
					Tiny::xobject xdata = pdata.Get<Tiny::xobject>(JsonNames::MIDI_DEV_OUT.data());
					if (xdata.Count()) {

						xdata.Enter();
						if (xdata.Get<bool>(JsonNames::ENABLE.data(), false)) {

							Tiny::xarray dev = xdata.Get<Tiny::xarray>(JsonNames::PORT.data());
							std::size_t cnt = dev.Count();

							if (cnt) {
								for (uint16_t n = 0; n < cnt; n++) {
									dev.Enter(n);
									std::wstring s = dev.Get<std::wstring>();
									if (!s.empty()) cnf.midi_out_devices.push_back(s);
								}
							}
						}
					}
				} catch (...) {}

				if (cnf.empty())
					to_log::Get() << log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_CONFIG_EMPTY_NAME),
						JsonNames::MIDICTRL.data()
					);

				if (cnf.midi_in_devices.empty())
					to_log::Get() << log_string().to_log_string(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_JSONCONF_NO_DEVICE)
					);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void json_config::ReadMqttConfig(Tiny::TinyJson& root, MQTT::BrokerConfig<std::wstring>& cnf) {
			try {
				Tiny::xobject mdata = root.Get<Tiny::xobject>(JsonNames::MQTT.data());
				if (!mdata.Count()) return;

				mdata.Enter();

				cnf.enable = mdata.Get<bool>(JsonNames::ENABLE.data(), false);
				cnf.host = mdata.Get<std::wstring>(JsonNames::HOST.data());
				cnf.login = mdata.Get<std::wstring>(JsonNames::MQTT_LOGIN.data());
				cnf.password = mdata.Get<std::wstring>(JsonNames::MQTT_PASS.data());
				cnf.mqttprefix = mdata.Get<std::wstring>(JsonNames::MQTT_PREF.data());
				cnf.sslpsk = mdata.Get<std::wstring>(JsonNames::MQTT_SSLPSK.data());
				cnf.certcapath = mdata.Get<std::wstring>(JsonNames::MQTT_CAPATH.data());
				cnf.port = mdata.Get<uint32_t>(JsonNames::PORT.data(), 0);
				cnf.loglevel = mdata.Get<int32_t>(JsonNames::MQTT_LLEVEL.data(), 0);
				cnf.isssl = mdata.Get<bool>(JsonNames::MQTT_ISSSL.data(), false);
				cnf.isselfsigned = mdata.Get<bool>(JsonNames::MQTT_SSIGN.data(), true);

				if (cnf.empty())
					to_log::Get() << log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_CONFIG_EMPTY_NAME),
						JsonNames::MQTT.data()
					);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void json_config::ReadMMkeyConfig(Tiny::TinyJson& root, MIDI::MMKeyConfig& cnf) {
			try {
				Tiny::xobject mdata = root.Get<Tiny::xobject>(JsonNames::MMKEY.data());
				if (!mdata.Count()) return;

				mdata.Enter();

				cnf.enable = mdata.Get<bool>(JsonNames::ENABLE.data(), false);

				if (cnf.empty())
					to_log::Get() << log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_CONFIG_EMPTY_NAME),
						JsonNames::MMKEY.data()
					);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void json_config::ReadLightConfig(Tiny::TinyJson& root, LIGHT::LightsConfig& cnf) {
			Tiny::xobject mdata = root.Get<Tiny::xobject>(JsonNames::LIGHTS.data());
			if (!mdata.Count()) return;

			mdata.Enter();
			cnf.ispool = mdata.Get<bool>(JsonNames::LIGHTS_POOL.data(), false);

			ReadLightDmxConfig(root, cnf.dmxconf);
			ReadLightArtnetConfig(root, cnf.artnetconf);
		}
		void json_config::ReadLightArtnetConfig(Tiny::TinyJson& root, LIGHT::ArtnetConfig& cnf) {
			try {
				Tiny::xobject ldata = root.Get<Tiny::xobject>(JsonNames::LIGHTS.data());
				if (!ldata.Count()) return;

				ldata.Enter();

				Tiny::xobject mdata = ldata.Get<Tiny::xobject>(JsonNames::LIGHTS_ARTNET.data());
				if (!mdata.Count()) return;

				mdata.Enter();

				cnf.enable = mdata.Get<bool>(JsonNames::ENABLE.data(), false);
				cnf.port = mdata.Get<uint32_t>(JsonNames::PORT.data(), 0x1936);
				cnf.universe = mdata.Get<uint32_t>(JsonNames::LIGHTS_ARTNET_UNIVERSE.data(), 1U);
				cnf.ip = mdata.Get<std::wstring>(JsonNames::IP.data(), JsonNames::EMPTY.data());
				cnf.mask = mdata.Get<std::wstring>(JsonNames::MASK.data(), JsonNames::EMPTY.data());
				cnf.broadcast = mdata.Get<std::wstring>(JsonNames::LIGHTS_ARTNET_BROADCAST.data(), JsonNames::EMPTY.data());

				if (cnf.empty())
					to_log::Get() << log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_CONFIG_EMPTY_NAME),
						JsonNames::LIGHTS_ARTNET.data()
					);


			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void json_config::ReadLightDmxConfig(Tiny::TinyJson& root, LIGHT::SerialPortConfig& cnf) {
			try {
				Tiny::xobject ldata = root.Get<Tiny::xobject>(JsonNames::LIGHTS.data());
				if (!ldata.Count()) return;

				ldata.Enter();

				Tiny::xobject mdata = ldata.Get<Tiny::xobject>(JsonNames::LIGHTS_DMX.data());
				if (!mdata.Count()) return;

				mdata.Enter();

				cnf.enable = mdata.Get<bool>(JsonNames::ENABLE.data(), false);
				cnf.port = mdata.Get<int32_t>(JsonNames::PORT.data(), -1);
				cnf.baudrate = mdata.Get<uint32_t>(JsonNames::LIGHTS_DMX_BAUDRATE.data(), 250000U);
				cnf.stop_bits = mdata.Get<int>(JsonNames::LIGHTS_DMX_STOPBITS.data(), 2);
				cnf.timeout = mdata.Get<int>(JsonNames::LIGHTS_DMX_TIMEOUT.data(), 5);
				cnf.name = mdata.Get<std::wstring>(JsonNames::NAME.data(), JsonNames::EMPTY.data());

				if (cnf.empty())
					to_log::Get() << log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_CONFIG_EMPTY_NAME),
						JsonNames::LIGHTS_DMX.data()
					);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		#pragma region Write
		const bool json_config::Write(MMTConfig* mmt, std::wstring filepath, bool issetreg) {
			try {
				if (!mmt) return false;

				return WriteFile(filepath,
					[=](TinyJson& rjson, std::wstring& confname) -> bool {
						if (!WriteUnitConfig(rjson, mmt)) return false;
						WriteMidiConfig(rjson, mmt->midiconf);
						WriteMqttConfig(rjson, mmt->mqttconf);
						WriteMMkeyConfig(rjson, mmt->mmkeyconf);
						WriteLightConfig(rjson, mmt->lightconf);

						if (issetreg)
							common_config::Get().Registry.SetConfPath(confname);

						return true;
					}
				);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		const bool json_config::WriteFile(std::wstring filepath, std::function<bool(Tiny::TinyJson&, std::wstring&)> f) {
			try {
				TinyJson rjson{};
				bool b = f(rjson, filepath);
				if (!b) return false;

				std::wofstream cnf(filepath, std::ios::trunc);
				if (!cnf.is_open()) {
					to_log::Get() << log_string().to_log_format(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_JSONCONF_WRITE_ACCESS),
						filepath
					);
					return false;
				}
				cnf << rjson.WriteJson();
				cnf.close();
				return true;

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}

		const bool json_config::WriteUnitConfig(Tiny::TinyJson& root, MMTConfig* mmt) {
			try {
				if (mmt->empty()) {
					to_log::Get() << log_string().to_log_string(
						__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_CONFIG_EMPTY)
					);
					return false;
				}
				{
					Tiny::TinyJson cdata{};
					cdata[JsonNames::NAME.data()].Set(mmt->config.c_str());
					cdata[JsonNames::ASTART.data()].Set(mmt->auto_start);
					cdata[JsonNames::BUILDER.data()].Set(Utils::to_string(VER_TXT));
					root[JsonNames::CONF.data()].Set(std::move(cdata));
				}
				Tiny::TinyJson mdata{};
				for (auto& u : mmt->units) {
					Tiny::TinyJson subj{};
					subj[JsonNames::SCENE.data()].Set(static_cast<uint16_t>(u.scene));
					subj[JsonNames::ID.data()].Set(static_cast<uint16_t>(u.key));
					subj[JsonNames::TYPE.data()].Set(static_cast<uint16_t>(u.type));
					subj[JsonNames::TARGET.data()].Set(static_cast<uint16_t>(u.target));
					subj[JsonNames::LONGTARGET.data()].Set(static_cast<uint16_t>(u.longtarget));
					subj[JsonNames::ONOF.data()].Set<bool>(u.value.lvalue);
					subj[JsonNames::VALUE.data()].Set(static_cast<uint16_t>(u.value.value));

					if (!u.appvolume.empty()) {
						Tiny::TinyJson appjson;
						for (auto& app : u.appvolume)
							appjson[JsonNames::EMPTY.data()].Set(app);

						Tiny::TinyJson ajson;
						ajson.Push(std::move(appjson));
						subj[JsonNames::APPS.data()].Set(std::move(ajson));
					}
					mdata.Push(std::move(subj));
				}
				root[JsonNames::UNITS.data()].Set(std::move(mdata));
				return true;

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		void json_config::WriteMidiConfig(Tiny::TinyJson& root, MIDI::MidiConfig& cnf) {
			try {
				if (cnf.empty()) return;
				Tiny::TinyJson mjson{};
				Tiny::TinyJson cjson{};

				cjson[JsonNames::ENABLE.data()].Set<bool>(cnf.enable);
				cjson[JsonNames::OUTPORT_TYPE.data()].Set<bool>(cnf.out_system_port);
				cjson[JsonNames::JOGFILTER.data()].Set<bool>(cnf.jog_scene_filter);
				cjson[JsonNames::OUT_COUNT.data()].Set<uint32_t>(cnf.out_count);
				cjson[JsonNames::PROXY_COUNT.data()].Set<uint32_t>(cnf.proxy_count);
				cjson[JsonNames::BTNINTERVAL.data()].Set<uint32_t>(cnf.btn_interval);
				cjson[JsonNames::BTNLONGINTERVAL.data()].Set<uint32_t>(cnf.btn_long_interval);
				mjson[JsonNames::MIDICTRL.data()].Set(std::move(cjson));

				try {
					TinyJson injson{};
					if (!cnf.midi_in_devices.empty()) {
						TinyJson mjin{};
						for (auto& s : cnf.midi_in_devices)
							mjin[JsonNames::EMPTY.data()].Set(s);
						injson.Push(std::move(mjin));
					}
					injson[JsonNames::ENABLE.data()].Set<bool>(!cnf.midi_in_devices.empty());
					mjson[JsonNames::MIDI_DEV_IN.data()].Set(std::move(injson));
				} catch (...) {}

				try {
					TinyJson outjson{};
					if (!cnf.midi_out_devices.empty()) {
						TinyJson mjout{};
						for (auto& s : cnf.midi_out_devices)
							mjout[JsonNames::EMPTY.data()].Set(s);
						outjson.Push(std::move(mjout));
					}
					outjson[JsonNames::ENABLE.data()].Set<bool>(!cnf.midi_out_devices.empty());
					mjson[JsonNames::MIDI_DEV_OUT.data()].Set(std::move(outjson));
				} catch (...) {}

				root[JsonNames::MIDI.data()].Set(std::move(mjson));
				
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void json_config::WriteMqttConfig(Tiny::TinyJson& root, MQTT::BrokerConfig<std::wstring>& cnf) {
			try {
				if (cnf.empty()) return;
				Tiny::TinyJson mjson{};
				mjson[JsonNames::ENABLE.data()].Set<bool>(cnf.enable);
				mjson[JsonNames::HOST.data()].Set(cnf.host);
				mjson[JsonNames::MQTT_LOGIN.data()].Set(cnf.login);
				mjson[JsonNames::MQTT_PASS.data()].Set(cnf.password);
				mjson[JsonNames::MQTT_PREF.data()].Set(cnf.mqttprefix);
				mjson[JsonNames::MQTT_SSLPSK.data()].Set(cnf.sslpsk);
				mjson[JsonNames::MQTT_CAPATH.data()].Set(cnf.certcapath);
				mjson[JsonNames::PORT.data()].Set<uint32_t>(cnf.port);
				mjson[JsonNames::MQTT_LLEVEL.data()].Set<int32_t>(cnf.loglevel);
				mjson[JsonNames::MQTT_ISSSL.data()].Set<bool>(cnf.isssl);
				mjson[JsonNames::MQTT_SSIGN.data()].Set<bool>(cnf.isselfsigned);
				root[JsonNames::MQTT.data()].Set(std::move(mjson));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void json_config::WriteMMkeyConfig(Tiny::TinyJson& root, MIDI::MMKeyConfig& cnf) {
			try {
				Tiny::TinyJson mjson{};
				mjson[JsonNames::ENABLE.data()].Set<bool>(cnf.enable);
				root[JsonNames::MMKEY.data()].Set(std::move(mjson));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void json_config::WriteLightConfig(Tiny::TinyJson& root, LIGHT::LightsConfig& cnf) {
			if (cnf.empty()) return;
			Tiny::TinyJson rjson{};
			rjson[JsonNames::LIGHTS_POOL.data()].Set<bool>(cnf.ispool);

			WriteLightDmxConfig(rjson, cnf.dmxconf);
			WriteLightArtnetConfig(rjson, cnf.artnetconf);

			root[JsonNames::LIGHTS.data()].Set(std::move(rjson));
		}
		void json_config::WriteLightArtnetConfig(Tiny::TinyJson& root, LIGHT::ArtnetConfig& cnf) {
			try {
				if (cnf.empty()) return;
				Tiny::TinyJson mjson{};
				mjson[JsonNames::ENABLE.data()].Set<bool>(cnf.enable);
				mjson[JsonNames::PORT.data()].Set(cnf.port);
				mjson[JsonNames::LIGHTS_ARTNET_UNIVERSE.data()].Set(cnf.universe);
				mjson[JsonNames::IP.data()].Set(cnf.ip.c_str());
				mjson[JsonNames::MASK.data()].Set(cnf.mask.c_str());
				mjson[JsonNames::LIGHTS_ARTNET_BROADCAST.data()].Set(cnf.broadcast.c_str());
				root[JsonNames::LIGHTS_ARTNET.data()].Set(std::move(mjson));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void json_config::WriteLightDmxConfig(Tiny::TinyJson& root, LIGHT::SerialPortConfig& cnf) {
			try {
				if (cnf.empty()) return;
				Tiny::TinyJson mjson{};
				mjson[JsonNames::ENABLE.data()].Set<bool>(cnf.enable);
				mjson[JsonNames::PORT.data()].Set(cnf.port);
				mjson[JsonNames::LIGHTS_DMX_BAUDRATE.data()].Set(cnf.baudrate);
				mjson[JsonNames::LIGHTS_DMX_STOPBITS.data()].Set<int>(cnf.stop_bits);
				mjson[JsonNames::LIGHTS_DMX_TIMEOUT.data()].Set<int>(cnf.timeout);
				mjson[JsonNames::NAME.data()].Set(cnf.name.c_str());
				root[JsonNames::LIGHTS_DMX.data()].Set(std::move(mjson));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		std::wstring json_config::Dump(MMTConfig* mmt) {
			log_string ls{};
			if (!mmt) return JsonNames::EMPTY.data();

			ls << L"\n* MIDI-MT " << JsonNames::CONF << L":\n";
			ls << L"\t" << JsonNames::CONF << L"=" << Utils::to_string(mmt->config) << L"\n";
			ls << L"\t" << JsonNames::ASTART << L"=" << Utils::BOOL_to_string(mmt->auto_start) << L"\n";
			ls << L"\t" << JsonNames::BUILDER << L"=" << Utils::to_string(mmt->builder) << L"\n";

			ls << L"* " << JsonNames::MIDI << L" :: " << JsonNames::MIDICTRL << L":\n";
			ls << mmt->midiconf.dump() << L"\n\n";

			ls << L"* " << JsonNames::MMKEY << L":\n";
			ls << mmt->mmkeyconf.dump() << L"\n\n";

			ls << L"* " << JsonNames::MQTT << L":\n";
			ls << mmt->mqttconf.dump() << L"\n\n";

			ls << L"* " << JsonNames::LIGHTS << L" :: " << JsonNames::LIGHTS_DMX << L" :: " << JsonNames::LIGHTS_ARTNET << L":\n";
			ls << mmt->lightconf.dump() << L"\n";

			ls << L"* " << JsonNames::UNITS << L":\n";
			for (auto& u : mmt->units) {
				ls << L"\t" << JsonNames::ID << L"=" << static_cast<int>(u.key) << ", ";
				ls << JsonNames::SCENE << L"=" << MIDI::MidiHelper::GetScene(u.scene) << L"/" << static_cast<int>(u.scene) << ", ";
				ls << JsonNames::TYPE << L"=" << MIDI::MidiHelper::GetType(u.type) << ", ";
				ls << JsonNames::TARGET << L"=" << MIDI::MackieHelper::GetTarget(u.target) << ", ";
				ls << JsonNames::LONGTARGET << L"=" << MIDI::MackieHelper::GetTarget(u.longtarget) << L"\n";
				if (!u.appvolume.empty()) {
					ls << L"\t\t" << JsonNames::APPS << L"=[";
					for (auto& app : u.appvolume)
						ls << L"\"" << app << "\", ";
					ls << L"]";
				}
				ls << L"\n";
			}
			return ls.str();
		}
	}
}
