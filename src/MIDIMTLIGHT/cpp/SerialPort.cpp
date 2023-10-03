/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol, support USB Open DMX and USB RS485 dongle.
	+ Art-Net protocol, support UDP local network broadcast.
	+ serial compatible ENTTEC OPEN DMX == (standart RS-485 USB Chinese Dongle)
	(c) CC 2023, MIT

	MIDIMTLIGHT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace LIGHT {

        constexpr uint8_t start_dmx_frame_{ 0x0 };

        SerialPort::SerialPort() {
        }
        SerialPort::~SerialPort() {
            dispose_();
        }
        void SerialPort::dispose_() {
            try {
                canceller = true;
                if (serial__) {
                    if (serial__->is_open()) {
                        EscapeCommFunction(serial__->native_handle(), CLRBREAK);
                        serial__->cancel();
                        serial__->close();
                    }
                    serial__.reset();
                }
                if (io__) {
                    io__->stop();
                    io__->reset();
                    io__.reset();
                }
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
        }
        void SerialPort::open_DMX_usb_RS485_HELLO_(auto h) {
            (void)::EscapeCommFunction(h, CLRRTS);
            (void)::PurgeComm(h, PURGE_TXCLEAR | PURGE_RXCLEAR);

            (void)::EscapeCommFunction(h, SETBREAK);
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); /* microseconds(36) */
            (void)::EscapeCommFunction(h, CLRBREAK);
            
            (void)asio::write(*serial__.get(), asio::buffer((void*)&start_dmx_frame_, 0));
            (void)asio::write(*serial__.get(), asio::buffer((void*)&start_dmx_frame_, sizeof(start_dmx_frame_)));
            std::this_thread::sleep_for(std::chrono::microseconds(8));
        }

        const bool SerialPort::is_open() const {
            return (serial__ && serial__->is_open());
        }
		bool SerialPort::start(SerialPortConfig& conf) {
            if ((serial__) && serial__->is_open()) return true;
            if (conf.empty()) return false;
            try {
                canceller = false;
                io__ = std::unique_ptr<asio::io_service>(new asio::io_service());
                serial__ = std::unique_ptr<asio::serial_port>(new asio::serial_port(*io__.get()));
                serial__->open(conf.GetPort());
                if (!serial__->is_open()) {
                    dispose_();
                    return false;
                }
                serial__->set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
                serial__->set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));
                serial__->set_option(asio::serial_port_base::character_size(8));
                serial__->set_option(asio::serial_port_base::baud_rate(conf.baudrate));
                serial__->set_option(asio::serial_port_base::stop_bits((conf.stop_bits == 2) ?
                    asio::serial_port_base::stop_bits::two : asio::serial_port_base::stop_bits::one
                ));
                if (!serial__->is_open()) {
                    dispose_();
                    return false;
                }
                auto h = serial__->native_handle();
                if (!h) {
                    dispose_();
                    return false;
                }
                (void)::PurgeComm(h, PURGE_TXCLEAR | PURGE_RXCLEAR);
                (void)::EscapeCommFunction(h, CLRRTS);
                (void)::SetCommMask(h, EV_TXEMPTY);
                return true;
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            dispose_();
            return false;
		}
        void SerialPort::stop() {
            dispose_();
        }
        bool SerialPort::send(byte* arr, size_t sz) {
            if ((!serial__) || !serial__->is_open()) return false;
            bool r{ false };
            try {
                auto h = serial__->native_handle();
                if (!h) return r;
                open_DMX_usb_RS485_HELLO_(h);

                r = (asio::write(*serial__.get(), asio::buffer((void*)arr, sz)) == sz);
                DWORD status{ 0 };
                while (status != EV_TXEMPTY) {
                    h = serial__->native_handle();
                    if (!h) break;
                    if (canceller.load()) return true;
                    (void)::WaitCommEvent(h, &status, 0);
                    std::this_thread::sleep_for(std::chrono::milliseconds(2));
                }
                #if defined(_DEBUG_PRINT)
                ::OutputDebugStringW(L"* WaitCommEvent done\n");
                #endif
                if (h) (void)::EscapeCommFunction(h, SETRTS);
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return r;
        }
        std::vector<byte> SerialPort::receive() {
            do {
                if ((!serial__) || !serial__->is_open()) break;
                try {
                    std::vector<byte> packet(1024);
                    size_t sz = asio::read(*serial__.get(), asio::buffer(packet, packet.size()));
                    packet.resize(sz);
                    return packet;
                } catch (...) {
                    Utils::get_exception(std::current_exception(), __FUNCTIONW__);
                }
            } while (0);
            return std::vector<byte>();
        }
	}
}
