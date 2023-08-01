/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	MIDIMTMIX DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace MIXER {

		using namespace std::string_view_literals;

		constexpr std::wstring_view _ERRSESMSGS[] = {
			L"OK"sv,
			L"Not implemented yet"sv,
			L"Undefined sessionid"sv,
			L"AudioSessionEnumerator init failed"sv,
			L"SessionControl init failed"sv,
			L"SessionControl 2 init failed"sv,
			L"SimpleAudio Volume init failed"sv,
			L"SimpleAudio Mute get failed"sv,
			L"Get info about process failed"sv,

			L"Co*Initialize Extended failed"sv,
			L"Co*Create Instance failed"sv,
			L"Get Default Audio Endpoint failed"sv,
			L"Audio Session Manager 2 failed"sv,
			L"Audio Meter Peak failed"sv,
			L"Get Session Enumerator failed"sv,
			L"Get Session list failed"sv,
			L"Initialize before call this"sv,
			L"Audio session list is null!"sv,
			L"Audio session disconnect, reason="sv,
			L"Audio session created, name="sv,
			L": Mixer audio sessions started, at the time of start, there are "sv,
			L" running programs available for regulation."sv,
			L": The audio sessions Mixer is gracefully stopped."sv,
			L": The audio sessions mixer could not be stopped correctly, the details of the error should be in the log file."sv,
			L": The audio sessions mixer could not be started, error details should be in the log file."sv,
			L": Audio session item \""sv,
			L"\" - bad \"SC\" registred state: "sv,
			L"\" - bad \"AE\" registred state: "sv,

			L"<empty>"sv,
			L"This Error not defined"sv
		};

		class AudioSessionErrors
		{
		public:
			enum Id : int {
				ERRMSG_OK = 0,
				ERRMSG_NOTIMPLEMENTEDYET,
				ERRMSG_UNDEFINEDSESSIONID,
				ERRMSG_AUIDOSESSIONENUMERATOR_INIT_FAILED,
				ERRMSG_SESSIONCONTROL_INIT_FAILED,
				ERRMSG_SESSIONCONTROL2_INIT_FAILED,
				ERRMSG_AUDIOVOLUME_INIT_FAILED,
				ERRMSG_AUDIOMUTE_GET_FAILED,
				ERRMSG_GETPROCESSINFO_FAILE,
				ERRMSG_INIT_COINITIALIZE,
				ERRMSG_INIT_COCREATE,
				ERRMSG_INIT_DEFAULT_AE,
				ERRMSG_INIT_AS_MANAGER2,
				ERRMSG_INIT_AS_AUDIOMETER,
				ERRMSG_INIT_SESSION_ENUM,
				ERRMSG_SESSION_LIST,
				ERRMSG_NO_INIT,
				ERRMSG_LISTISNULL,
				ERRMSG_SESSIONDISCONNECTED,
				ERRMSG_SESSIONCREATED,
				ERRMSG_MIXER_START1OK,
				ERRMSG_MIXER_START2OK,
				ERRMSG_MIXER_STOPOK,
				ERRMSG_MIXER_STRSTOPERROR,
				ERRMSG_MIXER_STARTERROR,
				ERRMSG_ITEM_BEGIN,
				ERRMSG_ITEM_SC,
				ERRMSG_ITEM_AE,

				EMPTY,
				NONE
			};

			static std::wstring GetMsg(AudioSessionErrors::Id err) {
				if (((int)err >= (int)AudioSessionErrors::Id::NONE) || ((int)err < 0))
					return _ERRSESMSGS[(int)AudioSessionErrors::Id::NONE].data();
				return _ERRSESMSGS[(int)err].data();
			}
		};
	}
}
