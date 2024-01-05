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
	namespace REMOTE {

		template <typename T1>
		T1 RemoteConfig<T1>::clr_() {
			if constexpr (std::is_same_v<std::string, T1>) return "";
			else return L"";
		}

		template <typename T1>
		template <typename T2>
		T1 RemoteConfig<T1>::scnv_(T2& s) {
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
		void RemoteConfig<T1>::clear() {
			port = 0;
			timeoutreq = 5;
			timeoutidle = 0;
			loglevel = 1;
			enable = false;
			isipv6 = false;
			isreuseaddr = false;
			isfastsocket = false;
			host = clr_();
			server_ua = clr_();
		}

		template <typename T1>
		const bool RemoteConfig<T1>::empty() {
			return (port == 0) || (port >= 65536) || host.empty();
		}

		template <typename T1>
		template <typename T2>
		void RemoteConfig<T1>::Copy(RemoteConfig<T2>& c) {
			enable = c.enable;
			port = ((c.port > 0) && (c.port < 65536)) ? c.port : port;
			timeoutreq = c.timeoutreq;
			timeoutidle = c.timeoutidle;
			loglevel = c.loglevel;
			isipv6 = c.isipv6;
			isreuseaddr = c.isreuseaddr;
			isfastsocket = c.isfastsocket;
			host = scnv_(c.host);
			server_ua = scnv_(c.server_ua);
		}

		template <typename T1>
		std::wstring RemoteConfig<T1>::dump() {
			return (log_string() << L"\tenable module: " << Utils::BOOL_to_string(enable)
								 << L"\n\tInterface: http://" << Utils::to_string(host) << L":" << port
								 << L"\n\tServer UA: " << Utils::to_string(server_ua)
								 << L"\n\tIPv6: " << Utils::BOOL_to_string(isipv6) << L", reuse address: " << Utils::BOOL_to_string(isreuseaddr) << L", socket fast open: " << Utils::BOOL_to_string(isfastsocket)
								 << L"\n\tTimeouts, request: " << timeoutreq << L", idle: " << timeoutidle << L", log level: " << loglevel).str();
		}

		template std::wstring RemoteConfig<std::string>::dump();
		template std::wstring RemoteConfig<std::wstring>::dump();

		template void RemoteConfig<std::string>::clear();
		template void RemoteConfig<std::wstring>::clear();

		template const bool RemoteConfig<std::string>::empty();
		template const bool RemoteConfig<std::wstring>::empty();

		template std::string RemoteConfig<std::string>::clr_();
		template std::wstring RemoteConfig<std::wstring>::clr_();

		template std::string RemoteConfig<std::string>::scnv_(std::wstring&);
		template std::string RemoteConfig<std::string>::scnv_(std::string&);
		template std::wstring RemoteConfig<std::wstring>::scnv_(std::string&);
		template std::wstring RemoteConfig<std::wstring>::scnv_(std::wstring&);

		template FLAG_EXPORT void RemoteConfig<std::string>::Copy(RemoteConfig<std::wstring>&);
		template FLAG_EXPORT void RemoteConfig<std::string>::Copy(RemoteConfig<std::string>&);
		template FLAG_EXPORT void RemoteConfig<std::wstring>::Copy(RemoteConfig<std::string>&);
		template FLAG_EXPORT void RemoteConfig<std::wstring>::Copy(RemoteConfig<std::wstring>&);
	}
}