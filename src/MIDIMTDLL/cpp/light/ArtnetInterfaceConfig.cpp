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

        static ArtnetConfig artnetconfig_default{};

        ArtnetConfig::ArtnetConfig() : ip(L""), mask(L""), broadcast(L"") {
        }
        ArtnetConfig::ArtnetConfig(std::string i, std::string m, std::string b) {
            ip = Utils::to_string(i);
            mask = Utils::to_string(m);
            broadcast = Utils::to_string(b);
        }
        const bool ArtnetConfig::empty() {
            return broadcast.empty() || ip.empty();
        }
        log_string ArtnetConfig::dump() {
            log_string ls{};
            if (!ip.empty())
                ls << L"IP: " << ip.c_str() << L", ";
            if (!mask.empty())
                ls << L"Mask: " << mask.c_str() << L", ";
            if (!broadcast.empty())
                ls << L"Broadcast: " << broadcast.c_str() << L", ";
            if (!broadcast.empty())
                ls << L"Universe: " << universe << L"\n";
            return ls;
        }
        void ArtnetConfig::Copy(ArtnetConfig& c) {
            enable = c.enable;
            port = c.port;
            universe = c.universe;
            ip = c.ip.empty() ? L"" : std::wstring(c.ip.begin(), c.ip.end());
            mask = c.mask.empty() ? L"" : std::wstring(c.mask.begin(), c.mask.end());
            broadcast = c.broadcast.empty() ? L"" : std::wstring(c.broadcast.begin(), c.broadcast.end());
        }

        uint32_t ArtnetConfigs::count() {
            return static_cast<uint32_t>(interfaces__.size());
        }
        void ArtnetConfigs::clear() {
            interfaces__.clear();
        }
        const bool ArtnetConfigs::empty() {
            return interfaces__.empty();
        }
        std::vector<ArtnetConfig>& ArtnetConfigs::get() {
            return std::ref(interfaces__);
        }
        ArtnetConfig& ArtnetConfigs::get(uint16_t i) {
            if (i < interfaces__.size())
                return std::ref(interfaces__[i]);
            return std::ref(artnetconfig_default);
        }
        void ArtnetConfigs::add(std::string i, std::string m, std::string b) {
            interfaces__.push_back(ArtnetConfig(i, m, b));
        }

        log_string ArtnetConfigs::dump() {
            uint32_t i = 0;
            log_string ls{};
            for (auto& s : interfaces__)
                ls << i++ << L"| " << s.dump().str().c_str();
            return ls;
        }
    }
}
