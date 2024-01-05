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
#define MAX_MIDI_VALUE 127

// #define DEBUG_NO_TARGET 1

namespace Common {
    namespace IO {

        using namespace std::placeholders;
        using namespace std::string_view_literals;

        constexpr std::wstring_view EXCEPT_CB1 = L" - call function pointer \"CbOut1\" not valid! = "sv;
        constexpr std::wstring_view EXCEPT_CB2 = L" - call function pointer \"CbOut2\" not valid! = "sv;
        
        static inline void to_log__(const wchar_t* f, const char* s, const std::wstring_view& v, PluginCb* cb) {
            try {
                log_string ls;
                ls.to_log_method(f);
                ls << cb->GetType() << v.data() << cb->GetId();
                if (s) ls << L", " << Utils::to_string(s);
                to_log::Get() << ls.str();
            } catch (...) {}
        }

        bridge_out_event::bridge_out_event() : cnf_(common_config::Get().GetConfig()) {

            PluginCb::id_ = Utils::random_hash(this);
            PluginCb::type_ = PluginClassTypes::ClassOut;
            PluginCb::using_ = (PluginCbType::Out1Cb | PluginCbType::Out2Cb | PluginCbType::ConfCb);

            PluginCb::out1_cb_ = [](MIDI::Mackie::MIDIDATA, DWORD) {};
            PluginCb::out2_cb_ = [](MIDI::MidiUnit&, DWORD) {};
            PluginCb::cnf_cb_  = std::bind(static_cast<void(bridge_out_event::*)(std::shared_ptr<JSON::MMTConfig>&)>(&bridge_out_event::set_config_cb_), this, _1);

            lock__ = std::make_shared<locker_awaiter>();
        }
        bridge_out_event::~bridge_out_event() {
            clear();
        }

        PluginCb& bridge_out_event::GetCb() {
            return *static_cast<PluginCb*>(this);
        }
        void bridge_out_event::set_config_cb_(std::shared_ptr<JSON::MMTConfig>& cnf) {
            try {
                if (cnf->empty()) return;
                locker_auto locker(lock__, locker_auto::LockType::TypeLockOnlyOne);
                if (!locker.Begin()) return;

                cnf_ = cnf;
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }

        #pragma region PluginCbSet Interface
        void bridge_out_event::SetCbOut(PluginCb* cb) {
            try {
                uint16_t key__ = TO_BIT(Out1Cb);
                if (cb->IsCbType(key__)) {
                    callOut1Cb_t f = cb->GetCbOut1();
                    if (f) event_.add(cb, cb->GetId());
                } else {
                    key__ = TO_BIT(Out2Cb);
                    if (cb->IsCbType(key__)) {
                        callOut2Cb_t f = cb->GetCbOut2();
                        if (f) event_.add(cb, cb->GetId());
                    }
                }
            } catch (...) {}
        }
        void bridge_out_event::RemoveCbOut(uint32_t id) {
            event_.remove(id);
        }
        #pragma endregion

        void bridge_out_event::clear() {
            event_.clear();
        }
        void bridge_out_event::CbInCall(MIDI::Mackie::MIDIDATA m, DWORD t) {
            try {

                auto& list = event_.get();
                if (list.empty()) return;

                MIDI::MidiUnitRef unitref{};

                for (auto& pair : list) {
                    PluginCb* pcb = pair.second;
                    if (!pcb) continue;

                    #if defined(DEBUG_BRIDGE_OUT)
                    to_log::Get() << (log_string() << L"\t\t(CallCbOut) = " << (unsigned long)a.GetId() << L" [" << a.GetType() << L"],");
                    to_log::Get() << L"\t\t\t\t" << m.Dump();
                    #endif

                    if (unitref.isbegin()) {
                        if (!unitref.isvalid()) continue;

                        PluginClassTypes utype = unitref.type();
                        utype = (pcb->IsCbType(PluginCbType::Out1Cb)) ? PluginClassTypes::ClassOut1 :
                                 ((pcb->IsCbType(PluginCbType::Out2Cb)) ? PluginClassTypes::ClassOut2 : utype);

                        switch (utype) {
                            using enum PluginClassTypes;
                            case ClassOut1:
                            case ClassProxy:
                            case ClassMonitor: {
                                try {
                                    callOut1Cb_t f = pcb->GetCbOut1();
                                    if (f) f(m, t);
                                }
                                catch (std::bad_function_call& e) {
                                    to_log__(__FUNCTIONW__, e.what(), EXCEPT_CB1, pcb);
                                }
                                break;
                            }
                            case ClassOut2:
                            case ClassMixer:
                            case ClassRemote:
                            case ClassMqttKey:
                            case ClassMediaKey:
                            case ClassLightKey:
                            case ClassOutMidiMackie: {
                                try {
                                    callOut2Cb_t f = pcb->GetCbOut2();
                                    if (f) f(unitref.get(), t);
                                }
                                catch (std::bad_function_call& e) {
                                    to_log__(__FUNCTIONW__, e.what(), EXCEPT_CB2, pcb);
                                }
                                break;
                            }
                            default: break;
                        }
                        continue;
                    }

                    PluginClassTypes ctype = pcb->GetType();
                    ctype = (pcb->IsCbType(PluginCbType::Out1Cb)) ? PluginClassTypes::ClassOut1 : ctype;

                    switch (ctype) {
                        using enum PluginClassTypes;
                        case ClassOut1:
                        case ClassProxy:
                        case ClassMonitor: {
                            try {
                                callOut1Cb_t f = pcb->GetCbOut1();
                                if (f) f(m, t);
                            } catch (...) {
                                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
                            }
                            break;
                        }
                    }

                    locker_auto locker(lock__, locker_auto::LockType::TypeWaitOnlyOne);
                    if (locker.IsCanceled()) return;

                    if (cnf_->units.empty()) return;

                    switch (ctype) {
                        using enum PluginClassTypes;
                        case ClassRemote: {

                            for (auto& u : cnf_->units) {

                                if (u.key == m.key() && u.scene == m.scene()) { // && u.target == m.target()) {

                                    if ((u.target == MIDI::Mackie::Target::VOLUMEMIX) && u.appvolume.empty())
                                        continue;

                                    if (!MIDI::MidiSetter::ValidTarget(u))
                                        continue;

                                    if ((u.type == MIDI::MidiUnitType::BTN) || (u.type == MIDI::MidiUnitType::BTNTOGGLE))
                                        MIDI::MidiSetter::SetButton(u);

                                    else if (
                                        (u.type == MIDI::MidiUnitType::SLIDER) ||
                                        (u.type == MIDI::MidiUnitType::FADER) ||
                                        (u.type == MIDI::MidiUnitType::KNOB) ||
                                        (u.type == MIDI::MidiUnitType::SLIDERINVERT) ||
                                        (u.type == MIDI::MidiUnitType::FADERINVERT) ||
                                        (u.type == MIDI::MidiUnitType::KNOBINVERT)) {
                                        if (!MIDI::MidiSetter::SetVolume(u, m.value()))
                                            continue;
                                    }
                                    else continue;

                                    try {
                                        callOut2Cb_t f = pcb->GetCbOut2();
                                        if (f) f(u, t);
                                    } catch (std::bad_function_call& e) {
                                        to_log__(__FUNCTIONW__, e.what(), EXCEPT_CB2, pcb);
                                    }
                                    if (remote_once) break;
                                }
                            }
                            break;
                        }
                        case ClassMixer: {

                            for (auto& u : cnf_->units) {

                                if (u.key == m.key() && u.scene == m.scene()) {
                                    if (u.target != MIDI::Mackie::Target::VOLUMEMIX) break;

                                    unitref.begin();

                                    if (u.appvolume.empty())
                                        break;

                                    if (!MIDI::MidiSetter::ValidTarget(u))
                                        break;

                                    if ((u.type == MIDI::MidiUnitType::BTN) || (u.type == MIDI::MidiUnitType::BTNTOGGLE)) {
                                        if (!MIDI::MidiSetter::ÑhatterButton(u, m, cnf_.get()->midiconf.get_interval()))
                                            break;

                                        MIDI::MidiSetter::SetButton(u);
                                    }
                                    else if (
                                        (u.type == MIDI::MidiUnitType::SLIDER) ||
                                        (u.type == MIDI::MidiUnitType::FADER) ||
                                        (u.type == MIDI::MidiUnitType::KNOB) ||
                                        (u.type == MIDI::MidiUnitType::SLIDERINVERT) ||
                                        (u.type == MIDI::MidiUnitType::FADERINVERT) ||
                                        (u.type == MIDI::MidiUnitType::KNOBINVERT)) {
                                        if (!MIDI::MidiSetter::SetVolume(u, m.value()))
                                            break;
                                    }
                                    else break;

                                    unitref.set(u, pcb->GetType());
                                    try {
                                        callOut2Cb_t f = pcb->GetCbOut2();
                                        if (f) f(u, t);
                                    }
                                    catch (std::bad_function_call& e) {
                                        to_log__(__FUNCTIONW__, e.what(), EXCEPT_CB2, pcb);
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                        case ClassLightKey: {

                            for (auto& u : cnf_->units) {
                                if (u.key == m.key() && u.scene == m.scene()) {
                                    #if !defined(DEBUG_NO_TARGET)
                                    if ((u.target != MIDI::Mackie::Target::LIGHTKEY8B) && (u.target != MIDI::Mackie::Target::LIGHTKEY16B)) break;
                                    #endif

                                    #if defined(DEBUG_BRIDGE_OUT)
                                    to_log::Get() << (log_string() << L"\t\t(ClassLightKey) = " << u.Dump());
                                    #endif

                                    unitref.begin();

                                    if (!MIDI::MidiSetter::ValidTarget(u))
                                        break;

                                    if ((u.type == MIDI::MidiUnitType::BTN) || (u.type == MIDI::MidiUnitType::BTNTOGGLE)) {
                                        if (!MIDI::MidiSetter::ÑhatterButton(u, m, cnf_.get()->midiconf.get_interval()))
                                            break;

                                        MIDI::MidiSetter::SetButton(u);
                                    }
                                    else if (
                                        (u.type == MIDI::MidiUnitType::SLIDER) ||
                                        (u.type == MIDI::MidiUnitType::FADER) ||
                                        (u.type == MIDI::MidiUnitType::KNOB) ||
                                        (u.type == MIDI::MidiUnitType::SLIDERINVERT) ||
                                        (u.type == MIDI::MidiUnitType::FADERINVERT) ||
                                        (u.type == MIDI::MidiUnitType::KNOBINVERT)) {
                                        if (!MIDI::MidiSetter::SetVolume(u, m.value()))
                                            break;
                                    }
                                    else break;

                                    unitref.set(u, pcb->GetType());
                                    try {
                                        callOut2Cb_t f = pcb->GetCbOut2();
                                        if (f) f(u, t);
                                    } catch (std::bad_function_call& e) {
                                        to_log__(__FUNCTIONW__, e.what(), EXCEPT_CB2, pcb);
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                        case ClassMqttKey: {

                            for (auto& u : cnf_->units) {
                                if (u.key == m.key() && u.scene == m.scene()) {
                                    #if !defined(DEBUG_NO_TARGET)
                                    if (u.target != MIDI::Mackie::Target::MQTTKEY) break;
                                    #endif

                                    #if defined(DEBUG_BRIDGE_OUT)
                                    to_log::Get() << (log_string() << L"\t\t(ClassMqttKey) = " << u.Dump());
                                    #endif

                                    unitref.begin();

                                    if (!MIDI::MidiSetter::ValidTarget(u))
                                        break;

                                    if ((u.type == MIDI::MidiUnitType::BTN) || (u.type == MIDI::MidiUnitType::BTNTOGGLE)) {
                                        if (!MIDI::MidiSetter::ÑhatterButton(u, m, cnf_.get()->midiconf.get_interval()))
                                            break;

                                        MIDI::MidiSetter::SetButton(u);
                                    }
                                    else if (
                                        (u.type == MIDI::MidiUnitType::SLIDER) ||
                                        (u.type == MIDI::MidiUnitType::FADER) ||
                                        (u.type == MIDI::MidiUnitType::KNOB) ||
                                        (u.type == MIDI::MidiUnitType::SLIDERINVERT) ||
                                        (u.type == MIDI::MidiUnitType::FADERINVERT) ||
                                        (u.type == MIDI::MidiUnitType::KNOBINVERT)) {
                                        if (!MIDI::MidiSetter::SetVolume(u, m.value()))
                                            break;
                                    }
                                    else break;

                                    unitref.set(u, pcb->GetType());
                                    try {
                                        callOut2Cb_t f = pcb->GetCbOut2();
                                        if (f) f(u, t);
                                    } catch (std::bad_function_call& e) {
                                        to_log__(__FUNCTIONW__, e.what(), EXCEPT_CB2, pcb);
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                        case ClassMediaKey: {

                            for (auto& u : cnf_->units) {
                                if (u.key == m.key() && u.scene == m.scene()) {
                                    #if !defined(DEBUG_NO_TARGET)
                                    if (u.target != MIDI::Mackie::Target::MEDIAKEY) break;
                                    #endif

                                    #if defined(DEBUG_BRIDGE_OUT)
                                    to_log::Get() << (log_string() << L"\t\t(ClassMediaKey) = " << u.Dump());
                                    #endif

                                    unitref.begin();

                                    if (!MIDI::MidiSetter::ValidTarget(u))
                                        break;

                                    if (!MIDI::MidiSetter::ÑhatterButton(u, m, cnf_.get()->midiconf.get_interval()))
                                        break;

                                    MIDI::MidiSetter::SetButton(u);
                                    unitref.set(u, pcb->GetType());
                                    try {
                                        callOut2Cb_t f = pcb->GetCbOut2();
                                        if (f) f(u, t);
                                    }
                                    catch (std::bad_function_call& e) {
                                        to_log__(__FUNCTIONW__, e.what(), EXCEPT_CB2, pcb);
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                        case ClassOutMidiMackie: {

                            for (auto& u : cnf_->units) {
                                if (u.key == m.key() && u.scene == m.scene()) {
                                    
                                    unitref.begin();

                                    #if !defined(DEBUG_NO_TARGET)
                                    bool check = false;
                                    switch (u.target) {
                                        using enum MIDI::Mackie::Target;
                                        case VOLUMEMIX:
                                        case MEDIAKEY:
                                        case NOTARGET: {
                                            break;
                                        }
                                        default: {
                                            if ((u.scene == 255) || (u.type == MIDI::MidiUnitType::UNITNONE)) break;
                                            check = true;
                                            break;
                                        }
                                    }
                                    if (!check) break;
                                    #endif

                                    uint8_t newvalue = m.value();
                                    auto tm = std::chrono::high_resolution_clock::now();
                                    auto dtm = std::chrono::duration_cast<std::chrono::milliseconds>(tm - u.value.time);

                                    if (u.type == MIDI::MidiUnitType::BTNTOGGLE) {
                                        if (dtm < cnf_.get()->midiconf.get_interval()) {
                                            u.value.time = tm;
                                            break;
                                        }
                                        u.value.lvalue = (newvalue > 0);
                                        u.value.type = MIDI::Mackie::ClickType::ClickTrigger;
                                    }
                                    else if (u.type == MIDI::MidiUnitType::BTN) {
                                        if (newvalue > 0) {
                                            u.value.time = tm;
                                            break;
                                        } else {
                                            if (dtm < cnf_.get()->midiconf.get_interval()) {
                                                u.value.time = tm;
                                                break;
                                            }
                                            else if (dtm >= cnf_.get()->midiconf.get_long_interval()) {
                                                if (u.longtarget == MIDI::Mackie::Target::NOTARGET)
                                                    u.value.type = MIDI::Mackie::ClickType::ClickOnce;
                                                else
                                                    u.value.type = MIDI::Mackie::ClickType::ClickLong;
                                            }
                                            else {
                                                u.value.type = MIDI::Mackie::ClickType::ClickOnce;
                                            }
                                            u.value.lvalue = true;
                                        }
                                    }
                                    else if ((u.type == MIDI::MidiUnitType::SLIDER) || (u.type == MIDI::MidiUnitType::FADER)) {
                                        if (u.value.value == newvalue) break;
                                        switch (u.target) {
                                            using enum MIDI::Mackie::Target;
                                            case AP1:
                                            case AP2:
                                            case AP3:
                                            case AP4:
                                            case AP5:
                                            case AP6:
                                            case AP7:
                                            case AP8:
                                            case XP9: {
                                                u.value.lvalue = (u.value.value > newvalue);
                                                break;
                                            }
                                            default: break;
                                        }
                                    }
                                    else if ((u.type == MIDI::MidiUnitType::SLIDERINVERT) || (u.type == MIDI::MidiUnitType::FADERINVERT)) {
                                        newvalue = MAX_MIDI_VALUE - m.value();
                                        if (u.value.value == newvalue) break;
                                        switch (u.target) {
                                            using enum MIDI::Mackie::Target;
                                            case AV1:
                                            case AV2:
                                            case AV3:
                                            case AV4:
                                            case AV5:
                                            case AV6:
                                            case AV7:
                                            case AV8:
                                            case XV9:
                                            case AP1:
                                            case AP2:
                                            case AP3:
                                            case AP4:
                                            case AP5:
                                            case AP6:
                                            case AP7:
                                            case AP8:
                                            case XP9: {
                                                u.value.lvalue = (u.value.value < newvalue);
                                                break;
                                            }
                                            default: break;
                                        }
                                    }
                                    else if ((u.type == MIDI::MidiUnitType::KNOB) || (u.type == MIDI::MidiUnitType::KNOBINVERT)) {
                                        if (u.value.value == newvalue) break;
                                        u.value.lvalue = (u.type == MIDI::MidiUnitType::KNOBINVERT) ? (newvalue > u.value.value) : (newvalue < u.value.value);
                                    }
                                    u.value.value = newvalue;
                                    u.value.time  = tm;
                                    unitref.set(u, pcb->GetType());
                                    try {
                                        callOut2Cb_t f = pcb->GetCbOut2();
                                        if (f) f(u, t);
                                    }
                                    catch (std::bad_function_call& e) {
                                        to_log__(__FUNCTIONW__, e.what(), EXCEPT_CB2, pcb);
                                    }
                                }
                            }
                            break;
                        }
                        default: break;
                    }
                    /* switch end */
                }
                /* for end */
            } catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
        }
    }
}

