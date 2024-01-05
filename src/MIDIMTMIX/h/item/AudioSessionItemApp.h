/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIXER {

		typedef std::function<void(std::wstring&, std::wstring&, std::wstring&, std::wstring&, GUID&, uint32_t)> valuesOnAppChange;

		class FLAG_EXPORT AudioSessionItemTitle {
		private:
			const std::wstring_view name;
			std::size_t id = 0;
		public:

			AudioSessionItemTitle(std::wstring_view);
			std::wstring GetName();
			std::size_t  GetId();
		};

		class FLAG_EXPORT AudioSessionItemApp
		{
		private:
			std::wstring name;
			std::size_t  nameid = 0;
			valuesOnAppChange OnChangeCb = [](std::wstring&, std::wstring&, std::wstring&, std::wstring&, GUID&, uint32_t) {};

			void copy_(AudioSessionItemApp&, bool);

		public:
			std::wstring Path;
			std::wstring Desc;
			std::wstring Icon;

			GUID Guid = GUID_NULL;
			uint32_t Pid = 0U;

			operator bool();
			const bool operator==(const std::size_t&);
			log_string& operator<<(log_string& s);
			operator std::wstring() const;
			operator std::size_t() const;

			AudioSessionItemApp() = default;

			void set_onchangecb(valuesOnAppChange);
			void set_onchangecb_defsault();

			void set(std::size_t, std::wstring);
			void set(std::wstring);
			void set(std::wstring, std::size_t);
			void set(std::wstring, GUID, uint32_t, std::wstring, std::wstring = L"", std::wstring = L"");

			void copy(AudioSessionItem&);
			void copy(AudioSessionItemApp&);
			void copy(AudioSessionItemChange&);

			void copy_id(AudioSessionItemApp&);

			const bool equals(const std::size_t&);
			const bool empty();
			const bool update_empty();
			const bool guid_empty();
			log_string to_string();

			template<typename T1>
			FLAG_EXPORT T1 get();

			static std::size_t hash(std::wstring);
		};
		extern template FLAG_EXPORT std::size_t AudioSessionItemApp::get<std::size_t>();
		extern template FLAG_EXPORT std::wstring AudioSessionItemApp::get<std::wstring>();
	}
}

