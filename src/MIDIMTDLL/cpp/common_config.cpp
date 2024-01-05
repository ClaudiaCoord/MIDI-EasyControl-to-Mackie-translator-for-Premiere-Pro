/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {

	using namespace std::string_view_literals;
	constexpr std::wstring_view DEFAULT_CNF_FILE = L"MidiController.cnf"sv;

	common_config common_config::ctrlcommonconfig_;

	static time_t getConfigFileModify__(std::wstring& s) {
		__try {
			struct _stat64 fileInfo;
			if (_wstat64(s.c_str(), &fileInfo) == 0)
				return fileInfo.st_mtime;
		} __except (EXCEPTION_EXECUTE_HANDLER) {}
		return 0;
	}
	time_t getConfigFileModify_(std::wstring& s) {
		_set_se_translator(seh_exception_catch);
		try {
			return getConfigFileModify__(s);
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
		return 0;
	}
	void getDefaultConfig_(std::wstring& cnfpath) {
		if (cnfpath.empty()) {
			cnfpath = Common::common_config::Get().Registry.GetConfPath();
			if (cnfpath.empty())
				cnfpath = Utils::runing_dir(DEFAULT_CNF_FILE);
		}
	}

	common_config::common_config() {
		config_ptr_ = std::make_shared<JSON::MMTConfig>();
	}
	common_config::~common_config() {
		config_ptr_.reset();
	}

	common_config& common_config::Get() {
		return std::ref(common_config::ctrlcommonconfig_);
	}
	std::shared_ptr<JSON::MMTConfig>& common_config::GetConfig() {
		return std::ref(config_ptr_);
	}

	const bool		common_config::IsStart() {
		return IsConfig() && Local.IsMidiDriverRun();
	}
	const bool		common_config::IsConfig() {
		return (config_ptr_ && !config_ptr_->empty());
	}
	const bool		common_config::IsNewConfig() {
		try {
			do {
				if (config_last_write_time == 0) break;
				std::wstring p = Registry.GetConfPath();
				if (p.empty()) break;
				time_t ft = getConfigFileModify_(p);
				if (ft == 0) break;
				if (config_last_write_time != ft) return true;
			} while (0);
			return !config_ptr_;
		} catch (...) {}
		return true;
	}

	const bool		common_config::IsConfigEmpty() {
		return (!config_ptr_) || config_ptr_->empty();
	}
	const bool		common_config::IsProxy() {
		return config_ptr_ ? (config_ptr_->midiconf.proxy_count > 0) : false;
	}
	const uint32_t	common_config::NumProxy() {
		return config_ptr_ ? config_ptr_->midiconf.proxy_count : 0U;
	}
	void			common_config::NumProxy(uint32_t cnt) {
		if (config_ptr_) update_value_(((cnt < 16) ? cnt : 15), config_ptr_->midiconf.proxy_count);
	}
	const bool		common_config::IsAutoStart() {
		return config_ptr_ ? config_ptr_->auto_start : false;
	}
	void			common_config::IsAutoStart(bool b) {
		if (config_ptr_) update_value_(b, config_ptr_->auto_start);
	}
	uint32_t		common_config::ButtonOnInterval() {
		return config_ptr_ ? config_ptr_->midiconf.btn_interval : 0;
	}
	void			common_config::ButtonOnInterval(uint32_t i) {
		if (config_ptr_) update_value_(i, config_ptr_->midiconf.btn_interval);
	}
	uint32_t		common_config::ButtonOnLongInterval() {
		return config_ptr_ ? config_ptr_->midiconf.btn_long_interval : 0;
	}
	void			common_config::ButtonOnLongInterval(uint32_t i) {
		if (config_ptr_) update_value_(i, config_ptr_->midiconf.btn_long_interval);
	}
	const bool		common_config::IsJogSceneFilter() {
		return config_ptr_ ? config_ptr_->midiconf.jog_scene_filter : false;
	}
	void			common_config::IsJogSceneFilter(bool b) {
		if (config_ptr_) update_value_(b, config_ptr_->midiconf.jog_scene_filter);
	}
	std::wstring	common_config::GetConfigPath() {
		std::wstring cnfpath{};
		getDefaultConfig_(cnfpath);
		return cnfpath;
	}

	const bool		common_config::Load(std::wstring cnfpath) {
		try {

			getDefaultConfig_(cnfpath);
			
			bool b = false;
			auto a = new JSON::MMTConfig();
			auto f = std::async(std::launch::async, [=](JSON::MMTConfig* md_, std::wstring& cnf_) -> bool {
				JSON::json_config js;
				return js.Read(md_, cnf_, true);
				}, a, std::ref(cnfpath));

			try {
				b = f.get();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			if (b) {
				config_ptr_.reset(std::move(a));
				update_conf_();
				config_last_write_time = getConfigFileModify_(cnfpath);
			} else delete a;
			return b;
		}
		catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
		return false;
	}
	const bool		common_config::Save(std::wstring cnfpath) {
		try {

			if (!IsConfig()) return false;
			getDefaultConfig_(cnfpath);

			bool b = false;
			auto f = std::async(std::launch::async, [=]() -> bool {
				JSON::json_config js;
				return js.Write(config_ptr_, cnfpath, true);
				});

			try {
				b = f.get();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			if (b) {
				update_conf_();
				config_last_write_time = getConfigFileModify_(cnfpath);
			}
			return b;
		}
		catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
		return false;
	}

	uint32_t		common_config::add(callConfigCb_t cb) {
		try {
			uint32_t id = event_.add(cb);
			if (config_ptr_ && (!config_ptr_->empty())) {
				worker_background::Get().to_async(
					std::async(std::launch::async, [=](callConfigCb_t c, std::shared_ptr<JSON::MMTConfig>& md) {
						try {
							c(md);
						} catch (...) {
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						}
					}, cb, std::ref(config_ptr_))
				);
			}
			return id;
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
		return 0U;
	}
	void			common_config::add(callConfigCb_t cb, uint32_t id) {
		try {
			event_.add(cb, id);
			if (config_ptr_ && (!config_ptr_->empty())) {
				worker_background::Get().to_async(
					std::async(std::launch::async, [=](callConfigCb_t c, std::shared_ptr<JSON::MMTConfig>& md) {
					try {
						c(md);
					} catch (...) {
						Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					}
				}, cb, std::ref(config_ptr_))
				);
			}
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
	}
	void			common_config::remove(callConfigCb_t cb) {
		event_.remove(cb);
	}
	void			common_config::remove(uint32_t id) {
		event_.remove(id);
	}

	void			common_config::update_conf_() {
		event_.send_async(std::ref(config_ptr_));
	}

	/* CONFIG LOCAL */

	const bool		common_config_local::IsMidiDriverRun() {
		return midibridgerun_;
	}
	void			common_config_local::IsMidiDriverRun(bool b) {
		midibridgerun_ = b;
	}

	const bool		common_config_local::IsAudioMixerRun() {
		return audiomixerrun_;
	}
	void			common_config_local::IsAudioMixerRun(bool b) {
		audiomixerrun_ = b;
	}
}
