/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023, MIT

	MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace IO {

		using namespace std::placeholders;
		using namespace std::string_view_literals;

		class FixedPlugingGUID {
		public:
			static constexpr std::string_view Clsid[4] = {
				"{856364ff-cb6e-464c-9041-f18758158c48}"sv,
				"{856364ff-cb6e-464c-9042-f18758158c48}"sv,
				"{856364ff-cb6e-464c-9043-f18758158c48}"sv,
				"{856364ff-cb6e-464c-9044-f18758158c48}"sv
			};
		};

		static IOBridge io_bridge__{};

		plugin_t& IOBridge::operator[] (IO::FixedPlugin fp) {
			return plugins_->get_plugin<GUID, PluginTypeId::BY_GUID>(
				Utils::guid_from_string(
					FixedPlugingGUID::Clsid[static_cast<uint16_t>(fp)].data()
				)
			);
		}
		plugin_t& IOBridge::operator[] (uint16_t idx) {
			return plugins_->get_plugin_index(idx);
		}
		plugin_t& IOBridge::operator[] (std::wstring s) {
			return plugins_->get_plugin<std::wstring, PluginTypeId::BY_PATH>(s);
		}

		IOBridge::IOBridge()
			: plugins_(std::make_unique<Plugins>()),
			  event_sys_(std::make_shared<bridge_sys>()),
			  event_out_(std::make_shared<bridge_out_event>()),
			  event_in_(std::make_shared<bridge_in_event>(event_out_)) {

			event_in_->GetCb().SetCb(event_sys_->GetCb());
			event_out_->GetCb().SetCb(event_sys_->GetCb());
		}
		IOBridge::~IOBridge() {
			dispose_();
		}

		void IOBridge::dispose_() {
			try {
				plugins_->release(event_sys_, event_in_, event_out_);
				plugins_.reset();

				event_in_->GetCb().UnSetCb(event_sys_->GetCb());
				event_in_.reset();

				event_out_->GetCb().UnSetCb(event_sys_->GetCb());
				event_out_.reset();

				event_sys_.reset();
			} catch (...) {}
		}

		const bool   IOBridge::IsLoaded() const {
			return (state_.at(0)) > 0U && (state_.at(1) > 0U);
		}
		const bool   IOBridge::IsStarted() const {
			return (state_.at(2) > 0U) && (state_.at(3) == 0U);
		}
		const bool   IOBridge::IsStoped() const {
			return (state_.at(3) > 0U) && (state_.at(2) == 0U);
		}

		const bool   IOBridge::PluginEmpty() const {
			return plugins_->empty();
		}
		const bool   IOBridge::PluginEnabled() const {
			return plugins_->enabled();
		}
		const size_t IOBridge::PluginCount() {
			return plugins_->count();
		}
		const size_t IOBridge::PluginCountIn() {
			return plugins_->count_in();
		}
		const size_t IOBridge::PluginCountOut() {
			return plugins_->count_out();
		}

		const bool   IOBridge::Init() {
			try {
				if (!plugins_->empty()) {
					to_log::Get() << log_string().to_log_string(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_PLUGIN_ALREADY_LOADED));
					return true;
				}
				state_.fill(0U);
				state_.at(0) = plugins_->init(event_sys_, event_in_, event_out_);
				if (!state_.at(0)) {
					to_log::Get() << log_string().to_log_string(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_BRIDGE_NOT_INIT));
					return false;
				}

				auto& cnf = common_config::Get().GetConfig();
				state_.at(1) = plugins_->load(cnf);
				if (!state_.at(1)) {
					to_log::Get() << log_string().to_log_format(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_BRIDGE_NOT_LOAD), state_.at(0));
					return false;
				}
				to_log::Get() << log_string().to_log_format(__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_BRIDGE_OK_INIT_LOAD), state_.at(0), state_.at(1));
				return true;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		const bool   IOBridge::Start() {
			try {
				if (IsStarted()) {
					to_log::Get() << log_string().to_log_string(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_PLUGIN_ALREADY_STARTED));
					return true;
				}
				state_.at(2) = plugins_->start();
				if (!state_.at(2)) {
					to_log::Get() << log_string().to_log_format(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_BRIDGE_NOT_START), state_.at(0), state_.at(1));
					return false;
				}
				to_log::Get() << log_string().to_log_format(__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_BRIDGE_OK_START), state_.at(0), state_.at(1), state_.at(2));
				state_.at(3) = 0U;
				return true;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		const bool   IOBridge::Stop() {
			try {
				if (!IsStarted()) {
					to_log::Get() << log_string().to_log_string(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_PLUGIN_ALREADY_STOPED));
					return true;
				}
				state_.at(3) = plugins_->stop();
				if (!state_.at(3)) {
					to_log::Get() << log_string().to_log_format(__FUNCTIONW__,
						common_error_code::Get().get_error(common_error_id::err_BRIDGE_NOT_STOP), state_.at(0), state_.at(1), state_.at(2));
					return false;
				}
				to_log::Get() << log_string().to_log_format(__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_BRIDGE_OK_STOP), state_.at(0), state_.at(1), state_.at(2), state_.at(3));
				state_.at(2) = 0U;
				return true;
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		const bool   IOBridge::Reload() {
			try {
				if (IsStarted()) Stop();
				dispose_();

				state_.fill(0U);
				plugins_   = std::make_unique<Plugins>();
				event_sys_ = std::make_shared<bridge_sys>();
				event_out_ = std::make_shared<bridge_out_event>();
				event_in_  = std::make_shared<bridge_in_event>(event_out_);

				event_in_->GetCb().SetCb(event_sys_->GetCb());
				event_out_->GetCb().SetCb(event_sys_->GetCb());

				return Init();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}

		void IOBridge::SetCb(IO::PluginCb& pcb) {
			try {
				pcb.SetCb(event_sys_->GetCb());
				pcb.SetCb(event_out_->GetCb());
				pcb.SetCb(event_in_->GetCb());

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void IOBridge::UnSetCb(IO::PluginCb& pcb) {
			try {
				pcb.UnSetCb(event_in_->GetCb());
				pcb.UnSetCb(event_out_->GetCb());
				pcb.UnSetCb(event_sys_->GetCb());

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		plugin_t& IOBridge::GetEmptyPlugin() {
			return plugins_->get_empty_plugin();
		}

		std::array<uint32_t, 4>& IOBridge::GetState() {
			return std::ref(state_);
		}
		IO::IOBridge& IOBridge::Get() {
			return std::ref(io_bridge__);
		}
	}
}
