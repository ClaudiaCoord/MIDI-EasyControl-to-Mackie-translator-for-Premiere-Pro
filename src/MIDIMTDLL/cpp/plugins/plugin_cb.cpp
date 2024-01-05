/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL + MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace IO {

		using namespace std::placeholders;

		PluginCb::PluginCb() {
			id_ = 0U;
			type_ = PluginClassTypes::ClassNone;
			using_ = PluginCbType::None;
			set_cb_default_();
		}

		std::wstring PluginCb::dump() {
			return (log_string()
				<< L"id: " << id_
				<< L", type: " << PluginHelper::GetClassTypes(type_)
				<< L", using: " << PluginHelper::GetCbType(using_).str() << L" (" << (uint32_t)using_ << L")\n"
				);
		}

		#pragma region Private
		void PluginCb::set_cb_default_() {
			log_cb_  = [](std::wstring&) {};
			pid_cb_  = [](std::wstring&) -> uint32_t { return 0U; };
			in1_cb_  = [](DWORD, DWORD) {};
			in2_cb_  = [](MIDI::Mackie::MIDIDATA, DWORD) {};
			out1_cb_ = [](MIDI::Mackie::MIDIDATA, DWORD) {};
			out2_cb_ = [](MIDI::MidiUnit&, DWORD) {};
		}
		bool PluginCb::class_filter_(PluginClassTypes t) {
			switch (t) {
				using enum PluginClassTypes;
				case ClassIn:
				case ClassOut:
				case ClassOut1:
				case ClassOut2:
				case ClassSys:
				case ClassMixer: return true;
				default: return false;
			}
		}
		#pragma endregion

		#pragma region Logical Getter
		const bool   PluginCb::empty() const {
			return ((using_ == PluginCbType::None) || (type_ == PluginClassTypes::ClassNone));
		}
		const bool   PluginCb::IsCbType(uint16_t k) const {
			return (!k || (using_ == PluginCbType::None)) ? false : ((static_cast<uint32_t>(using_) & k) == k);
		}
		const bool   PluginCb::IsCbType(PluginCbType t) const {
			return IsCbType(to_bit(t));
		}
		#pragma endregion

		#pragma region CB Events setter overrides
		void PluginCb::SetCbOut(PluginCb*) {
			throw make_common_error(common_error_id::err_NOT_IMPLEMENTED);
		}
		void PluginCb::RemoveCbOut(uint32_t) {
			throw make_common_error(common_error_id::err_NOT_IMPLEMENTED);
		}
		void PluginCb::SetCbConfig(PluginCb*) {
			throw make_common_error(common_error_id::err_NOT_IMPLEMENTED);
		}
		void PluginCb::RemoveCbConfig(uint32_t) {
			throw make_common_error(common_error_id::err_NOT_IMPLEMENTED);
		}
		void PluginCb::SetCbOutLog(PluginCb*) {
			throw make_common_error(common_error_id::err_NOT_IMPLEMENTED);
		}
		void PluginCb::RemoveCbOutLog(uint32_t) {
			throw make_common_error(common_error_id::err_NOT_IMPLEMENTED);
		}
		void PluginCb::SetCbPid(PluginCb*) {
			throw make_common_error(common_error_id::err_NOT_IMPLEMENTED);
		}
		void PluginCb::RemoveCbPid(uint32_t) {
			throw make_common_error(common_error_id::err_NOT_IMPLEMENTED);
		}
		#pragma endregion

		#pragma region CB functions Getter
		callOut1Cb_t PluginCb::GetCbOut1() {
			return out1_cb_;
		}
		callOut2Cb_t PluginCb::GetCbOut2() {
			return out2_cb_;
		}
		callIn1Cb_t  PluginCb::GetCbIn1() {
			return in1_cb_;
		}
		callIn2Cb_t  PluginCb::GetCbIn2() {
			return in2_cb_;
		}
		callToLog_t  PluginCb::GetCbLog() {
			return log_cb_;
		}
		callToLogS_t PluginCb::GetCbLogS() {
			return logs_cb_;
		}
		callFromlog_t PluginCb::GetCbLogOut() {
			return logo_cb_;
		}
		callGetPid_t PluginCb::GetCbPid() {
			return pid_cb_;
		}
		callConfigCb_t PluginCb::GetCbConfig() {
			return cnf_cb_;
		}
		#pragma endregion

		#pragma region CB values Getter
		const uint32_t PluginCb::GetId() const {
			return id_;
		}
		const PluginCbType PluginCb::GetUsing() const {
			return using_;
		}
		const PluginClassTypes PluginCb::GetType() const {
			return type_;
		}
		#pragma endregion
	}
}
