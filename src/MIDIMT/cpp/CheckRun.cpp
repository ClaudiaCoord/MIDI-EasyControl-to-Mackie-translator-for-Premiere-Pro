/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "MIDIMT.h"

namespace Common {
    namespace MIDIMT {

        static CheckRun checkrunclass__{};

        static bool check_boolean_string_(std::wstring s) {
            return s.ends_with(L"yes") || s.ends_with(L"true");
        }

        CheckRun::CheckRun() : msgid__(RegisterWindowMessageW(L"MIDIMTAppAssoc")), confpath__({}) {
        }
        CheckRun::~CheckRun() {
            clear_();
        }
        CheckRun& CheckRun::Get() {
            return std::ref(checkrunclass__);
        }
        const uint32_t CheckRun::GetMsgId() const {
            return msgid__;
        }
        const std::wstring& CheckRun::GetConfigPath() const {
            return std::ref(confpath__);
        }
        const bool CheckRun::Begin() {
            try {
                const bool isparse = parse_();
                const bool isrun = check_(!isparse);
                if (isrun && isparse) calledit_();
                else if (isparse) setconfig_();
                return !isrun;

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return false;
        }

        const bool CheckRun::parse_() {
            try {
                clear_();

                int32_t na = 0;
                local_ptr_deleter<LPWSTR*> xargs(::CommandLineToArgvW(::GetCommandLineW(), &na));
                if (xargs.empty()) return false;
                common_config& conf = common_config::Get();
                for (int32_t i = 0; i < na; i++) {
                    if (!xargs.data[i]) continue;
                    std::wstring s = xargs.data[i];
                    if (s.starts_with(L"--conf=") && s.ends_with(L".cnf")) {
                        confpath__ = s.substr(7);
                    }
                    else if (s.starts_with(L"--autostart=")) {
                        conf.Registry.SetSysAutoBoot(check_boolean_string_(s));
                    }
                    else if (s.starts_with(L"--autorun=")) {
                        conf.Registry.SetAutoRun(check_boolean_string_(s));
                    }
                    else if (s.starts_with(L"--mixer=")) {
                        conf.Registry.SetMixerEnable(check_boolean_string_(s));
                    }
                    /*
                    else if (s.starts_with(L"--mmkey=")) {
                        conf.Registry.SetMMKeyEnable(check_boolean_string_(s));
                    }
                    else if (s.starts_with(L"--mqtt=")) {
                        conf.Registry.SetSmartHomeEnable(check_boolean_string_(s));
                    }
                    */
                    else if (s.starts_with(L"--log=")) {
                        conf.Registry.SetLogWrite(check_boolean_string_(s));
                    }
                }
                if (confpath__.empty() || !confpath__.ends_with(L".cnf")) return false;
                return true;

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return false;
        }
        const bool CheckRun::check_(bool isinfo) {
            try {
                LangInterface& lang = LangInterface::Get();
                HWND hwnd = FindWindowW(lang.GetMainClass().c_str(), lang.GetMainTitle().c_str());
                if (hwnd == nullptr) return false;
                if (isinfo) ::PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_GO_ABOUT, 0), (LPARAM)0);

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return true;
        }
        void CheckRun::setconfig_() {
            try {
                if (confpath__.empty() && !parse_()) return;
                LangInterface& lang = LangInterface::Get();
                if (::MessageBoxW(0,
                        log_string::format(lang.GetString(STRING_CHKR_MSG1), confpath__).c_str(),
                        lang.GetString(STRING_CHKR_MSG2).c_str(),
                        MB_YESNO | MB_ICONQUESTION) == IDYES)
                    common_config::Get().Registry.SetConfPath(confpath__);
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
        }
        void CheckRun::calledit_() {
            try {
                if (confpath__.empty() && !parse_()) return;
                (void) ::EnumWindows(CheckRun::broadcast_find_, (LPARAM)0);
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
        }
        void CheckRun::notify(HWND hwnd) {
            if ((!hwnd) || confpath__.empty()) return;
            try {
                COPYDATASTRUCT* data = CheckRun::build(msgid__, confpath__);
                if (!data) return;
                ::SendMessageW(hwnd, WM_COPYDATA, 0, reinterpret_cast<LPARAM>(data));
                delete [] data->lpData;
                delete data;

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
        }
        void CheckRun::clear_() {
            confpath__ = std::wstring();
        }

        BOOL CALLBACK CheckRun::broadcast_find_(HWND hwnd, LPARAM l) {
            try {
                wchar_t clzname[MAX_CLASS_NAME]{};
                if (::GetClassNameW(hwnd, clzname, _countof(clzname))) {
                    std::wstring s(clzname);
                    std::wstring& clsname = LangInterface::Get().GetMainClass();
                    if (s._Equal(clsname)) {
                        CheckRun::Get().notify(hwnd);
                        return false;
                    }
                }
            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return true;
        }
        COPYDATASTRUCT* CheckRun::build(COPYDATASTRUCT* cds) {
            try {
                if (!cds) return nullptr;
                COPYDATASTRUCT* data = new COPYDATASTRUCT();

                data->dwData = cds->dwData;
                data->cbData = cds->cbData;
                data->lpData = new wchar_t[data->cbData] {};
                if (data->lpData)
                    ::wcscpy_s((wchar_t*)data->lpData, data->cbData, (wchar_t*)cds->lpData);
                else {
                    delete data;
                    return nullptr;
                }
                return data;

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return nullptr;
        }
        COPYDATASTRUCT* CheckRun::build(uint32_t id, std::wstring s) {
            try {
                COPYDATASTRUCT* data = new COPYDATASTRUCT();

                data->dwData = id;
                #pragma warning( push )
                #pragma warning( disable : 4267 )
                data->cbData = (static_cast<DWORD>(s.size()) + 1) * sizeof(wchar_t);
                #pragma warning( pop )

                data->lpData = new wchar_t[data->cbData] {};
                if (data->lpData)
                    ::wcscpy_s((wchar_t*)data->lpData, data->cbData, s.data());
                else {
                    delete data;
                    return nullptr;
                }
                return data;

            } catch (...) {
                Utils::get_exception(std::current_exception(), __FUNCTIONW__);
            }
            return nullptr;
        }
    }
}
