/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Joystick support.
	+ GamePad support.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (GamePad/Joistick Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace GAMEPAD {

		static inline uint16_t calc_polling__(JOYCAPSW& caps) {
			return caps.wPeriodMax - ((caps.wPeriodMax - caps.wPeriodMin) / 10);
		}

		#pragma region Joisticks Base
		JoystickConfig JoisticksBase::jsconfig_empty{};

		JoisticksBase::JoisticksBase() : config_(JoisticksBase::jsconfig_empty), clz_(reinterpret_cast<DWORD_PTR>(this)) {
		}
		JoisticksBase::~JoisticksBase() {
			dispose_(false);
		}

		#pragma region private
		void JoisticksBase::dispose_(const bool iserr) {
			try {
				if (data.empty()) {
					if (iserr && hwnj_) hwnj_.reset();
					return;
				}
				if (iserr && hwnj_) {
					std::vector<JoistickData> data_(data);
					data.clear();

					UI::UiUtils::Post(hwnj_, [=]() {
							for (auto& a : data_)
								if ((a.state == JOYERR_NOERROR) && (!a.is_offline)) {
									MMRESULT r = ::joyReleaseCapture(a.id);
									print_state_(r);
								}
							::PostMessageW(hwnj_.get(), WM_DESTROY, 0, 0);
							
						}
					);
				} else {
					std::vector<JoistickData> data_(data);
					data.clear();

					for (auto& a : data_)
						if ((a.state == JOYERR_NOERROR) && (!a.is_offline))
							(void) ::joyReleaseCapture(a.id);
					hwnj_.reset();
				}
			} catch (...) {}
			devs_started_ = 0U;
		}
		void JoisticksBase::print_state_(MMRESULT r) const {
			switch (r) {
				case MMSYSERR_NODRIVER: {
					log_cb(common_error_code::Get().get_error(common_error_id::err_JOY_NODRIVER));
					break;
				}
				case JOYERR_NOCANDO: {
					log_cb(common_error_code::Get().get_error(common_error_id::err_JOY_NOCANDO));
					break;
				}
				case JOYERR_UNPLUGGED: {
					log_cb(common_error_code::Get().get_error(common_error_id::err_JOY_UNPLUGGED));
					break;
				}
				case JOYERR_PARMS:
				case MMSYSERR_INVALPARAM: {
					log_cb(common_error_code::Get().get_error(common_error_id::err_JOY_INVALIDPARAM));
					break;
				}
				case JOYERR_NOERROR: {
					break;
				}
			}
		}
		const uint16_t JoisticksBase::build_devices_list_() {
			try {
				uint16_t cnt = 0U;
				data.clear();
				devs_total_ = ::joyGetNumDevs();
				for (uint16_t i = 0U; i < devs_total_; i++) {
					JoistickData jc = JoistickData(i);
					if (::joyGetDevCapsW(i, &jc.caps, sizeof(jc.caps)) == JOYERR_NOERROR) {
						JOYINFO ji{};
						jc.state = ::joyGetPos(i, &ji);
						jc.is_error = (jc.state != JOYERR_NOERROR) && (jc.state != JOYERR_UNPLUGGED);
						jc.is_offline = (jc.state == JOYERR_UNPLUGGED) || jc.is_error;
						if (!jc.is_error && !jc.is_offline) cnt++;
						data.push_back(std::move(jc));
					}
				}
				return cnt;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return 0U;
		}
		const uint16_t JoisticksBase::start_devices_(HWND hwnd) {
			try {
				if (!hwnd) return 0;
				if (empty()) {
					devs_online_ = build_devices_list_();
					if (empty()) return 0;
				}

				if (!UI::UiUtils::IsUIThread()) return 0;

				hwnj_.reset(
					hwnd, &JoisticksBase::event_callback_,
					clz_, 101 // wid_++
				);
				if (!hwnj_) return 0;

				uint16_t cnt{ 0 };

				for (auto& a : data)
					if (!a.is_offline) {
						uint16_t poll = (config_.polling > a.caps.wPeriodMax) ?
							calc_polling__(a.caps) :
							((config_.polling < a.caps.wPeriodMin) ? calc_polling__(a.caps) : config_.polling);

						a.state = ::joySetCapture(hwnj_, a.id, poll, true);
						switch (a.state) {
							case JOYERR_NOERROR: {
								cnt++;
								break;
							}
							default: {
								print_state_(a.state);
								break;
							}
						}
					} else print_state_(a.state);

				return cnt;

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return 0;
		}
		#pragma endregion

		const uint16_t JoisticksBase::TotalDevices() const {
			return devs_total_;
		}
		const uint16_t JoisticksBase::OnlineDevices() const {
			return devs_online_;
		}
		const uint16_t JoisticksBase::StartDevices() const {
			return devs_started_;
		}

		const bool JoisticksBase::empty() const {
			return data.empty() || devs_online_ == 0;
		}
		const bool JoisticksBase::init() {
			devs_online_ = build_devices_list_();
			return devs_online_ > 0;
		}
		const bool JoisticksBase::start(HWND hwnd, const JoystickConfig& cnf) {
			config_ = cnf;
			if (!hwnd || config_.empty()) return false;
			devs_started_ = start_devices_(hwnd);
			if (config_.rcontrols && (devs_started_ > 0)) jc_.update(cnf);
			return devs_started_ > 0;
		}
		void JoisticksBase::stop() {
			dispose_();
		}
		void JoisticksBase::update(std::shared_ptr<JSON::MMTConfig>& mmt) {
			jc_.update(mmt);
		}
		void JoisticksBase::clear() {
			jc_.clear();
		}

		LRESULT CALLBACK JoisticksBase::event_callback_(HWND hwnd, UINT c, WPARAM w, LPARAM l, UINT_PTR sc, DWORD_PTR data) {
			bool ismoved{ false };

			switch (c) {
				case (WM_APP + 2): {
					UI::UiUtils::PostExec(l);
					return static_cast<INT_PTR>(0);
				}
				case WM_DESTROY: {
					auto clz = reinterpret_cast<JoisticksBase*>(data);
					if (!clz) break;
					clz->hwnj_.reset();
					break;
				}
				case MM_JOY1MOVE:
				case MM_JOY2MOVE:
				case MM_JOY1ZMOVE:
				case MM_JOY2ZMOVE: 
				case MM_JOY1BUTTONUP:
				case MM_JOY2BUTTONUP:
				case MM_JOY1BUTTONDOWN:
				case MM_JOY2BUTTONDOWN: {

					if (!JoistickInput::filter(c, w, l, std::ref(ismoved)))
						return static_cast<INT_PTR>(0);

					auto clz = reinterpret_cast<JoisticksBase*>(data);
					if (!clz) break;

					MIDI::Mackie::MIDIDATA m = clz->ji_.parse(c, w, l, clz->config_);
					if ((m.data[0] == 0U) || (m.data[1] == 0U)) break;

					if (ismoved && clz->config_.directasbutton && !clz->config_.rcontrols) {
						clz->event_cb(m);
						std::this_thread::sleep_for(std::chrono::milliseconds(25));
						m.data[2] = 0U;
						clz->event_cb(std::move(m));
					} else if (clz->config_.rcontrols) {
						if (clz->jc_.change(m) && !m.empty())
							clz->event_cb(std::move(m));
					} else clz->event_cb(std::move(m));
					break;
				}
				default: break;
			}
			return ::DefSubclassProc(hwnd, c, w, l);
		}
		#pragma endregion
	}
}