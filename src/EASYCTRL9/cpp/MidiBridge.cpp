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
            mdrv__ = std::shared_ptr<MidiDriver>(new MidiDriver());
            min__ = std::unique_ptr<MidiControllerIn>(new MidiControllerIn(mdrv__));
            mout__ = std::unique_ptr<MidiControllerOut>(new MidiControllerOut(mdrv__));
            mproxy__ = std::unique_ptr<MidiControllerProxy>(new MidiControllerProxy(mdrv__));
        }
        MidiBridge::~MidiBridge() {
            dispose_();
            try {
                min__.reset();
                mout__.reset();
                mproxy__.reset();
                mdrv__.reset();
            } catch (...) {}
        }

        void MidiBridge::dispose_() {
            try {
                if (isproxy__ && mproxy__) {
                    if (out_event__)
                        out_event__->remove_ptr(mproxy__.get());
                    mproxy__->Stop();
                    isproxy__ = false;
                }
                else if (isproxy__)
                    isproxy__ = false;

                if (min__) min__->Stop();
                if (in_event__) {
                    if (min__)
                        in_event__->remove_ptr(min__.get());
                    in_event__.reset();
                }

                if (mout__) mout__->Stop();
                if (out_event__) {
                    if (mout__)
                        out_event__->remove_ptr(mout__.get());
                    out_event__.reset();
                }
                isenable__ = false;
            } catch (...) {}
        }
        MidiBridge& MidiBridge::Get() {
            return std::ref(ctrlmidibridge__);
        }
        const bool MidiBridge::IsEnable() {
            return isenable__;
        }
        const bool MidiBridge::CheckVirtualDriver() {
            return mdrv__->Check();
        }
        std::wstring MidiBridge::GetVirtualDriverVersion() {
            try {
                LPCWSTR v = mdrv__->vGetDriverVersion();
                if (!CHECK_LPWSTRING(v)) return L"";
                return Utils::to_string(v);
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
            return L"";
        }

        bool loader_(common_config& conf, std::wstring& confpath) {
            return confpath.empty() ? conf.Load() : conf.Load(confpath);
        }

        const bool MidiBridge::Start(std::wstring cnfpath) {
            try {
                to_log& log = to_log::Get();
                if (!mdrv__->Check()) {
                    log << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_NOT_DRIVER));
                    return false;
                }
                common_config& conf = common_config::Get();
                if (!conf.IsNewConfig()) {
                    log << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_CONFIG_NEW));
                    if (conf.IsConfig()) {
                        if (conf.IsConfigEmpty()) {
                            to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_CONFIG_EMPTY));
                            return false;
                        }
                        to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_CONFIG_LOADA));
                        return start_();
                    }
                }

                if (loader_(conf, cnfpath)) {
                    if (conf.IsConfigEmpty()) {
                        to_log::Get() << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_CONFIG_EMPTY));
                        return false;
                    }
                    log << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_CONFIG_LOAD));
                    return start_();
                } else {
                    log << log_string().to_log_string(__FUNCTIONW__, common_error_code::Get().get_error(common_error_id::err_CONFIG_FAIL));
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
                common_config& conf = common_config::Get();
                std::shared_ptr<MidiDevice>& cnf = conf.GetConfig();
                if (cnf->IsEmpty()) return false;

                out_event__.reset(new bridge_out_event(cnf));
                out_event__->add_ptr(mout__.get());

                in_event__.reset(new bridge_in_event(out_event__));
                in_event__->add_ptr(min__.get());
                in_event__->IsJogSceneFilter(cnf->jogscenefilter);

                do {
                    if (!(isenable__ = min__->Start(cnf))) break;
                    if (!(isenable__ = mout__->Start(cnf))) break;
                    if (conf.IsProxy()) {
                        if (!(isenable__ = mproxy__->Start(cnf))) break;
                        if (!(isproxy__ = (mproxy__->GetProxyCount() > 0)))
                            mproxy__->Stop();
                        else
                            out_event__->add_ptr(mproxy__.get());
                    }
                    common_config::Get().Local.IsMidiBridgeRun(true);
                    return true;
                } while (0);
            }
            catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            dispose_();
            return false;
        }
        const bool MidiBridge::Stop() {
            dispose_();
            common_config::Get().Local.IsMidiBridgeRun(isenable__);
            return !isenable__;
        }

        std::vector<std::wstring>& MidiBridge::GetInputDeviceList() {
            return min__->GetReBuildDeviceList();
        }
        std::vector<std::wstring>& MidiBridge::GetOutputDeviceList() {
            return mout__->GetDeviceList();
        }
        std::vector<std::wstring>& MidiBridge::GetProxyDeviceList() {
            return mproxy__->GetDeviceList();
        }

        void MidiBridge::SetCallbackIn(MidiControllerBase& mcb) {
            if (in_event__ && isenable__) in_event__->add(mcb);
            else throw_common_error(common_error_id::err_SET_CB_WARNING);
        }
        void MidiBridge::SetCallbackIn(MidiInstance* mi) {
            if (in_event__ && isenable__) in_event__->add_ptr(mi);
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
        void MidiBridge::RemoveCallbackIn(MidiInstance* mi) {
            if (in_event__ && isenable__) in_event__->remove_ptr(mi);
        }
    }
}