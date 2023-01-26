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
#define MAX_MIDI_VALUE 127

// #define DEBUG_NO_TARGET 1

namespace Common {
    namespace MIDI {

        using namespace std::string_view_literals;

        constexpr std::wstring_view EXCEPT_CB1 = L" - call function pointer \"CbOut1\" not valid! = "sv;
        constexpr std::wstring_view EXCEPT_CB2 = L" - call function pointer \"CbOut2\" not valid! = "sv;
        
        bridge_out_event::bridge_out_event(std::shared_ptr<MidiDevice>& cnf) : cnf__(cnf) {
        }
        void bridge_out_event::SetConfig(std::shared_ptr<MidiDevice>& cnf) {
            cnf__ = cnf;
        }

        void bridge_out_event::add(MidiInstance & mi) {
            if (mi.GetId() == 0U) return;
            event__.add(mi, mi.GetId());
        }
        void bridge_out_event::add(MidiControllerBase& mcb) {
            if (mcb.GetId() == 0U) return;
            event__.add(MidiInstance(mcb), mcb.GetId());
        }
        void bridge_out_event::remove(uint32_t id) {
            event__.remove(id);
        }
        void bridge_out_event::remove(MidiInstance& mi) {
            if (mi.GetId() == 0U) return;
            event__.remove(mi.GetId());
        }
        void bridge_out_event::remove(MidiControllerBase& mcb) {
            if (mcb.GetId() == 0U) return;
            event__.remove(mcb.GetId());
        }

        void bridge_out_event::clear() {
            event__.clear();
        }

        void bridge_out_event::CallCbOut(Mackie::MIDIDATA& m, DWORD& t) {
            try {

                MidiUnitRef unitref;
                auto& list = event__.get();
                for (auto& pair : list) {
                    MidiInstance& mi = std::ref(pair.second);

                    #if defined(DEBUG_BRIDGE_OUT)
                    Common::to_log::Get() << (log_string() << L"\t\t(CallCbOut) = " << (unsigned long)a.GetId() << L" [" << a.GetType() << L"],");
                    Common::to_log::Get() << L"\t\t\t\t" << m.Dump();
                    #endif

                    if (unitref.isbegin()) {
                        if (!unitref.isvalid()) continue;

                        switch (unitref.type()) {
                            case ClassTypes::ClassProxy:
                            case ClassTypes::ClassMonitor: {
                                try {
                                    mi.GetCbOut1()(m, t);
                                }
                                catch (std::bad_function_call& e) {
                                    Common::to_log::Get() << (log_string() << __FUNCTIONW__ << Utils::DefaulPrefixError() << mi.GetType() << EXCEPT_CB1 << mi.GetId() << Utils::DefaulPrefixError() << e.what());
                                }
                                break;
                            }
                            case ClassTypes::ClassMixer:
                            case ClassTypes::ClassMediaKey:
                            case ClassTypes::ClassOutMidiMackie: {
                                try {
                                    mi.GetCbOut2()(unitref.get(), t);
                                }
                                catch (std::bad_function_call& e) {
                                    Common::to_log::Get() << (log_string() << __FUNCTIONW__ << Utils::DefaulPrefixError() << mi.GetType() << EXCEPT_CB2 << mi.GetId() << Utils::DefaulPrefixError() << e.what());
                                }
                                break;
                            }
                            default: break;
                        }
                        continue;
                    }

                    switch (mi.GetType()) {
                        case ClassTypes::ClassProxy:
                        case ClassTypes::ClassMonitor: {
                            try {
                                mi.GetCbOut1()(m, t);
                            }
                            catch (std::bad_function_call& e) {
                                Common::to_log::Get() << (log_string() << __FUNCTIONW__ << Utils::DefaulPrefixError() << mi.GetType() << EXCEPT_CB1 << mi.GetId() << Utils::DefaulPrefixError() << e.what());
                            }
                            break;
                        }
                        case ClassTypes::ClassMixer: {

                            for (auto& u : cnf__->units) {

                                if (u.key == m.key() && u.scene == m.scene()) {
                                    if (u.target != Mackie::Target::VOLUMEMIX) break;

                                    unitref.begin();

                                    if (u.appvolume.empty())
                                        break;

                                    if (!MidiSetter::ValidTarget(u))
                                        break;

                                    if ((u.type == MidiUnitType::BTN) || (u.type == MidiUnitType::BTNTOGGLE)) {
                                        if (!MidiSetter::ÑhatterButton(u, m, t, cnf__.get()->btninterval))
                                            break;

                                        MidiSetter::SetButton(u);
                                    }
                                    else if ((u.type == MidiUnitType::SLIDER) || (u.type == MidiUnitType::FADER)) {
                                        if (!MidiSetter::SetVolume(u, t, m.value()))
                                            break;
                                    }
                                    else if ((u.type == MidiUnitType::SLIDERINVERT) || (u.type == MidiUnitType::FADERINVERT)) {
                                        if (!MidiSetter::SetVolume(u, t, m.value()))
                                            break;
                                    }
                                    else if ((u.type == MidiUnitType::KNOB) || (u.type == MidiUnitType::KNOBINVERT)) {
                                        if (!MidiSetter::SetVolume(u, t, m.value()))
                                            break;
                                    }

                                    unitref.set(u, mi.GetType());
                                    try {
                                        mi.GetCbOut2()(u, t);
                                    }
                                    catch (std::bad_function_call& e) {
                                        Common::to_log::Get() << (log_string() << __FUNCTIONW__ << Utils::DefaulPrefixError() << mi.GetType() << EXCEPT_CB2 << mi.GetId() << Utils::DefaulPrefixError() << e.what());
                                    }
                                }
                            }
                            break;
                        }
                        case ClassTypes::ClassMediaKey: {

                            for (auto& u : cnf__->units) {
                                if (u.key == m.key() && u.scene == m.scene()) {
                                    #if !defined(DEBUG_NO_TARGET)
                                    if (u.target != Mackie::Target::MEDIAKEY) break;
                                    #endif

                                    #if defined(DEBUG_BRIDGE_OUT)
                                    Common::to_log::Get() << (log_string() << L"\t\t(ClassMediaKey) = " << u.Dump());
                                    #endif

                                    unitref.begin();

                                    if (!MidiSetter::ValidTarget(u))
                                        break;

                                    if (!MidiSetter::ÑhatterButton(u, m, t, cnf__.get()->btninterval))
                                        break;

                                    MidiSetter::SetButton(u);
                                    unitref.set(u, mi.GetType());
                                    try {
                                        mi.GetCbOut2()(u, t);
                                    }
                                    catch (std::bad_function_call& e) {
                                        Common::to_log::Get() << (log_string() << __FUNCTIONW__ << L" [" << mi.GetType() << EXCEPT_CB2 << mi.GetId() << L" : " << e.what());
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                        case ClassTypes::ClassOutMidiMackie: {

                            for (auto& u : cnf__->units) {
                                if (u.key == m.key() && u.scene == m.scene()) {
                                    
                                    unitref.begin();

                                    #if !defined(DEBUG_NO_TARGET)
                                    bool check = false;
                                    switch (u.target) {
                                        case Mackie::Target::VOLUMEMIX:
                                        case Mackie::Target::MEDIAKEY:
                                        case Mackie::Target::NOTARGET: {
                                            break;
                                        }
                                        default: {
                                            if ((u.scene == 255) || (u.type == MidiUnitType::UNITNONE)) break;
                                            check = true;
                                            break;
                                        }
                                    }
                                    if (!check) break;
                                    #endif

                                    uint8_t  newvalue = m.value();

                                    if (u.type == MidiUnitType::BTNTOGGLE) {
                                        if (t - u.value.time < cnf__.get()->btninterval) {
                                            u.value.time = t;
                                            break;
                                        }
                                        u.value.lvalue = (newvalue > 0);
                                        u.value.type = Mackie::ClickType::ClickTrigger;
                                    }
                                    else if (u.type == MidiUnitType::BTN) {
                                        if (newvalue > 0) {
                                            u.value.time = t;
                                            break;
                                        }
                                        else {
                                            uint32_t ctime = (t - u.value.time);
                                            if (ctime < cnf__.get()->btninterval) {
                                                u.value.time = t;
                                                break;
                                            }
                                            else if (ctime >= cnf__.get()->btnlonginterval) {
                                                if (u.longtarget == Mackie::Target::NOTARGET)
                                                    u.value.type = Mackie::ClickType::ClickOnce;
                                                else
                                                    u.value.type = Mackie::ClickType::ClickLong;
                                            }
                                            else {
                                                u.value.type = Mackie::ClickType::ClickOnce;
                                            }
                                            u.value.lvalue = true;
                                        }
                                    }
                                    else if ((u.type == MidiUnitType::SLIDER) || (u.type == MidiUnitType::FADER)) {
                                        if (u.value.value == newvalue) break;
                                        switch (u.target) {
                                            case Mackie::Target::AP1:
                                            case Mackie::Target::AP2:
                                            case Mackie::Target::AP3:
                                            case Mackie::Target::AP4:
                                            case Mackie::Target::AP5:
                                            case Mackie::Target::AP6:
                                            case Mackie::Target::AP7:
                                            case Mackie::Target::AP8:
                                            case Mackie::Target::XP9: {
                                                u.value.lvalue = (u.value.value > newvalue);
                                                break;
                                            }
                                            default: break;
                                        }
                                    }
                                    else if ((u.type == MidiUnitType::SLIDERINVERT) || (u.type == MidiUnitType::FADERINVERT)) {
                                        newvalue = MAX_MIDI_VALUE - m.value();
                                        if (u.value.value == newvalue) break;
                                        switch (u.target) {
                                            case Mackie::Target::AV1:
                                            case Mackie::Target::AV2:
                                            case Mackie::Target::AV3:
                                            case Mackie::Target::AV4:
                                            case Mackie::Target::AV5:
                                            case Mackie::Target::AV6:
                                            case Mackie::Target::AV7:
                                            case Mackie::Target::AV8:
                                            case Mackie::Target::XV9:
                                            case Mackie::Target::AP1:
                                            case Mackie::Target::AP2:
                                            case Mackie::Target::AP3:
                                            case Mackie::Target::AP4:
                                            case Mackie::Target::AP5:
                                            case Mackie::Target::AP6:
                                            case Mackie::Target::AP7:
                                            case Mackie::Target::AP8:
                                            case Mackie::Target::XP9: {
                                                u.value.lvalue = (u.value.value < newvalue);
                                                break;
                                            }
                                            default: break;
                                        }
                                    }
                                    else if ((u.type == MidiUnitType::KNOB) || (u.type == MidiUnitType::KNOBINVERT)) {
                                        if (u.value.value == newvalue) break;
                                        u.value.lvalue = (u.type == MidiUnitType::KNOBINVERT) ? (newvalue > u.value.value) : (newvalue < u.value.value);
                                    }
                                    u.value.value = newvalue;
                                    u.value.time  = t;
                                    unitref.set(u, mi.GetType());
                                    try {
                                        mi.GetCbOut2()(u, t);
                                    }
                                    catch (std::bad_function_call& e) {
                                        Common::to_log::Get() << (log_string() << __FUNCTIONW__ << Utils::DefaulPrefixError() << mi.GetType() << EXCEPT_CB2 << mi.GetId() << Utils::DefaulPrefixError() << e.what());
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

