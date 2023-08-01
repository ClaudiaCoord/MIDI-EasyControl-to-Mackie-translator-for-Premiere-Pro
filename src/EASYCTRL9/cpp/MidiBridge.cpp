/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	EASYCTRL9

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
    namespace MIDI {
        
        using namespace std::string_view_literals;
        
        MidiBridge MidiBridge::ctrlmidibridge__;

        MidiBridge::MidiBridge() : isenable__(false), isproxy__(false) {
        }
        MidiBridge::~MidiBridge() {
            Dispose();
        }

        void MidiBridge::Dispose() {
            try {
                if (isproxy__) {
                    isproxy__ = false;
                    MidiControllerProxy::Get().Stop();
                }
                MidiControllerOut::Get().Stop();
                MidiControllerIn::Get().Stop();

                in_event__->remove(MidiControllerIn::Get());
                in_event__.reset();
                out_event__.reset();
                isenable__ = false;
            } catch (...) {}
        }
        MidiBridge& MidiBridge::Get() {
            return std::ref(ctrlmidibridge__);
        }
        const bool MidiBridge::IsEnable() {
            return isenable__;
        }
        std::wstring MidiBridge::GetVirtualDriverVersion() {
            do {
                try {
                    LPCWSTR v = midi_utils::Run_virtualMIDIGetDriverVersion();
                    if (!CHECK_LPWSTRING(v)) break;
                    return Utils::to_string(v);
                } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            } while (0);
            return L"";
        }

        bool loader_(Common::common_config& cconf, std::wstring& cnfpath) {
            return cnfpath.empty() ? cconf.Load() : cconf.Load(cnfpath);
        }

        const bool MidiBridge::Start(std::wstring cnfpath) {
            try {
                Common::to_log& log_ = Common::to_log::Get();
                Common::common_config& cconf = Common::common_config::Get();
                if (!cconf.IsNewConfig()) {
                    to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_CONFIG_NEW));
                    if (cconf.IsConfig()) {
                        if (cconf.IsConfigEmpty()) {
                            to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_CONFIG_EMPTY));
                            return false;
                        }
                        to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_CONFIG_LOADA));
                        return start_();
                    }
                }

                if (loader_(cconf, cnfpath)) {
                    if (cconf.IsConfigEmpty()) {
                        to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_CONFIG_EMPTY));
                        return false;
                    }
                    to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_CONFIG_LOAD));
                    return start_();
                } else {
                    to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_CONFIG_FAIL));
                }
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return false;
        }
        const bool MidiBridge::start_() {
            try {
                auto f = std::async(std::launch::async, [=]() -> const bool {
                    return start__();
                    });
                return f.get();
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return false;
        }
        const bool MidiBridge::start__() {
            try {
                Common::common_config& cconf = Common::common_config::Get();
                std::shared_ptr<MidiDevice>& cnf = cconf.GetConfig();
                if (cnf->IsEmpty()) return false;

                out_event__.reset(new bridge_out_event(cnf));
                out_event__->add(MidiControllerOut::Get());
                if (cconf.IsProxy())
                    out_event__->add(MidiControllerProxy::Get());

                in_event__.reset(new bridge_in_event(out_event__));
                in_event__->add(MidiControllerIn::Get());
                in_event__->IsJogSceneFilter(cnf->jogscenefilter);

                do {
                    if (!(isenable__ = MidiControllerIn::Get().Start(cnf))) break;
                    if (!(isenable__ = MidiControllerOut::Get().Start(cnf))) break;
                    if (cconf.IsProxy()) {
                        if (!(isenable__ = MidiControllerProxy::Get().Start(cnf))) break;
                        if (!(isproxy__ = (MidiControllerProxy::Get().GetProxyCount() > 0)))
                            MidiControllerProxy::Get().Stop();
                    }
                    Common::common_config::Get().Local.IsMidiBridgeRun(true);
                    return true;
                } while (0);
            }
            catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            Dispose();
            return false;
        }
        const bool MidiBridge::Stop() {
            Dispose();
            Common::common_config::Get().Local.IsMidiBridgeRun(isenable__);
            return !isenable__;
        }

        std::vector<std::wstring>& MidiBridge::GetInputDeviceList() {
            return MidiControllerIn::Get().GetReBuildDeviceList();
        }
        std::vector<std::wstring>& MidiBridge::GetOutputDeviceList() {
            return MidiControllerOut::Get().GetDeviceList();
        }
        std::vector<std::wstring>& MidiBridge::GetProxyDeviceList() {
            return MidiControllerProxy::Get().GetDeviceList();
        }

        void MidiBridge::SetCallbackIn(MidiControllerBase& mcb) {
            if (in_event__ && isenable__) in_event__->add(mcb);
            else throw_common_error(common_error_id::err_SET_CB_WARNING);
        }
        void MidiBridge::SetCallbackOut(MidiControllerBase& mcb) {
            if (out_event__ && isenable__) out_event__->add(mcb);
            else throw_common_error(common_error_id::err_SET_CB_WARNING);

        }
        void MidiBridge::SetCallbackOut(MidiInstance& mi) {
            if (out_event__ && isenable__) out_event__->add(mi);
            else throw_common_error(common_error_id::err_SET_CB_WARNING);
        }
        void MidiBridge::RemoveCallbackOut(uint32_t id) {
            if (out_event__ && isenable__) out_event__->remove(id);
        }
        void MidiBridge::RemoveCallbackOut(MidiInstance& mi) {
            if (out_event__ && isenable__) out_event__->remove(mi.GetId());
        }
        void MidiBridge::RemoveCallbackOut(MidiControllerBase& mcb) {
            if (out_event__ && isenable__) out_event__->remove(mcb.GetId());
        }
        void MidiBridge::RemoveCallbackIn(MidiControllerBase& mcb) {
            if (in_event__ && isenable__) in_event__->remove(mcb);
        }
    }
}