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
using namespace std::placeholders;

namespace Common {
    namespace MIDI {

        MidiControllerBase::MidiControllerBase(IO::PluginCb& cb, std::shared_ptr<MidiDriver>& drv)
            : isenable_(false), isconnect_(false), cb_(cb), mdrv_(drv) {
        }
        MidiControllerBase::MidiControllerBase(IO::PluginCb& cb, std::shared_ptr<MidiDriver>& drv, std::wstring dev)
            : isenable_(false), isconnect_(false), cb_(cb), mdrv_(drv), active_device_(dev) {
        }

        const bool MidiControllerBase::IsEmpty() {
            return active_device_.empty();
        }
        const bool MidiControllerBase::IsEnable() {
            return isenable_;
        }
        const bool MidiControllerBase::IsConnected() {
            return isconnect_;
        }

        std::wstring& MidiControllerBase::GetActiveDevice() {
            return std::ref(active_device_);
        }

        #pragma region CALLBACK
        void CALLBACK MidiControllerBase::MidiInProc_(HMIDIIN h, UINT m, DWORD_PTR dwi, DWORD p1, DWORD p2) {
            MidiControllerBase::MidiProc_(m, dwi, p1, p2);
        }
        void CALLBACK MidiControllerBase::MidiOutProc_(HMIDIOUT h, UINT m, DWORD_PTR dwi, DWORD p1, DWORD p2) {
            MidiControllerBase::MidiProc_(m, dwi, p1, p2);
        }
        void CALLBACK MidiControllerBase::MidiProc_(UINT m, DWORD_PTR dwi, DWORD p1, DWORD p2) {
            MidiControllerBase* mcb = reinterpret_cast<MidiControllerBase*>(dwi);
            if (mcb == nullptr) return;

            switch (m) {
                case MOM_OPEN:
                case MIM_OPEN: {
                    to_log::Get() << log_string().to_log_format(
                        __FUNCTIONW__,
                        common_error_code::Get().get_error(common_error_id::err_MIDI_DEVICE_OPEN),
                        mcb->GetActiveDevice(), L"(system)"
                    );
                    mcb->isconnect_ = true;
                    break;
                }
                case MOM_CLOSE:
                case MIM_CLOSE: {
                    to_log::Get() << log_string().to_log_format(
                        __FUNCTIONW__,
                        common_error_code::Get().get_error(common_error_id::err_MIDI_DEVICE_CLOSE),
                        mcb->GetActiveDevice(), L"(system)"
                    );
                    mcb->isconnect_ = false;
                    break;
                }
                case MIM_DATA: {
                    mcb->cb_.GetCbIn1()(p1, p2);
                    break;
                }
                case MOM_DONE: {
                    to_log::Get() << log_string().to_log_format(
                        __FUNCTIONW__,
                        common_error_code::Get().get_error(common_error_id::err_MIDI_DEVICE_DONE),
                        mcb->GetActiveDevice()
                    );
                    break;
                }
                case MIM_ERROR:
                case MIM_MOREDATA:
                case MIM_LONGDATA:
                case MIM_LONGERROR:
                default: break;
            }
        }
        #pragma endregion
    }
}
