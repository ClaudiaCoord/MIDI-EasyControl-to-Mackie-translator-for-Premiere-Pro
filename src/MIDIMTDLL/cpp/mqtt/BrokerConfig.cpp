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
	namespace MQTT {

		template <typename T1>
		T1 BrokerConfig<T1>::clr_() {
			if constexpr (std::is_same_v<std::string, T1>) return "";
			else return L"";
		}

		template <typename T1>
		template <typename T2>
		T1 BrokerConfig<T1>::scnv_(T2& s) {
			if (s.empty()) {
				if constexpr (std::is_same_v<std::string, T1>) return "";
				else return L"";
			}
			if constexpr ((std::is_same_v<std::string, T1>) && (std::is_same_v<std::wstring, T2>)) {
				return Utils::from_string(s);
			}
			else if constexpr ((std::is_same_v<std::wstring, T1>) && (std::is_same_v<std::string, T2>)) {
				return Utils::to_string(s);
			}
			#pragma warning( push )
			#pragma warning( disable : 4244 )
			return T1(s.begin(), s.end());
			#pragma warning( pop )
		}

		template <typename T1>
		void BrokerConfig<T1>::clear() {
			port = 0;
			loglevel = 0;
			isssl = false;
			isselfsigned = false;
			host = clr_();
			login = clr_();
			password = clr_();
			mqttprefix = clr_();
			sslpsk = clr_();
			certcapath = clr_();
		}

		template <typename T1>
		const bool BrokerConfig<T1>::empty() {
			return ((port == 0) || (port >= 65536) || host.empty()) || (isssl && sslpsk.empty());
		}

		template <typename T1>
		template <typename T2>
		void BrokerConfig<T1>::Copy(BrokerConfig<T2>& c) {
			enable = c.enable;
			port = ((c.port > 0) && (c.port < 65536)) ? c.port : port;
			isssl = c.certcapath.empty() ? false : (c.sslpsk.empty() ? c.isssl : false);
			loglevel = c.loglevel;
			host = scnv_(c.host);
			login = scnv_(c.login);
			sslpsk = scnv_(c.sslpsk);
			password = scnv_(c.password);
			mqttprefix = scnv_(c.mqttprefix);
			certcapath = scnv_(c.certcapath);
			isselfsigned = c.isselfsigned;
		}

		template <typename T1>
		std::wstring BrokerConfig<T1>::dump() {
			return (log_string() << L"\tenable module:" << Utils::BOOL_to_string(enable)
								 << L"\n\tMQTT host:" << Utils::to_string(host) << L":" << port << L", prefix: " << Utils::to_string(mqttprefix)
								 << L"\n\tlogin: " << Utils::to_string(login) << L"/" << (password.empty() ? L"-empty-" : L"****")
								 << L"\n\tssl: " << Utils::BOOL_to_string(isssl) << L", self-signed: " << Utils::BOOL_to_string(isselfsigned)
								 << L"\n\tCA: [" << Utils::to_string(certcapath) << L"], psk: [" << Utils::to_string(sslpsk)) << L"]"
								 << L"\n\tloglevel: " << loglevel;
		}

		template std::wstring BrokerConfig<std::string>::dump();
		template std::wstring BrokerConfig<std::wstring>::dump();

		template void BrokerConfig<std::string>::clear();
		template void BrokerConfig<std::wstring>::clear();

		template const bool BrokerConfig<std::string>::empty();
		template const bool BrokerConfig<std::wstring>::empty();

		template std::string BrokerConfig<std::string>::clr_();
		template std::wstring BrokerConfig<std::wstring>::clr_();

		template std::string BrokerConfig<std::string>::scnv_(std::wstring&);
		template std::string BrokerConfig<std::string>::scnv_(std::string&);
		template std::wstring BrokerConfig<std::wstring>::scnv_(std::string&);
		template std::wstring BrokerConfig<std::wstring>::scnv_(std::wstring&);

		template FLAG_EXPORT void BrokerConfig<std::string>::Copy(BrokerConfig<std::wstring>&);
		template FLAG_EXPORT void BrokerConfig<std::string>::Copy(BrokerConfig<std::string>&);
		template FLAG_EXPORT void BrokerConfig<std::wstring>::Copy(BrokerConfig<std::string>&);
		template FLAG_EXPORT void BrokerConfig<std::wstring>::Copy(BrokerConfig<std::wstring>&);
	}
}