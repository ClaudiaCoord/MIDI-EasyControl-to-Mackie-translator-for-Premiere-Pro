/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol.
	+ Art-Net protocol.
	(c) CC 2023, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace LIGHT {

        static SerialPortConfig serialportconfig_default{};

        const bool SerialPortConfig::empty() {
            return (port <= 0) || (baudrate == 0) || (timeout == 0);
        }
        std::string SerialPortConfig::GetPort() {
            if (port <= 0) return "";
            return "COM" + std::to_string(port);
        }
        SerialPortConfig::SerialPortConfig() {
            name = L"-";
        }
        SerialPortConfig::SerialPortConfig(int32_t p, std::string s) {
            port = p;
            name = Utils::to_string(s);
        }
        SerialPortConfig::SerialPortConfig(int32_t p, std::wstring s) {
            port = p;
            name = std::wstring(s.begin(), s.end());
        }
        log_string SerialPortConfig::dump() {
            log_string ls{};
            ls << L"\tenable module:" << Utils::BOOL_to_string(enable);
            ls << L"\n\tPort: COM" << port << L", ";
            ls << L"Baudrate: " << baudrate << L", ";
            ls << L"Stop bits: " << stop_bits;
            if (!name.empty())
                ls << L"\n\tDevice name: " << name.c_str();
            ls << L"\n";
            return ls;
        }
        void SerialPortConfig::Copy(SerialPortConfig& c) {
            enable = c.enable;
            port = c.port;
            baudrate = c.baudrate;
            stop_bits = c.stop_bits;
            timeout = c.timeout;
            name = c.name.empty() ? L"" : std::wstring(c.name.begin(), c.name.end());
        }

        uint32_t SerialPortConfigs::count() {
            return static_cast<uint32_t>(devices__.size());
        }
        void SerialPortConfigs::clear() {
            devices__.clear();
        }
        const bool SerialPortConfigs::empty() {
            return devices__.empty();
        }
        SerialPortConfig& SerialPortConfigs::get(uint16_t i) {
            if (i < devices__.size())
                return std::ref(devices__[i]);
            return std::ref(serialportconfig_default);
        }
        std::vector<SerialPortConfig>& SerialPortConfigs::get() {
            return std::ref(devices__);
        }
        log_string SerialPortConfigs::dump() {
            uint32_t i = 0;
            log_string ls{};
            for (auto& s : devices__)
                ls << i++ << L"| " << s.dump().str().c_str();
            return ls;
        }
	}
}
