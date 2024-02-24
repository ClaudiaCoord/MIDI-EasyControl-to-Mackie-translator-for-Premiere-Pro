/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace SCRIPT {

		VmScript::VmScript(
			const ScriptEntry& se,
			std::shared_ptr<chaiscript::ChaiScript> cs,
			std::function<void(const uint32_t)> hcb,
			std::function<void(const std::wstring&)> pcb)
			: chai_(std::move(cs)), entry_(se), pack_cb_(hcb), print_cb_(pcb) {

			if (entry_.body.empty())
				throw make_common_error(common_error_id::err_SCRIPT_EMPTY);

			chai_->add(chaiscript::fun([=]() -> std::string { return entry_.name; }), "ScriptName");
			chai_->add(chaiscript::fun([=]() -> uint32_t { return entry_.hash; }), "ScriptHash");
			chai_->add(chaiscript::fun(static_cast<const bool(VmScript::*)()>(&VmScript::iscanceled), this), "IsCanceled");
		}
		VmScript::~VmScript() {
			dispose_();
			try {
				chai_.reset();
			} catch (...) {}
		}
		void VmScript::dispose_() {
			try {
				if (task_.joinable()) {
					std::stop_source s_source = task_.get_stop_source();
					if (s_source.stop_possible())
						s_source.request_stop();

					if (task_.joinable()) {
						if (auto h = task_.native_handle(); h)
							#pragma warning( push )
							#pragma warning( disable : 6258 )
							::TerminateThread(h, 1);
							#pragma warning( pop )
						task_.join();
					}
				}
			} catch (...) {}
		}
		void VmScript::exec_() {
			try {
				chai_->eval(entry_.body);

			} catch (const chaiscript::exception::bad_boxed_cast& e) {
				print_cb_(log_string().to_log_format(__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_SCRIPT_ERROR),
					Utils::to_string(e.what()))
				);

			} catch (const chaiscript::exception::eval_error& e) {
				print_cb_(log_string().to_log_format(__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_SCRIPT_ERROR),
					Utils::to_string(e.pretty_print()))
				);

			} catch (const chaiscript::exception::name_conflict_error& e) {
				print_cb_(log_string().to_log_format(__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_SCRIPT_ERROR),
					Utils::to_string(e.what()))
				);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		const bool VmScript::start() {
			try {
				if (entry_.body.empty()) return false;
				task_ = std::jthread(
					[=](std::stop_token stoken) {
						exec_();
						isended_.store(true, std::memory_order_release);
						try {
							chai_.reset();
							pack_cb_(entry_.hash);
						} catch (...) {
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						}
					}
				);
				return task_.joinable();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		void VmScript::stop() {
			try {
				if (task_.joinable()) dispose_();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		const bool VmScript::equals(const std::string& s) const {
			return !entry_.name.empty() && entry_.name._Equal(s);
		}
		const bool VmScript::isrun() const {
			return task_.joinable() && !isended_.load(std::memory_order_acquire);
		}
		const bool VmScript::empty() const {
			return entry_.body.empty();
		}
		const bool VmScript::isend() const {
			return isended_.load(std::memory_order_acquire);
		}
		const bool VmScript::iscanceled() {
			try {
				if (!task_.joinable()) return false;

				std::stop_token stoken = task_.get_stop_token();
				return stoken.stop_requested();

			} catch (...) {}
			return false;
		}

		const uint32_t VmScript::hash() const {
			return entry_.hash;
		}
		const std::string& VmScript::name() const {
			return std::ref(entry_.name);
		}
		std::wstring VmScript::namew() const {
			if (entry_.name.empty()) return std::wstring();
			return Utils::to_string(entry_.name);
		}
	}
}