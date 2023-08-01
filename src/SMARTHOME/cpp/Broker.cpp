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
#include "mosquitto.h"
#include <comdef.h>

namespace Common {
	namespace MQTT {

		#define MOSQ_WILL_ENABLE 1

		using namespace std::string_view_literals;

		constexpr std::wstring_view errConfig = L": MQTT configuration is empty, abort.."sv;
		constexpr std::wstring_view errClient = L": MQTT client creation error, canceled.."sv;
		constexpr std::wstring_view okClient = L": MQTT client successfully created, continue.."sv;
		constexpr std::wstring_view cbOnConnectOk = L": connected to MQTT server:"sv;
		constexpr std::wstring_view cbOnConnectErr = L": failed connect to MQTT server:"sv;
		constexpr std::wstring_view cbOnDisconnect = L": disconnected from MQTT server:"sv;
		constexpr std::wstring_view sslCaFileMissing = L": CA certificate file not exists: "sv;

		constexpr std::string_view strTopic = u8"sensor";
		constexpr std::string_view strTopicTitle = u8"title";
		constexpr std::string_view strTopicSlider = u8"slider";
		constexpr std::string_view strTopicOnOff = u8"onoff";
		constexpr std::string_view strTopicWill = u8"state";
		constexpr std::string_view strDefaultName = u8"ctrl9";

		void default_mosquitto_deleter::operator()(void* m) {
			mosquitto* mosq = (mosquitto*)m;
			if (mosq != nullptr) {
				::mosquitto_disconnect(mosq);
				::mosquitto_destroy(mosq);
			}
		}

		static std::wstring method_to_name(const wchar_t locate[]) {
			size_t sz = wcslen(locate);
			if (sz > 2)
				return std::wstring(&locate[0], &locate[(sz - 2)]);
			return std::wstring(locate);
		}
		static void cb_tolog__(const wchar_t locate[], std::wstring_view w, void* u, int rc, int flag = -1) {
			try {
				Broker* br = reinterpret_cast<Broker*>(u);
				const char* s = ::mosquitto_reason_string(rc);
				log_string log;
				log << method_to_name(locate) << w;
				if (br != nullptr)
					log << L" " << br->GetHost();
				if (s != nullptr)
					log << " (" << Utils::to_string(s) << L")";
				if (flag != -1)
					log << L" [" << flag << L"]";
				to_log::Get() << log.str() << L" ";
			} catch (...) {}
		}
		static void on_connect__(mosquitto*, void* u, int rc, int flag, const mosquitto_property*) {
			cb_tolog__(__FUNCTIONW__, (rc == 0) ? cbOnConnectOk : cbOnConnectErr, u, rc, flag);
		}
		static void on_disconnect__(mosquitto*, void* u, int rc, const mosquitto_property*) {
			cb_tolog__(__FUNCTIONW__, cbOnDisconnect, u, rc, -1);
		}
		static void on_log__(mosquitto* mosq, void* u, int level, const char* s) {
			try {
				Broker* br = reinterpret_cast<Broker*>(u);
				if (br == nullptr) return;
				if (br->GetLogLevel() >= level) {
					if (s != nullptr)
						to_log::Get() << (log_string() << method_to_name(__FUNCTIONW__) << L": " << Utils::to_string(s) << L" [" << level << L"]");
					else
						to_log::Get() << (log_string() << method_to_name(__FUNCTIONW__) << L": [" << level << L"]");
				}
			} catch (...) {}
		}

		Broker::Broker(){
			try { ::mosquitto_lib_init(); } catch (...) {}
		}
		Broker::~Broker() {
			dispose_();
			try { ::mosquitto_lib_cleanup(); } catch (...) {}
		}
		void Broker::dispose_() {
			try {
				isrun__ = false;
				if (!broker__) return;
				mosquitto* mosq = (mosquitto*) broker__.release();
				if (mosq == nullptr) return;
				::mosquitto_disconnect(mosq);
				::mosquitto_destroy(mosq);
				config__.clear();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		std::string Broker::get_target_topic_(MIDI::Mackie::Target key) {
			switch (key) {
				case MIDI::Mackie::Target::AV1: return u8"av1";
				case MIDI::Mackie::Target::AV2: return u8"av2";
				case MIDI::Mackie::Target::AV3: return u8"av3";
				case MIDI::Mackie::Target::AV4: return u8"av4";
				case MIDI::Mackie::Target::AV5: return u8"av5";
				case MIDI::Mackie::Target::AV6: return u8"av6";
				case MIDI::Mackie::Target::AV7: return u8"av7";
				case MIDI::Mackie::Target::AV8: return u8"av8";
				case MIDI::Mackie::Target::XV9: return u8"xv9";
				case MIDI::Mackie::Target::AP1: return u8"ap1";
				case MIDI::Mackie::Target::AP2: return u8"ap2";
				case MIDI::Mackie::Target::AP3: return u8"ap3";
				case MIDI::Mackie::Target::AP4: return u8"ap4";
				case MIDI::Mackie::Target::AP5: return u8"ap5";
				case MIDI::Mackie::Target::AP6: return u8"ap6";
				case MIDI::Mackie::Target::AP7: return u8"ap7";
				case MIDI::Mackie::Target::AP8: return u8"ap8";
				case MIDI::Mackie::Target::XP9: return u8"xp9";
				case MIDI::Mackie::Target::B11: return u8"b11";
				case MIDI::Mackie::Target::B12: return u8"b12";
				case MIDI::Mackie::Target::B13: return u8"b13";
				case MIDI::Mackie::Target::B14: return u8"b14";
				case MIDI::Mackie::Target::B15: return u8"b15";
				case MIDI::Mackie::Target::B16: return u8"b16";
				case MIDI::Mackie::Target::B17: return u8"b17";
				case MIDI::Mackie::Target::B18: return u8"b18";
				case MIDI::Mackie::Target::B19: return u8"b19";
				case MIDI::Mackie::Target::B21: return u8"b21";
				case MIDI::Mackie::Target::B22: return u8"b22";
				case MIDI::Mackie::Target::B23: return u8"b23";
				case MIDI::Mackie::Target::B24: return u8"b24";
				case MIDI::Mackie::Target::B25: return u8"b25";
				case MIDI::Mackie::Target::B26: return u8"b26";
				case MIDI::Mackie::Target::B27: return u8"b27";
				case MIDI::Mackie::Target::B28: return u8"b28";
				case MIDI::Mackie::Target::B29: return u8"b29";
				case MIDI::Mackie::Target::B31: return u8"b31";
				case MIDI::Mackie::Target::B32: return u8"b32";
				case MIDI::Mackie::Target::B33: return u8"b33";
				case MIDI::Mackie::Target::B34: return u8"b34";
				case MIDI::Mackie::Target::B35: return u8"b35";
				case MIDI::Mackie::Target::B36: return u8"b36";
				case MIDI::Mackie::Target::B37: return u8"b37";
				case MIDI::Mackie::Target::B38: return u8"b38";
				case MIDI::Mackie::Target::B39: return u8"b39";
				default: return u8"default";
			}
		}
		std::string Broker::get_target_ttitle_(MIDI::Mackie::Target key) {
			switch (key) {
				case MIDI::Mackie::Target::AV1: return u8"level 1";
				case MIDI::Mackie::Target::AV2: return u8"level 2";
				case MIDI::Mackie::Target::AV3: return u8"level 3";
				case MIDI::Mackie::Target::AV4: return u8"level 4";
				case MIDI::Mackie::Target::AV5: return u8"level 5";
				case MIDI::Mackie::Target::AV6: return u8"level 6";
				case MIDI::Mackie::Target::AV7: return u8"level 7";
				case MIDI::Mackie::Target::AV8: return u8"level 8";
				case MIDI::Mackie::Target::XV9: return u8"level 9";
				case MIDI::Mackie::Target::AP1: return u8"level 10";
				case MIDI::Mackie::Target::AP2: return u8"level 11";
				case MIDI::Mackie::Target::AP3: return u8"level 12";
				case MIDI::Mackie::Target::AP4: return u8"level 13";
				case MIDI::Mackie::Target::AP5: return u8"level 14";
				case MIDI::Mackie::Target::AP6: return u8"level 15";
				case MIDI::Mackie::Target::AP7: return u8"level 16";
				case MIDI::Mackie::Target::AP8: return u8"level 17";
				case MIDI::Mackie::Target::XP9: return u8"level 18";
				case MIDI::Mackie::Target::B11: return u8"on/off 1";
				case MIDI::Mackie::Target::B12: return u8"on/off 2";
				case MIDI::Mackie::Target::B13: return u8"on/off 3";
				case MIDI::Mackie::Target::B14: return u8"on/off 4";
				case MIDI::Mackie::Target::B15: return u8"on/off 5";
				case MIDI::Mackie::Target::B16: return u8"on/off 6";
				case MIDI::Mackie::Target::B17: return u8"on/off 7";
				case MIDI::Mackie::Target::B18: return u8"on/off 8";
				case MIDI::Mackie::Target::B19: return u8"on/off 9";
				case MIDI::Mackie::Target::B21: return u8"on/off 10";
				case MIDI::Mackie::Target::B22: return u8"on/off 11";
				case MIDI::Mackie::Target::B23: return u8"on/off 12";
				case MIDI::Mackie::Target::B24: return u8"on/off 13";
				case MIDI::Mackie::Target::B25: return u8"on/off 14";
				case MIDI::Mackie::Target::B26: return u8"on/off 15";
				case MIDI::Mackie::Target::B27: return u8"on/off 16";
				case MIDI::Mackie::Target::B28: return u8"on/off 17";
				case MIDI::Mackie::Target::B29: return u8"on/off 18";
				case MIDI::Mackie::Target::B31: return u8"on/off 19";
				case MIDI::Mackie::Target::B32: return u8"on/off 20";
				case MIDI::Mackie::Target::B33: return u8"on/off 21";
				case MIDI::Mackie::Target::B34: return u8"on/off 22";
				case MIDI::Mackie::Target::B35: return u8"on/off 23";
				case MIDI::Mackie::Target::B36: return u8"on/off 24";
				case MIDI::Mackie::Target::B37: return u8"on/off 25";
				case MIDI::Mackie::Target::B38: return u8"on/off 26";
				case MIDI::Mackie::Target::B39: return u8"on/off 27";
				default: return u8"default";
			}
		}
		std::string Broker::build_topic_(std::string s, const std::string_view& v) {
			std::stringstream topic;
			topic << u8"";
			if (!config__.mqttprefix.empty())
				topic << config__.mqttprefix.c_str() << u8"/";
			topic << strTopic;

			if (!config__.login.empty())
				topic << u8"/" << config__.login.c_str();
			else
				topic << u8"/" << strDefaultName;
			if (!s.empty())
				topic << u8"/" << s.c_str();
			topic << u8"/" << v;
			return topic.str();
		}

		const bool Broker::isrunning() {
			return isrun__;
		}
		std::wstring Broker::GetHost() {
			std::wstringstream ws;
			ws << std::wstring(config__.host.begin(), config__.host.end()) << L":" << config__.port;
			return ws.str();
		}
		int32_t Broker::GetLogLevel() {
			return config__.loglevel;
		}

		bool Broker::init(BrokerConfig<std::wstring>& c) {
			try {
				int32_t err = 0;
				mosquitto* mosq = nullptr;
				do {
					config__.Copy(c);
					if (config__.empty()) {
						to_log::Get() << __FUNCTIONW__ << errConfig;
						break;
					}

					std::stringstream mqttid;
					mqttid << (!config__.login.empty() ? config__.login.c_str() : strDefaultName) << "-";
					mqttid << Utils::random_hash(this);

					mosq = ::mosquitto_new(mqttid.str().c_str(), true, this);
					if (mosq == nullptr) break;

					::mosquitto_log_callback_set(mosq, on_log__);
					if ((err = ::mosquitto_int_option(mosq, MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V5)) != MOSQ_ERR_SUCCESS) break;

					if (!config__.login.empty()) {
						if ((err = ::mosquitto_username_pw_set(mosq, config__.login.data(), config__.password.data())) != MOSQ_ERR_SUCCESS) break;
						if (!config__.sslpsk.empty())
							if ((err = ::mosquitto_tls_psk_set(mosq, config__.sslpsk.data(), config__.login.data(), 0)) != MOSQ_ERR_SUCCESS) break;
					}
					if (config__.isssl && config__.sslpsk.empty() && !config__.certcapath.empty()) {
						std::filesystem::path capath(config__.certcapath);
						if (std::filesystem::exists(capath)) {
							if ((err = ::mosquitto_tls_set(mosq, capath.string().data(), capath.parent_path().string().data(), 0, 0, 0)) != MOSQ_ERR_SUCCESS) break;
							if ((err = ::mosquitto_tls_opts_set(mosq, 1, 0, 0)) != MOSQ_ERR_SUCCESS) break;
							if ((err = ::mosquitto_tls_insecure_set(mosq, config__.isselfsigned)) != MOSQ_ERR_SUCCESS) break;
						} else {
							to_log::Get() << (log_string() << __FUNCTIONW__ << sslCaFileMissing << capath.wstring());
						}
					}

					::mosquitto_connect_v5_callback_set(mosq, on_connect__);
					::mosquitto_disconnect_v5_callback_set(mosq, on_disconnect__);

					#if defined(MOSQ_WILL_ENABLE)
					std::string topic = build_topic_(std::string(), strTopicWill);
					if ((err = ::mosquitto_will_set_v5(mosq, topic.data(), 1, u8"0", 0, true, 0)) != MOSQ_ERR_SUCCESS) break;
					#endif

					if ((err = ::mosquitto_connect(mosq, config__.host.data(), config__.port, 0)) != MOSQ_ERR_SUCCESS) break;

					err = ::mosquitto_loop_misc(mosq);

					#if defined(MOSQ_WILL_ENABLE)
					int32_t ids = ids__.load();
					if (::mosquitto_publish_v5(mosq, &ids, topic.data(), 1, u8"1", 0, true, nullptr) == MOSQ_ERR_SUCCESS) {
						ids__ = ids;
					}
					#endif

				} while (0);
				switch (err) {
					case MOSQ_ERR_SUCCESS: {
						if (mosq == nullptr) {
							to_log::Get() << (log_string() << __FUNCTIONW__ << errClient);
							return false;
						}
						broker__.reset(mosq);
						isrun__ = true;
						return true;
					}
					case MOSQ_ERR_ERRNO: {
						_com_error e(err);
						to_log::Get() << (log_string() << __FUNCTIONW__ << L": " << e.ErrorMessage());
						break;
					}
					default: {
						const char* s = ::mosquitto_strerror(err);
						to_log::Get() << (log_string() << __FUNCTIONW__ << L": " << Utils::to_string(s));
						break;
					}
				}
				if (mosq != nullptr) {
					::mosquitto_disconnect(mosq);
					::mosquitto_destroy(mosq);
				}

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		void Broker::release() {
			dispose_();
		}
		void Broker::reset() {
			try {
				isrun__ = false;
				if (broker__) {
					mosquitto* mosq = (mosquitto*)broker__.release();
					if (mosq != nullptr) {
						::mosquitto_disconnect(mosq);
						::mosquitto_destroy(mosq);
					}
				}
				::mosquitto_lib_cleanup();
				::mosquitto_lib_init();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void Broker::settitle(std::vector<MIDI::Mackie::Target>& v) {
			try {
				for (auto& t : v) send(get_target_ttitle_(t), t);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		template <typename T>
		void Broker::send(T val, MIDI::Mackie::Target target) {
			try {
				bool retain = false;
				int32_t err = 0,
						ids = ids__.load();
				do {
					if (!broker__) break;
					mosquitto* mosq = (mosquitto*)broker__.get();
					if (mosq == nullptr) break;

					std::string v;
					std::string topic;

					if constexpr (std::is_same_v<uint32_t, T>) {
						topic = build_topic_(get_target_topic_(target), strTopicSlider);
						v = std::to_string(val);
					}
					else if constexpr (std::is_same_v<bool, T>) {
						topic = build_topic_(get_target_topic_(target), strTopicOnOff);
						v = std::to_string(val);
					}
					else if constexpr (std::is_same_v<std::string, T>) {
						topic = build_topic_(get_target_topic_(target), strTopicTitle);
						v = val;
						retain = true;
					}
					else break;

					err = ::mosquitto_loop_misc(mosq);
					if (err == MOSQ_ERR_NO_CONN) {
						if ((err = ::mosquitto_reconnect(mosq)) != MOSQ_ERR_SUCCESS) break;
					}
					else if (err == MOSQ_ERR_SUCCESS) {}
					else break;

					if ((err = ::mosquitto_publish_v5(mosq, &ids, topic.c_str(), static_cast<int>(v.size()), v.data(), 0, retain, nullptr)) != MOSQ_ERR_SUCCESS) break;
					(void) ::mosquitto_loop_write(mosq, 1);

				} while (0);
				switch (err) {
					case MOSQ_ERR_SUCCESS: {
						ids__ = ids;
						break; 
					}
					case MOSQ_ERR_ERRNO: {
						_com_error e(err);
						to_log::Get() << (log_string() << __FUNCTIONW__ << L": " << e.ErrorMessage());
						break;
					}
					default: {
						const char* s = ::mosquitto_strerror(err);
						to_log::Get() << (log_string() << __FUNCTIONW__ << L": " << Utils::to_string(s));
						break;
					}
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		template void Broker::send<bool>(bool, MIDI::Mackie::Target);
		template void Broker::send<uint32_t>(uint32_t, MIDI::Mackie::Target);
		template void Broker::send<std::string>(std::string, MIDI::Mackie::Target);
	}
}
