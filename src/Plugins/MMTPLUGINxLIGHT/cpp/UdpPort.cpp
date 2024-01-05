/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol, support USB Open DMX and USB RS485 dongle.
	+ Art-Net protocol, support UDP local network broadcast.
	(c) CC 2023-2024, MIT

	MIDIMTLIGHT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace LIGHT {

        UdpPort::UdpPort() {
        }
        UdpPort::~UdpPort() {
            dispose_();
        }
        void UdpPort::dispose_() {
            try {
                canceller = true;
                if (socket__) {
                    if (socket__->is_open()) {
                        socket__->cancel();
                        socket__->close();
                    }
                    socket__.reset();
                }
                if (io__) {
                    io__->stop();
                    io__->reset();
                    io__.reset();
                }
                if (endpoint__)
                    endpoint__.reset();

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
        }
        const bool UdpPort::is_open() const {
            return (socket__ && socket__->is_open());
        }
		bool UdpPort::start(ArtnetConfig& cnf) {
            if ((socket__) && socket__->is_open()) return true;
            try {
                canceller = false;
                io__ = std::unique_ptr<asio::io_service>(new asio::io_service());
                socket__ = std::unique_ptr<asio::ip::udp::socket>(new asio::ip::udp::socket(*io__.get()));
                socket__->open(asio::ip::udp::v4());
                if (!socket__->is_open()) {
                    dispose_();
                    return false;
                }
                socket__->set_option(asio::ip::udp::socket::reuse_address(true));
                socket__->set_option(asio::socket_base::broadcast(true));
                endpoint__ = std::unique_ptr<asio::ip::udp::endpoint>(new asio::ip::udp::endpoint(asio::ip::address::from_string(Utils::from_string(cnf.broadcast)), cnf.port));
                return socket__->is_open();
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            dispose_();
            return false;
		}
        void UdpPort::stop() {
            dispose_();
        }
        bool UdpPort::send(byte* arr, size_t sz) {
            if ((!socket__) || !socket__->is_open() || (!endpoint__)) return false;
            bool r{ false };
            try {
                r = (socket__->send_to(asio::buffer((void*)arr, sz), *endpoint__.get()) == sz);
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return r;
        }
        std::vector<byte> UdpPort::receive() {
            do {
                if ((!socket__) || !socket__->is_open()) break;
                try {
                } catch (...) {
                    Utils::get_exception(std::current_exception(), __FUNCTIONW__);
                }
            } while (0);
            return std::vector<byte>();
        }
	}
}
