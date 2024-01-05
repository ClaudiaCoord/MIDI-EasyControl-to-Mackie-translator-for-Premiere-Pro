/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIDIMT {

		class ClassStorage {
		private:
			std::unique_ptr<DialogStart> dlgs_{};
			std::unique_ptr<DialogEdit> dlgc_{};
			std::unique_ptr<DialogMonitor> dlgm_{};
			std::unique_ptr<DialogLogView> dlgl_{};
			std::unique_ptr<DialogAbout> dlga_{};
			std::unique_ptr<DialogThemeColors> dlgt_{};
			std::unique_ptr<DialogEditInfo> dlgi_{};
			std::unique_ptr<TrayMenu> menu_{};
			std::unique_ptr<TrayNotify> notify_{};
			std::unique_ptr<AudioMixerPanels> mctrl_{};
			std::unique_ptr<MIXER::AudioSessionMixer> mix_{};

			static ClassStorage dlg_storage_;
			const bool start_();
			const bool stop_();
			void stop_audiomixerpanels_();
			void stop_audiosessionmixer_();
			void start_audiosessionmixer_(bool, bool);

		public:

			ClassStorage() = default;
			~ClassStorage() = default;

			static ClassStorage& Get();

			const bool IsRunOther() const;

			const bool Start(bool = false);
			const bool StartAsync();

			const bool Stop();
			const bool StopAsync();

			template <class T1>
			std::unique_ptr<T1>& GetDialog() {
				try {
					if constexpr (std::is_same_v<DialogStart, T1>) {
						if (!dlgs_) dlgs_.reset(new T1());
						return std::ref(dlgs_);
					}
					else if constexpr (std::is_same_v<DialogEdit, T1>) {
						if (!dlgc_) dlgc_.reset(new T1());
						return std::ref(dlgc_);
					}
					else if constexpr (std::is_same_v<DialogMonitor, T1>) {
						if (!dlgm_) dlgm_.reset(new T1());
						return std::ref(dlgm_);
					}
					else if constexpr (std::is_same_v<DialogLogView, T1>) {
						if (!dlgl_) dlgl_.reset(new T1());
						return std::ref(dlgl_);
					}
					else if constexpr (std::is_same_v<DialogAbout, T1>) {
						if (!dlga_) dlga_.reset(new T1());
						return std::ref(dlga_);
					}
					else if constexpr (std::is_same_v<DialogThemeColors, T1>) {
						if (!dlgt_) dlgt_.reset(new T1());
						return std::ref(dlgt_);
					}
					else if constexpr (std::is_same_v<DialogEditInfo, T1>) {
						if (!dlgi_) dlgi_.reset(new T1());
						return std::ref(dlgi_);
					}
					else if constexpr (std::is_same_v<AudioMixerPanels, T1>) {
						if (!mctrl_) mctrl_.reset(new T1());
						return std::ref(mctrl_);
					}
					else if constexpr (std::is_same_v<TrayMenu, T1>) {
						if (!menu_) menu_.reset(new T1());
						return std::ref(menu_);
					}
					else if constexpr (std::is_same_v<TrayNotify, T1>) {
						if (!notify_) notify_.reset(new T1());
						return std::ref(notify_);
					}
					else if constexpr (std::is_same_v<MIXER::AudioSessionMixer, T1>) {
						if (!mix_) mix_.reset(new T1());
						return std::ref(mix_);
					}
					else throw make_common_error(L"GetDialog class selector error");

				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
					throw;
				}
			}

			template <class T1>
			const bool IsDialog() {
				try {
					if constexpr (std::is_same_v<DialogStart, T1>)
						return (dlgs_.get());
					else if constexpr (std::is_same_v<DialogEdit, T1>)
						return (dlgc_.get());
					else if constexpr (std::is_same_v<DialogMonitor, T1>)
						return (dlgm_.get());
					else if constexpr (std::is_same_v<DialogLogView, T1>)
						return (dlgl_.get());
					else if constexpr (std::is_same_v<DialogAbout, T1>)
						return (dlga_.get());
					else if constexpr (std::is_same_v<DialogThemeColors, T1>)
						return (dlgt_.get());
					else if constexpr (std::is_same_v<DialogEditInfo, T1>)
						return (dlgi_.get());
					else if constexpr (std::is_same_v<AudioMixerPanels, T1>)
						return (mctrl_.get());
					else if constexpr (std::is_same_v<TrayMenu, T1>)
						return (menu_.get());
					else if constexpr (std::is_same_v<TrayNotify, T1>)
						return (notify_.get());
					else if constexpr (std::is_same_v<MIXER::AudioSessionMixer, T1>)
						return (mix_.get());
					else throw make_common_error(L"IsDialog class selector error");

				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				return false;
			}

			template <class T1>
			HWND OpenDialog(HWND h, bool isonce) {
				try {
					std::unique_ptr<T1>& ptr = GetDialog<T1>();
					if (!ptr)
						throw make_common_error(L"OpenDialog class selector error");

					if (!ptr->IsRunOnce()) {
						ptr->SetFocus();
						return nullptr;
					}
					else if (isonce && IsRunOther()) return nullptr;
					else return ptr->BuildDialog(h);

				} catch (...) {
					Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				}
				return nullptr;
			}
		};
	}
}