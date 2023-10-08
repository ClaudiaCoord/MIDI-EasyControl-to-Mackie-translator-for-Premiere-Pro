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

		constexpr std::u8string_view strTopic = u8"sensor";
		constexpr std::u8string_view strTopicTitle = u8"title";
		constexpr std::u8string_view strTopicSlider = u8"slider";
		constexpr std::u8string_view strTopicOnOff = u8"onoff";
		constexpr std::u8string_view strTopicWill = u8"state";
		constexpr std::u8string_view strDefaultName = u8"ctrl9";
		constexpr std::u8string_view strOn = u8"255";
		constexpr std::u8string_view strOff = u8"0";
		static std::atomic<bool> isconnect__{ false };

		void default_mosquitto_deleter::operator()(void* v) {
			mosquitto* m = reinterpret_cast<mosquitto*>(v);
			if (m) {
				::mosquitto_disconnect(m);
				::mosquitto_destroy(m);
			}
			try { ::mosquitto_lib_cleanup(); } catch (...) {}
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
				const char* s = (rc != -1) ? ::mosquitto_reason_string(rc) : nullptr;
				log_string ls;
				ls.to_log_method(method_to_name(locate));
				if (!w.empty())
					ls << w.data() << L" ";
				if (br != nullptr)
					ls << L"-> " << br->GetHost().c_str();
				if (s != nullptr)
					ls << " (" << Utils::to_string(s) << L")";
				if (flag != -1)
					ls << L" [" << flag << L"]";
				if (!log_string::is_string_empty(ls))
					to_log::Get() << ls.str();
			} catch (...) {}
		}
		static void on_connect__(mosquitto*, void* u, int rc, int flag, const mosquitto_property*) {
			isconnect__ = (rc == 0);
			cb_tolog__(__FUNCTIONW__, isconnect__.load() ?
				common_error_code::Get().get_error(common_error_id::err_MQTT_CONNECTED) :
				common_error_code::Get().get_error(common_error_id::err_MQTT_NOT_CONNECTED),
				u, rc, flag
			);
		}
		static void on_disconnect__(mosquitto*, void* u, int rc, const mosquitto_property*) {
			isconnect__ = false;
			cb_tolog__(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_MQTT_DISCONNECTED), u, rc, -1);
		}
		static void on_log__(mosquitto* mosq, void* u, int level, const char* s) {
			try {
				if (s == nullptr) return;
				Broker* br = reinterpret_cast<Broker*>(u);
				if (br == nullptr) return;
				if (br->GetLogLevel() >= level)
					cb_tolog__(__FUNCTIONW__, Utils::to_string(s), u, -1, level);
			} catch (...) {}
		}

		static bool reconnect_(mosquitto* m, int32_t& err) {
			if (!m) return false;
			try {
				if ((err = ::mosquitto_reconnect(m)) != MOSQ_ERR_SUCCESS) return false;
				(void) ::mosquitto_loop_misc(m);
				isconnect__ = true;
				return isconnect__.load();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}

		Broker::Broker() {
		}
		Broker::~Broker() {
			reset();
		}
		void Broker::dispose_() {
			try {
				isrun__ = false;
				if (broker__) {
					mosquitto* m = (mosquitto*)broker__.release();
					if (m) {
						::mosquitto_disconnect(m);
						::mosquitto_destroy(m);
					}
				}
				try { ::mosquitto_lib_cleanup(); } catch (...) {}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		std::u8string Broker::get_target_topic_(MIDI::Mackie::Target key) {
			switch (key) {
				using enum MIDI::Mackie::Target;
				case AV1: return u8"av1";
				case AV2: return u8"av2";
				case AV3: return u8"av3";
				case AV4: return u8"av4";
				case AV5: return u8"av5";
				case AV6: return u8"av6";
				case AV7: return u8"av7";
				case AV8: return u8"av8";
				case XV9: return u8"xv9";
				case AP1: return u8"ap1";
				case AP2: return u8"ap2";
				case AP3: return u8"ap3";
				case AP4: return u8"ap4";
				case AP5: return u8"ap5";
				case AP6: return u8"ap6";
				case AP7: return u8"ap7";
				case AP8: return u8"ap8";
				case XP9: return u8"xp9";
				case B11: return u8"b11";
				case B12: return u8"b12";
				case B13: return u8"b13";
				case B14: return u8"b14";
				case B15: return u8"b15";
				case B16: return u8"b16";
				case B17: return u8"b17";
				case B18: return u8"b18";
				case B19: return u8"b19";
				case B21: return u8"b21";
				case B22: return u8"b22";
				case B23: return u8"b23";
				case B24: return u8"b24";
				case B25: return u8"b25";
				case B26: return u8"b26";
				case B27: return u8"b27";
				case B28: return u8"b28";
				case B29: return u8"b29";
				case B31: return u8"b31";
				case B32: return u8"b32";
				case B33: return u8"b33";
				case B34: return u8"b34";
				case B35: return u8"b35";
				case B36: return u8"b36";
				case B37: return u8"b37";
				case B38: return u8"b38";
				case B39: return u8"b39";
				default: return u8"default";
			}
		}
		std::u8string Broker::get_target_ttitle_(MIDI::Mackie::Target key) {
			switch (key) {
				using enum MIDI::Mackie::Target;
				case AV1: return u8"level 1";
				case AV2: return u8"level 2";
				case AV3: return u8"level 3";
				case AV4: return u8"level 4";
				case AV5: return u8"level 5";
				case AV6: return u8"level 6";
				case AV7: return u8"level 7";
				case AV8: return u8"level 8";
				case XV9: return u8"level 9";
				case AP1: return u8"level 10";
				case AP2: return u8"level 11";
				case AP3: return u8"level 12";
				case AP4: return u8"level 13";
				case AP5: return u8"level 14";
				case AP6: return u8"level 15";
				case AP7: return u8"level 16";
				case AP8: return u8"level 17";
				case XP9: return u8"level 18";
				case B11: return u8"on/off 1";
				case B12: return u8"on/off 2";
				case B13: return u8"on/off 3";
				case B14: return u8"on/off 4";
				case B15: return u8"on/off 5";
				case B16: return u8"on/off 6";
				case B17: return u8"on/off 7";
				case B18: return u8"on/off 8";
				case B19: return u8"on/off 9";
				case B21: return u8"on/off 10";
				case B22: return u8"on/off 11";
				case B23: return u8"on/off 12";
				case B24: return u8"on/off 13";
				case B25: return u8"on/off 14";
				case B26: return u8"on/off 15";
				case B27: return u8"on/off 16";
				case B28: return u8"on/off 17";
				case B29: return u8"on/off 18";
				case B31: return u8"on/off 19";
				case B32: return u8"on/off 20";
				case B33: return u8"on/off 21";
				case B34: return u8"on/off 22";
				case B35: return u8"on/off 23";
				case B36: return u8"on/off 24";
				case B37: return u8"on/off 25";
				case B38: return u8"on/off 26";
				case B39: return u8"on/off 27";
				default:  return u8"default";
			}
		}
		std::string   Broker::build_topic_(std::u8string s, const std::u8string_view& v) {
			std::stringstream topic;
			if (!config__.mqttprefix.empty())
				topic << config__.mqttprefix.c_str() << u8"/"_logs;
			topic << std::string(strTopic.begin(), strTopic.end());

			if (!config__.login.empty())
				topic << u8"/"_logs << config__.login.c_str();
			else
				topic << u8"/"_logs << std::string(strDefaultName.begin(), strDefaultName.end());
			if (!s.empty())
				topic << u8"/"_logs << std::string(s.begin(), s.end());
			topic << u8"/"_logs << std::string(v.begin(), v.end());
			return topic.str();
		}

		const bool Broker::isrunning() {
			return isrun__;
		}
		std::wstring Broker::GetHost() {
			std::wstringstream ws;
			ws << std::wstring(config__.host.begin(), config__.host.end()).c_str() << L":" << config__.port;
			return ws.str();
		}
		int32_t Broker::GetLogLevel() {
			return config__.loglevel;
		}

		bool Broker::init(BrokerConfig<std::wstring>& c) {
			try {
				int32_t err = 0;
				mosquitto* m = nullptr;
				isconnect__ = false;
				do {
					config__.Copy(c);
					if (config__.empty()) {
						to_log::Get() << log_string().to_log_string(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_MQTT_EMPTY_CONFIG)
						);
						break;
					}

					std::stringstream mqttid;
					mqttid << (!config__.login.empty() ? config__.login.c_str() : std::string(strDefaultName.begin(), strDefaultName.end()))
						   << "-"
						   << Utils::random_hash(this);

					::mosquitto_lib_init();
					m = ::mosquitto_new(mqttid.str().c_str(), true, this);
					if (m == nullptr) break;

					::mosquitto_log_callback_set(m, on_log__);
					if ((err = ::mosquitto_int_option(m, MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V5)) != MOSQ_ERR_SUCCESS) break;

					if (!config__.login.empty()) {
						if ((err = ::mosquitto_username_pw_set(m, config__.login.data(), config__.password.data())) != MOSQ_ERR_SUCCESS) break;
						if (!config__.sslpsk.empty())
							if ((err = ::mosquitto_tls_psk_set(m, config__.sslpsk.data(), config__.login.data(), 0)) != MOSQ_ERR_SUCCESS) break;
					}
					if (config__.isssl && config__.sslpsk.empty() && !config__.certcapath.empty()) {
						std::filesystem::path capath(config__.certcapath);
						if (std::filesystem::exists(capath)) {
							if ((err = ::mosquitto_tls_set(m, capath.string().data(), capath.parent_path().string().data(), 0, 0, 0)) != MOSQ_ERR_SUCCESS) break;
							if ((err = ::mosquitto_tls_opts_set(m, 1, 0, 0)) != MOSQ_ERR_SUCCESS) break;
							if ((err = ::mosquitto_tls_insecure_set(m, config__.isselfsigned)) != MOSQ_ERR_SUCCESS) break;
						} else {
							to_log::Get() << log_string().to_log_fomat(
								__FUNCTIONW__,
								common_error_code::Get().get_error(common_error_id::err_MQTT_NOT_CA_CERT),
								capath.wstring()
							);
						}
					}

					::mosquitto_connect_v5_callback_set(m, on_connect__);
					::mosquitto_disconnect_v5_callback_set(m, on_disconnect__);

					#if defined(MOSQ_WILL_ENABLE)
					std::string topic = build_topic_(std::u8string(), strTopicWill);
					if ((err = ::mosquitto_will_set_v5(m, topic.data(), 1, u8"0", 0, true, 0)) != MOSQ_ERR_SUCCESS) break;
					#endif

					if ((err = ::mosquitto_connect(m, config__.host.data(), config__.port, 0)) != MOSQ_ERR_SUCCESS) break;
					if ((err = ::mosquitto_loop_misc(m)) == MOSQ_ERR_SUCCESS) isconnect__ = true;

					#if defined(MOSQ_WILL_ENABLE)
					int32_t ids = ids__.load();
					if (::mosquitto_publish_v5(m, &ids, topic.data(), 1, u8"1", 0, true, nullptr) == MOSQ_ERR_SUCCESS) {
						ids__ = ids;
					}
					#endif

				} while (0);
				switch (err) {
					case MOSQ_ERR_SUCCESS: {
						if (m == nullptr) {
							to_log::Get() << log_string().to_log_string(
								__FUNCTIONW__,
								common_error_code::Get().get_error(common_error_id::err_MQTT_CLIENT_ERROR)
							);
							return false;
						}
						broker__.reset(m);
						isrun__ = true;
						to_log::Get() << log_string().to_log_string(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_MQTT_CLIENT_OK)
						);
						return true;
					}
					case MOSQ_ERR_ERRNO: {
						_com_error e(::GetLastError());
						to_log::Get() << (log_string().to_log_method(__FUNCTIONW__) << e.ErrorMessage());
						break;
					}
					default: {
						const char* s = ::mosquitto_strerror(err);
						to_log::Get() << (log_string().to_log_method(__FUNCTIONW__) << Utils::to_string(s));
						break;
					}
				}
				if (m != nullptr) {
					::mosquitto_disconnect(m);
					::mosquitto_destroy(m);
					::mosquitto_lib_cleanup();
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
			dispose_();
			config__.clear();
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
					mosquitto* m = (mosquitto*)broker__.get();
					if (!m) break;
					if (!isconnect__ && !reconnect_(m, err)) break;

					std::string v;
					std::string topic;

					if constexpr (std::is_same_v<uint32_t, T>) {
						topic = build_topic_(get_target_topic_(target), strTopicSlider);
						v = std::to_string(val);
					}
					else if constexpr (std::is_same_v<bool, T>) {
						topic = build_topic_(get_target_topic_(target), strTopicOnOff);
						v = val ? std::string(strOn.begin(), strOn.end()) : std::string(strOff.begin(), strOff.end());
					}
					else if constexpr (std::is_same_v<std::string, T>) {
						topic = build_topic_(get_target_topic_(target), strTopicTitle);
						v = val;
						retain = true;
					}
					else if constexpr (std::is_same_v<std::u8string, T>) {
						topic = build_topic_(get_target_topic_(target), strTopicTitle);
						v = reinterpret_cast<const char*>(val.data());
						retain = true;
					}
					else break;

					err = ::mosquitto_loop_misc(m);
					if (err == MOSQ_ERR_NO_CONN) {
						if ((err = ::mosquitto_reconnect(m)) != MOSQ_ERR_SUCCESS) break;
					}
					else if (err == MOSQ_ERR_SUCCESS) {}
					else break;

					if ((err = ::mosquitto_publish_v5(m, &ids, topic.c_str(), static_cast<int>(v.size()), v.data(), 0, retain, nullptr)) != MOSQ_ERR_SUCCESS) break;
					(void) ::mosquitto_loop_write(m, 1);

				} while (0);
				switch (err) {
					case MOSQ_ERR_SUCCESS: {
						ids__ = ids;
						break; 
					}
					case MOSQ_ERR_ERRNO: {
						_com_error e(::GetLastError());
						to_log::Get() << (log_string().to_log_method(__FUNCTIONW__) << e.ErrorMessage());
						if (!broker__) break;
						(void)reconnect_((mosquitto*)broker__.get(), err);
						break;
					}
					default: {
						const char* s = ::mosquitto_strerror(err);
						to_log::Get() << (log_string().to_log_method(__FUNCTIONW__) << Utils::to_string(s));
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
		template void Broker::send<std::u8string>(std::u8string, MIDI::Mackie::Target);
	}
}
