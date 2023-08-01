/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace MIXER {

		AudioSessionItemTitle::AudioSessionItemTitle(std::wstring_view s) : name(s) {
			id = AudioSessionItemApp::hash(std::wstring(s.data()));
		}
		std::wstring AudioSessionItemTitle::GetName() {
			return name.data();
		}
		std::size_t  AudioSessionItemTitle::GetId() {
			return id;
		}

		static inline const bool empty_(std::wstring& s, std::wstring& n, std::size_t& h) {
			return (s.empty() || (!n.empty() && (h > 0)));
		}
		void AudioSessionItemApp::copy_(AudioSessionItemApp& a, bool isclear) {
			try {
				if (!a.name.empty() && (isclear || !name._Equal(a.name))) {
					name = std::wstring(a.name.begin(), a.name.end());
					nameid = (nameid == 0) ? ((a.nameid > 0) ? a.nameid :AudioSessionItemApp::hash(name)) : nameid;
				}
				if ((a.Pid != 0) && (isclear || (Pid != a.Pid)))
					Pid = a.Pid;
				if ((a.Guid != GUID_NULL) && (isclear || (Guid != a.Guid)))
					Guid = a.Guid;
				if (!a.Path.empty() && (isclear || !Path._Equal(a.Path)))
					Path = std::wstring(a.Path.begin(), a.Path.end());
				if (!a.Desc.empty() && (isclear || !Desc._Equal(a.Desc)))
					Desc = std::wstring(a.Desc.begin(), a.Desc.end());
				if (!a.Icon.empty() && (isclear || !Icon._Equal(a.Icon)))
					Icon = std::wstring(a.Icon.begin(), a.Icon.end());

				OnChangeCb(name, Path, Desc, Icon, Guid, Pid);

			} catch (...) {}
		}

		AudioSessionItemApp::operator bool() {
			return empty();
		}
		AudioSessionItemApp::operator std::wstring() const {
			return name;
		}
		AudioSessionItemApp::operator std::size_t() const {
			return nameid;
		}
		log_string& AudioSessionItemApp::operator<<(log_string& s) {
			s << name << L" (" << nameid << ")";
			return s;
		}
		const bool AudioSessionItemApp::operator==(const std::size_t& h) {
			return equals(h);
		}
	
		void AudioSessionItemApp::set_onchangecb(valuesOnAppChange cb) {
			if (cb == nullptr) return;
			OnChangeCb = cb;
			OnChangeCb(name, Path, Desc, Icon, Guid, Pid);
		}
		void AudioSessionItemApp::set_onchangecb_defsault() {
			OnChangeCb = [](std::wstring&, std::wstring&, std::wstring&, std::wstring&, GUID&, uint32_t) {};
		}

		void AudioSessionItemApp::set(std::wstring s) {
			if (empty_(s, name, nameid)) return;
			set(s, AudioSessionItemApp::hash(s));
			OnChangeCb(name, Path, Desc, Icon, Guid, Pid);
		}
		void AudioSessionItemApp::set(std::wstring s, std::size_t h) {
			if (empty_(s, name, nameid)) return;
			if (h == 0) {
				set(s, AudioSessionItemApp::hash(s));
			} else {
				name = s;
				nameid = h;
				OnChangeCb(name, Path, Desc, Icon, Guid, Pid);
			}
		}
		void AudioSessionItemApp::set(std::size_t count, std::wstring s) {
			if (s.empty()) return;
			if (count == 0) set(s);
			else {
				name = (log_string() << s << L" " << (uint32_t)count).str();
				nameid = (nameid == 0) ? AudioSessionItemApp::hash(s) : nameid;
				OnChangeCb(name, Path, Desc, Icon, Guid, Pid);
			}
		}
		void AudioSessionItemApp::set(std::wstring n, GUID g, uint32_t pid, std::wstring p, std::wstring d, std::wstring i) {
			set(n);
			Pid = pid;
			Guid = g;
			Path = p;
			Desc = d;
			Icon = i;
			OnChangeCb(name, Path, Desc, Icon, Guid, Pid);
		}

		void AudioSessionItemApp::copy(AudioSessionItem& a) {
			copy_(a.Item.App, true);
		}
		void AudioSessionItemApp::copy(AudioSessionItemApp& a) {
			copy_(a, true);
		}
		void AudioSessionItemApp::copy(AudioSessionItemChange& a) {
			copy_(a.Item.App, false);
		}
		void AudioSessionItemApp::copy_id(AudioSessionItemApp& a) {
			nameid = a.get<std::size_t>();
		}

		std::size_t AudioSessionItemApp::hash(std::wstring s) {
			return std::hash<std::wstring>{}(s);
		}
		const bool AudioSessionItemApp::equals(const std::size_t& h) {
			return nameid == h;
		}
		const bool AudioSessionItemApp::empty() {
			return (nameid == 0) || name.empty();
		}
		const bool AudioSessionItemApp::update_empty() {
			return nameid == 0;
		}
		const bool AudioSessionItemApp::guid_empty() {
			return Guid == GUID_NULL;
		}

		log_string AudioSessionItemApp::to_string() {
			log_string s;
			s << L"(" << Pid << L") " << Utils::to_string(Guid) << L" - " << name << L" (" << nameid << L")";
			if (!Desc.empty())
				s << L", [" << Desc << L"]";
			if (!Icon.empty())
				s << L", [" << Icon << L"]";
			if (!Path.empty())
				s << L", [" << Path << L"]";
			return s;
		}

		template <typename T1>
		T1 AudioSessionItemApp::get() {
			if constexpr (std::is_same_v<std::wstring, T1>) return name;
			else if constexpr (std::is_same_v<std::size_t, T1>) return nameid;
		}
		template std::size_t AudioSessionItemApp::get<std::size_t>();
		template std::wstring AudioSessionItemApp::get<std::wstring>();
	}
}
