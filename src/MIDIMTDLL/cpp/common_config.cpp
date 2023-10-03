/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {

	using namespace std::string_view_literals;
	constexpr std::wstring_view DEFAULT_CNF_FILE = L"MidiController.cnf"sv;

	common_config common_config::ctrlcommonconfig__;

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
		config_ptr__ = std::make_shared<Common::MIDI::MidiDevice>();
	}
	common_config::~common_config() {
		config_ptr__.reset();
	}

	common_config& common_config::Get() {
		return std::ref(common_config::ctrlcommonconfig__);
	}
	std::shared_ptr<Common::MIDI::MidiDevice>& common_config::GetConfig() {
		return std::ref(config_ptr__);
	}


	const bool		common_config::IsStart() {
		return IsConfig() && Local.IsMidiBridgeRun();
	}
	const bool		common_config::IsConfig() {
		return (config_ptr__ && (!config_ptr__->IsEmpty()));
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
			return !config_ptr__;
		} catch (...) {}
		return true;
	}

	const bool		common_config::IsConfigEmpty() {
		return (!config_ptr__) || config_ptr__->IsEmpty();
	}
	const bool		common_config::IsProxy() {
		return config_ptr__ ? (config_ptr__->proxy > 0) : false;
	}
	void			common_config::NumProxy(uint32_t cnt) {
		if (config_ptr__) config_ptr__->proxy = (cnt < 16) ? cnt : 15;
	}
	const uint32_t	common_config::NumProxy() {
		return config_ptr__ ? config_ptr__->proxy : 0;
	}
	const bool		common_config::IsAutoStart() {
		return config_ptr__ ? config_ptr__->autostart : false;
	}
	void			common_config::IsAutoStart(bool b) {
		if (config_ptr__) config_ptr__->autostart = b;
	}
	uint32_t		common_config::ButtonOnInterval() {
		return config_ptr__ ? config_ptr__->btninterval : 0;
	}
	void			common_config::ButtonOnInterval(uint32_t i) {
		if (config_ptr__) config_ptr__->btninterval = i;
	}
	uint32_t		common_config::ButtonOnLongInterval() {
		return config_ptr__ ? config_ptr__->btnlonginterval : 0;
	}
	void			common_config::ButtonOnLongInterval(uint32_t i) {
		if (config_ptr__) config_ptr__->btnlonginterval = i;
	}
	const bool		common_config::IsJogSceneFilter() {
		return config_ptr__ ? config_ptr__->jogscenefilter : false;
	}
	void			common_config::IsJogSceneFilter(bool b) {
		if (config_ptr__) config_ptr__->jogscenefilter = b;
	}

	const bool		common_config::Load(std::wstring cnfpath) {
		try {

			getDefaultConfig_(cnfpath);
			
			bool b = false;
			auto a = new Common::MIDI::MidiDevice();
			auto f = std::async(std::launch::async, [=](Common::MIDI::MidiDevice* md_, std::wstring& cnf_) -> bool {
				Common::MIDI::json_config js;
				return js.Read(md_, cnf_);
				}, a, std::ref(cnfpath));

			try {
				b = f.get();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			if (b) {
				config_ptr__.reset(std::move(a));
				updateconf_();
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
				Common::MIDI::json_config js;
				return js.Write(config_ptr__, cnfpath);
				});

			try {
				b = f.get();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			if (b) {
				updateconf_();
				config_last_write_time = getConfigFileModify_(cnfpath);
			}
			return b;
		}
		catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
		return false;
	}

	uint32_t		common_config::add(callConfigChangeCb cb) {
		try {
			uint32_t id = event__.add(cb);
			if (config_ptr__ && (!config_ptr__->IsEmpty())) {
				Common::worker_background::Get().to_async(
					std::async(std::launch::async, [=](callConfigChangeCb c, std::shared_ptr<Common::MIDI::MidiDevice>& md) {
						try {
							c(md);
						} catch (...) {
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						}
					}, cb, std::ref(config_ptr__))
				);
			}
			return id;
		} catch (...) {
			Utils::get_exception(std::current_exception(), __FUNCTIONW__);
		}
		return 0U;
	}
	void			common_config::remove(callConfigChangeCb cb) {
		event__.remove(cb);
	}
	void			common_config::remove(uint32_t id) {
		event__.remove(id);
	}

	void			common_config::updateconf_() {
		event__.send_async(std::ref(config_ptr__));
	}

	/* CONFIG LOCAL */

	const bool		common_config_local::IsMidiBridgeRun() {
		return midibridgerun_;
	}
	void			common_config_local::IsMidiBridgeRun(bool b) {
		midibridgerun_ = b;
	}

	const bool		common_config_local::IsAudioMixerRun() {
		return audiomixerrun_;
	}
	void			common_config_local::IsAudioMixerRun(bool b) {
		audiomixerrun_ = b;
	}

	const bool		common_config_local::IsMMKeysRun() {
		return mmkeysrun_;
	}
	void			common_config_local::IsMMKeysRun(bool b) {
		mmkeysrun_ = b;
	}

	const bool		common_config_local::IsSmartHomeRun() {
		return smarthomerun_;
	}
	void			common_config_local::IsSmartHomeRun(bool b) {
		smarthomerun_ = b;
	}

	const bool		common_config_local::IsLightsRun() {
		return lightsrun_;
	}
	void			common_config_local::IsLightsRun(bool b) {
		lightsrun_ = b;
	}
}
