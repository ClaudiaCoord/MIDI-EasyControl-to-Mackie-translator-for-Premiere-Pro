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
using namespace std::placeholders;

namespace Common {
    namespace MIDI {

        static const wchar_t* unsupport_code = L"unsupported implementation in code, method not overridden: ";
        inline void unsupported_code(std::wstring s) {
            throw runtime_werror(log_string() << unsupport_code << s);
        }

        MidiControllerBase::MidiControllerBase() : isenable__(false), isconnect__(false), this_type__(ClassTypes::ClassNone) {
            eventin__ = [](DWORD, DWORD) {};
            id__      = Utils::random_hash(this);
        }
        MidiControllerBase::~MidiControllerBase() {
            this->Dispose();
        }
        bool MidiControllerBase::BuildDeviceList(uint32_t&) {
            unsupported_code(__FUNCTIONW__);
            return false;
        }
        const bool MidiControllerBase::Start() {
            unsupported_code(__FUNCTIONW__);
            return false;
        }
        const bool MidiControllerBase::Start(std::shared_ptr<MidiDevice>& cnf) {
            unsupported_code(__FUNCTIONW__);
            return false;
        }
        void MidiControllerBase::Stop() {
            unsupported_code(__FUNCTIONW__);
        }
        void MidiControllerBase::Dispose() {
            isenable__ = isconnect__ = false;
        }

        const bool MidiControllerBase::IsEmpty() {
            return device_list__.empty();
        }
        const bool MidiControllerBase::IsEnable() {
            return isenable__;
        }
        const bool MidiControllerBase::IsConnected() {
            return isconnect__;
        }

        void MidiControllerBase::InCallbackSet(callMidiInCb f) {
            eventin__ = f;
        }
        void MidiControllerBase::InCallbackRemove() {
            eventin__ = [](DWORD, DWORD) {};
        }

        uint32_t MidiControllerBase::GetId() {
            return id__;
        }

        ClassTypes MidiControllerBase::GetType() {
            return this_type__;
        }
        std::wstring& MidiControllerBase::DeviceName() {
            return active_device__;
        }
        std::vector<std::wstring>& MidiControllerBase::GetDeviceList() {
            return std::ref(device_list__);
        }

        // CALLBACK OUT Getter //
        callMidiOut1Cb MidiControllerBase::GetCbOut1() {
            return std::bind(
                static_cast<const bool(MidiControllerBase::*)(Mackie::MIDIDATA&, DWORD&)>(&MidiControllerBase::SendToPort),
                this, _1, _2);
        }
        callMidiOut2Cb MidiControllerBase::GetCbOut2() {
            return std::bind(
                static_cast<const bool(MidiControllerBase::*)(MidiUnit&, DWORD&)>(&MidiControllerBase::SendToPort),
                this, _1, _2);
        }

        // CALLBACK //
        const bool    MidiControllerBase::SendToPort(Mackie::MIDIDATA& m, DWORD& t) {
            unsupported_code(__FUNCTIONW__);
            return false;
        }
        const bool    MidiControllerBase::SendToPort(MidiUnit&, DWORD& t) {
            unsupported_code(__FUNCTIONW__);
            return false;
        }
        void CALLBACK MidiControllerBase::MidiInProc(HMIDIIN h, UINT m, DWORD_PTR dwi, DWORD p1, DWORD p2) {
            MidiControllerBase::MidiProc(m, dwi, p1, p2);
        }
        void CALLBACK MidiControllerBase::MidiOutProc(HMIDIOUT h, UINT m, DWORD_PTR dwi, DWORD p1, DWORD p2) {
            MidiControllerBase::MidiProc(m, dwi, p1, p2);
        }
        void MidiControllerBase::MidiProc(UINT m, DWORD_PTR dwi, DWORD p1, DWORD p2) {
            MidiControllerBase* mcb = reinterpret_cast<MidiControllerBase*>(dwi);
            if (mcb == nullptr) return;

            switch (m)
            {
                case MOM_OPEN:
                case MIM_OPEN: {
                    Common::to_log::Get() << (log_string() << mcb->DeviceName() << " - was opened (system)");
                    mcb->isconnect__ = true;
                    break;
                }
                case MOM_CLOSE:
                case MIM_CLOSE: {
                    Common::to_log::Get() << (log_string() << mcb->DeviceName() << " - was closed (system)");
                    mcb->isconnect__ = false;
                    break;
                }
                case MIM_DATA: {
                    mcb->eventin__(p1, p2);
                    break;
                }
                case MOM_DONE: {
                    Common::to_log::Get() << (log_string() << mcb->DeviceName() << " - was done (system)");
                    break;
                }
                case MIM_ERROR:
                case MIM_MOREDATA:
                case MIM_LONGDATA:
                case MIM_LONGERROR:
                default: break;
            }
        }
    }
}
