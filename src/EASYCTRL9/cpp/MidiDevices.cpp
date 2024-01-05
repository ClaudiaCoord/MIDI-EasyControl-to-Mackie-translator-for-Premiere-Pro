/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	EASYCTRL9

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
    namespace MIDI {
        
        using namespace std::string_view_literals;
        
        MidiDevices MidiDevices::ctrlmididevices__{};

        MidiDevices::MidiDevices() : mdrv_(std::make_shared<MidiDriver>()) {
            mdrv_->Init();
        }
        MidiDevices::~MidiDevices() {
            if (mdrv_) mdrv_.reset();
        }

        MidiDevices&                 MidiDevices::Get() {
            return std::ref(ctrlmididevices__);
        }
        const bool                   MidiDevices::CheckVirtualDriver() {
            return (mdrv_) ? mdrv_->Check() : false;
        }
        std::wstring                 MidiDevices::GetVirtualDriverVersion() {
            try {
                if (!mdrv_) return std::wstring();
                LPCWSTR v = mdrv_->vGetDriverVersion();
                if (!CHECK_LPWSTRING(v)) return L"";
                return Utils::to_string(v);
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return std::wstring();
        }
        std::shared_ptr<MidiDriver>& MidiDevices::GetMidiDriver() {
            return std::ref(mdrv_);
        }
        std::vector<std::wstring>&   MidiDevices::GetMidiInDeviceList(bool isrenew) {
            _set_se_translator(seh_exception_catch);
            try {
                if ((device_in_list_.empty() || isrenew) && CheckVirtualDriver()) {

                    device_in_list_.clear();
                    uint32_t i = 0U, cnt = mdrv_->InGetNumDevs();
                    if (!cnt) {
                        to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_MIDI_NOT_DEVICES));
                        return std::ref(device_in_list_);
                    }
                    for (; i < cnt; ++i) {

                        MIDIINCAPS mc{};
                        MMRESULT m = mdrv_->InGetDevCaps(i, &mc, sizeof(mc));
                        if (m != S_OK) {
                            to_log::Get() << (log_string() << LogTag << Utils::MMRESULT_to_string(m) << L" [" << i << L"]");
                            continue;
                        }
                        device_in_list_.push_back(std::wstring(mc.szPname));
                    }
                    if (cnt != device_in_list_.size())
                        to_log::Get() << log_string().to_log_format(
                            __FUNCTIONW__,
                            common_error_code::Get().get_error(common_error_id::err_MIDI_COUNT_DEVICES),
                            cnt, device_in_list_.size()
                        );
                }
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return std::ref(device_in_list_);
        }
        std::vector<std::wstring>&   MidiDevices::GetMidiOutDeviceList(bool isrenew) {
            _set_se_translator(seh_exception_catch);
            try {
                if ((device_out_list_.empty() || isrenew) && CheckVirtualDriver()) {

                    device_out_list_.clear();
                    uint32_t i = 0U, cnt = mdrv_->OutGetNumDevs();
                    if (!cnt) {
                        to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_MIDI_NOT_DEVICES));
                        return std::ref(device_out_list_);
                    }
                    for (; i < cnt; ++i) {
                        MIDIOUTCAPS mc{};
                        MMRESULT m = mdrv_->OutGetDevCaps(i, &mc, sizeof(mc));
                        if (m != S_OK) {
                            to_log::Get() << (log_string() << LogTag << Utils::MMRESULT_to_string(m) << L" [" << i << L"]");
                            continue;
                        }
                        device_out_list_.push_back(std::wstring(mc.szPname));
                    }
                    if (cnt != device_out_list_.size())
                        to_log::Get() << log_string().to_log_format(
                            __FUNCTIONW__,
                            common_error_code::Get().get_error(common_error_id::err_MIDI_COUNT_DEVICES),
                            cnt, device_out_list_.size()
                        );
                }
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return std::ref(device_out_list_);
        }
        std::vector<std::wstring>&   MidiDevices::GetMidiProxyDeviceList() {
            return std::ref(device_proxy_list_);
        }
    }
}