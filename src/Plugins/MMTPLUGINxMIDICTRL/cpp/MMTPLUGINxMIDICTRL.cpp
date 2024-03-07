/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (MidiCtrlPlugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace PLUGINS {

		#include "plugininfo.h"

		using namespace Common::IO;
		using namespace std::placeholders;
		using namespace std::string_view_literals;

		static constexpr std::wstring_view MidiMackiePort = L"MIDI-MT-Mackie-Out"sv;
		static constexpr std::wstring_view MidiProxyPort = L"MIDI-MT-Proxy-Out-"sv;

		MidiCtrlPlugin::MidiCtrlPlugin(std::wstring path, HWND hwnd)
			: plugin_ui_(*static_cast<PluginCb*>(this), hwnd),
			  IO::Plugin(
				Utils::to_hash(path), DLG_PLUG_MIDI_WINDOW,
				Plugin::GuidFromString(_PLUGINGUID),
				path,
				PLUG_HEADER,
				PLUG_DESCRIPTION,
				this,
				IO::PluginClassTypes::ClassMidi,
				(IO::PluginCbType::In1Cb | IO::PluginCbType::Out1Cb | IO::PluginCbType::Out2Cb | IO::PluginCbType::LogCb | PluginCbType::LogsCb | IO::PluginCbType::ConfCb),
				hwnd
			  ) {
			PluginCb::out1_cb_ = std::bind(static_cast<void(MidiCtrlPlugin::*)(MIDI::Mackie::MIDIDATA, DWORD)>(&MidiCtrlPlugin::cb_out_call_), this, _1, _2);
			PluginCb::out2_cb_ = std::bind(static_cast<void(MidiCtrlPlugin::*)(MIDI::MidiUnit&, DWORD)>(&MidiCtrlPlugin::cb_out_call_), this, _1, _2);
			PluginCb::cnf_cb_  = std::bind(static_cast<void(MidiCtrlPlugin::*)(std::shared_ptr<JSON::MMTConfig>&)>(&MidiCtrlPlugin::set_config_cb_), this, _1);
		}
		MidiCtrlPlugin::~MidiCtrlPlugin() {
			dispose_();
		}
		void MidiCtrlPlugin::dispose_() {
			TRACE_CALL();
			try {

				#pragma region MIDI-IN
				if (!dev_in_list_.empty()) {
					for (auto& a : dev_in_list_) {
						if ((a) && (a->IsConnected() || a->IsEnable())) a->Stop();
						a.reset();
					}
					dev_in_list_.clear();
				}
				#pragma endregion
				#pragma region MIDI-OUT
				if (!dev_out_list_.empty()) {
					for (auto& a : dev_out_list_) {
						if ((a) && (a->IsConnected() || a->IsEnable())) a->Stop();
						a.reset();
					}
					dev_out_list_.clear();
				}
				#pragma endregion
				#pragma region MIDI-PROXY
				if (dev_proxy_) {
					if (dev_proxy_->IsConnected() || dev_proxy_->IsEnable()) dev_proxy_->Stop();
					dev_proxy_.reset();
				}
				#pragma endregion

				export_list_.clear();

			} catch (...) {}
			is_config_ = is_enable_ = is_started_ = false;
		}
		void MidiCtrlPlugin::load_(MIDI::MidiConfig& mc) {
			is_enable_ = false;
			is_config_ = (!mc.empty() || (mc.out_count > 0) || (mc.proxy_count > 0U));
			if (is_config_ && !mc.midi_in_devices.empty()) is_enable_ = true;
			if (!is_enable_ && (mc.proxy_count > 0U)) is_enable_ = true;
			if (!is_enable_ && !mc.out_system_port && (mc.out_count > 0U)) is_enable_ = true;
			if (!is_enable_ && mc.out_system_port && !mc.midi_out_devices.empty()) is_enable_ = true;
		}

		#pragma region private call cb
		void MidiCtrlPlugin::set_config_cb_(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALL();
			try {
				if (!is_started_) {
					(void) load(mmt);
					#if defined (_DEBUG_CONFIG)
					::OutputDebugStringW(mmt->Dump().c_str());
					#endif
					return;
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}
		void MidiCtrlPlugin::cb_out_call_(MIDI::Mackie::MIDIDATA m, DWORD t) {
			try {
				if ((dev_proxy_) && dev_proxy_->IsConnected()) dev_proxy_->SendToPort(m, t);
			} catch (...) {}
		}
		void MidiCtrlPlugin::cb_out_call_(MIDI::MidiUnit& m, DWORD t) {
			try {
				if (dev_out_list_.empty()) return;
				for (auto& a : dev_out_list_)
					if ((a) && a->IsConnected()) a->SendToPort(m, t);

			} catch (...) {}
		}
		#pragma endregion

		#pragma region public actions
		bool MidiCtrlPlugin::load(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->empty()));
			try {
				load_(mmt->midiconf);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return true;
		}
		bool MidiCtrlPlugin::load(std::wstring s) {
			TRACE_CALLX(s);
			try {
				if (s.empty()) return true;

				worker_background::Get().to_async(std::async(std::launch::async, [=](std::wstring p) {
					try {
						MIDI::MidiConfig mc{};
						JSON::json_config jc{};

						is_enable_ = jc.ReadFile(p,
							[&](Tiny::TinyJson& root, std::wstring& path) -> bool {
								jc.ReadMidiConfig(root, mc);
								return true;
							}
						);
						if (is_enable_) load_(mc);
						if (is_enable_ && is_config_)
							common_config::Get().GetConfig()->midiconf.copy(mc);

					} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
				}, s));

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return true;
		}

		bool MidiCtrlPlugin::start(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->empty()));
			try {
				if (mmt->empty() || mmt->midiconf.empty()) return false;

				IO::PluginCb& pcb = GetPluginCb();
				std::shared_ptr<MIDI::MidiDriver>& drv = MIDI::MidiDevices::Get().GetMidiDriver();

				#pragma region MIDI-IN
				if (!mmt->midiconf.midi_in_devices.empty()) {
					for (auto& s : mmt->midiconf.midi_in_devices) {
						auto ptr = std::make_unique<MIDI::MidiControllerIn>(pcb, drv, std::wstring(s));

						if (!ptr->Start()) { ptr->Stop(); ptr.reset(); }
						else dev_in_list_.push_back(std::move(ptr));
					}
				}
				#pragma endregion
				#pragma region MIDI-OUT
				if (mmt->midiconf.out_system_port && !mmt->midiconf.midi_out_devices.empty()) {
					for (auto& s : mmt->midiconf.midi_out_devices) {
						auto ptr = std::make_unique<MIDI::MidiControllerOut>(pcb, drv, std::wstring(s));
						ptr->IsSystemPort(true);

						if (!ptr->Start()) { ptr->Stop(); ptr.reset(); }
						else dev_out_list_.push_back(std::move(ptr));
					}
				} else if (!mmt->midiconf.out_system_port && (mmt->midiconf.out_count > 0U)) {
					for (uint32_t i = 0U; i < mmt->midiconf.out_count; i++) {
						log_string ls{};
						if (mmt->midiconf.out_count == 1U) ls << MidiMackiePort;
						else ls << MidiMackiePort << L"-" << uint16_t(i + 1U);

						auto ptr = std::make_unique<MIDI::MidiControllerOut>(pcb, drv, ls.str());
						ptr->IsSystemPort(false);

						if (!ptr->Start()) { ptr->Stop(); ptr.reset(); }
						else dev_out_list_.push_back(std::move(ptr));
					}
				}
				#pragma endregion
				#pragma region MIDI-PROXY
				if (mmt->midiconf.proxy_count) {
					dev_proxy_ = std::make_unique<MIDI::MidiControllerProxy>(pcb, drv, MidiProxyPort.data());
					dev_proxy_->SetProxyCount(mmt->midiconf.proxy_count);

					if (!dev_proxy_->Start()) { dev_proxy_->Stop(); dev_proxy_.reset(); }
				}
				#pragma endregion

				is_started_ = (!dev_in_list_.empty() || !dev_out_list_.empty() || (dev_proxy_));
				return is_started_;

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
		bool MidiCtrlPlugin::stop() {
			TRACE_CALL();
			dispose_();
			return true;
		}
		void MidiCtrlPlugin::release() {
			TRACE_CALL();
			dispose_();
		}

		IO::PluginUi& MidiCtrlPlugin::GetPluginUi() {
			return std::ref(*plugin_ui_.GetPluginUi());
		}
		#pragma endregion

		IO::export_list_t& MidiCtrlPlugin::GetDeviceList() {
			TRACE_CALL();
			try {
				if (!export_list_.empty())
					return std::ref(export_list_);

				for (auto& a : dev_in_list_)
					export_list_.push_back(std::make_pair((uint16_t)1U, a->GetActiveDevice()));
				for (auto& a : dev_out_list_)
					export_list_.push_back(std::make_pair((uint16_t)2U, a->GetActiveDevice()));
				for (auto& s : MIDI::MidiDevices::Get().GetMidiProxyDeviceList())
					export_list_.push_back(std::make_pair((uint16_t)3U, std::wstring(s)));

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return std::ref(export_list_);
		}
		std::any MidiCtrlPlugin::GetDeviceData() {
			std::any a = (int)42;
			return a;
		}
	}
}