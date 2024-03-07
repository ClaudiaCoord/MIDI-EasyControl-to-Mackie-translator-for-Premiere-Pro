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

        DMXSerial::DMXSerial(std::function<DMXPacket()> f) : DMXClassInterface(f) {
        }
        DMXSerial::~DMXSerial() {
            dispose_();
        }
        void DMXSerial::dispose_() {
            try {
                if (wbg_id)
                    wbg_id = worker_background::Get().remove(wbg_id);
                if (isrun_)
                    sp_.stop();
                isrun_ = isready_ = false;
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
        }
        bool DMXSerial::reconnect_() {
            if (sp_.is_open()) return true;
            try {
                isrun_ = isready_ = false;
                SerialPortConfig cnf = config_;
                return Start(cnf);
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return false;
        }

		bool DMXSerial::Start(SerialPortConfig& cnf) {
            if (isrun_) {
                if (isready_) return true;
                dispose_();
            }
            isrun_ = true;

            try {
                config_ = cnf;
                auto f = std::async(std::launch::async, [=]() -> bool {
                    try {
                        return sp_.start(config_);
                    } catch (...) {
                        Utils::get_exception(std::current_exception(), __FUNCTIONW__);
                    }
                    return false;
                });
                isrun_ = isready_ = f.get();
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return isready_;
		}
        void DMXSerial::Stop() {
            dispose_();
        }
        bool DMXSerial::IsRun() {
            return isrun_.load() && isready_.load();
        }
        void DMXSerial::Send(std::vector<byte>& v) {
            if (!IsRun()) return;
            try {
                if (!sp_.send(&v[0], v.size()))
                    reconnect_();
                return;
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            reconnect_();
        }
        void DMXSerial::Send_async(std::vector<byte>& v) {
            if (!IsRun()) return;
            try {
                auto f = std::async(std::launch::async, [&]() -> void {
                    std::unique_lock<std::mutex> lock(mutex_);
                    Send(v);
                });
                (void) f.get();
                return;
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            reconnect_();
        }
        std::vector<byte> DMXSerial::Receive() {
            if (!IsRun()) return std::vector<byte>();
            try {
                auto f = std::async(std::launch::async, [=]() -> std::vector<byte> {
                    try {
                        return sp_.receive();
                    } catch (...) {
                        Utils::get_exception(std::current_exception(), __FUNCTIONW__);
                    }
                    return std::vector<byte>();
                });
                return f.get();
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return std::vector<byte>();
        }

        SerialPortConfigs& DMXSerial::GetDivices(bool isrescan) {
            if (isrescan || devices_.empty()) {
                try {
                    CEnumeratePortsArray ports;
                    CEnumerateNamesArray names;

                    if (CEnumerateSerial::UsingSetupAPI2(ports, names)) {
                        for (size_t i = 0; i < ports.size(); i++)
                            devices_.add(ports[i], names[i]);
                    }
                } catch (...) {
                    Utils::get_exception(std::current_exception(), __FUNCTIONW__);
                }
            }
            return std::ref(devices_);
        }
        SerialPortConfig&  DMXSerial::GetDivice() {
            return std::ref(config_);
        }
	}
}
