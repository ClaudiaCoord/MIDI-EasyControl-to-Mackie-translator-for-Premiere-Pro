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

		ClassStorage ClassStorage::dlg_storage_{};
		ClassStorage& ClassStorage::Get() {
			return std::ref(dlg_storage_);
		}

		const bool ClassStorage::IsRunOther() const {
			return ((dlgs_) && (!dlgs_->IsRunOnce())) || 
					((dlgc_) && (!dlgc_->IsRunOnce())) ||
					((dlgm_) && (!dlgm_->IsRunOnce()));
		}

		const bool ClassStorage::StartAsync() {
			try {
				auto f = std::async(std::launch::async, [=]() -> bool {
					return start_();
				});
				
				const bool b = f.get();
				start_audiosessionmixer_(b, false);

				return b;

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
		const bool ClassStorage::StopAsync() {
			try {

				stop_audiomixerpanels_();
				stop_audiosessionmixer_();

				auto f = std::async(std::launch::async, [=]() -> bool {
					return stop_();
				});
				return f.get();

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}

		const bool ClassStorage::Start(bool isthread) {
			try {
				const bool b = start_();
				start_audiosessionmixer_(b, isthread);
				return b;

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
		const bool ClassStorage::Stop() {
			try {

				stop_audiomixerpanels_();
				stop_audiosessionmixer_();
				return stop_();

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}

		const bool ClassStorage::start_() {
			try {
				common_config& cnf = common_config::Get();
				if (!cnf.Load() || cnf.IsConfigEmpty()) return false;

				IO::IOBridge& br = IO::IOBridge::Get();
				if (!br.IsLoaded()) {
					br.Reload();
					if (!br.IsLoaded()) return false;
				}
				return br.Start();

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
		const bool ClassStorage::stop_() {
			try {
				IO::IOBridge& br = IO::IOBridge::Get();
				if (!br.IsStarted()) return false;
				return br.Stop();

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}

		void ClassStorage::stop_audiomixerpanels_() {
			if (mctrl_)
				mctrl_->Close();
		}
		void ClassStorage::stop_audiosessionmixer_() {
			if (mix_)
				mix_->Stop();
		}
		void ClassStorage::start_audiosessionmixer_(bool b, bool isthread) {
			if (b && common_config::Get().Registry.GetMixerEnable())
				if (isthread)
					::PostMessageW(LangInterface::Get().GetMainHwnd(), WM_COMMAND, MAKEWPARAM(IDM_GO_MIXER_INIT, 0), (LPARAM)0);
				else
					GetDialog<MIXER::AudioSessionMixer>()->Start();
		}
	}
}