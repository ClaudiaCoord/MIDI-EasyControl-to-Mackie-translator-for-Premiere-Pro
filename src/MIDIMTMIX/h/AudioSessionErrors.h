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

		static const wchar_t* _ERRSESMSGS[] = {
			L"OK",
			L"Not implemented yet",
			L"Undefined sessionid",
			L"AudioSessionEnumerator init failed",
			L"SessionControl init failed",
			L"SessionControl 2 init failed",
			L"SimpleAudio Volume init failed",
			L"SimpleAudio Mute get failed",
			L"Get info about process failed",

			L"Co*Initialize Extended failed",
			L"Co*Create Instance failed",
			L"Get Default Audio Endpoint failed",
			L"Audio Session Manager 2 failed",
			L"Audio Endpoint Volume failed",
			L"Get Session Enumerator failed",
			L"Get Session list failed",
			L"Initialize before call this",

			L"<empty>",
			L"This Error not defined"
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
				ERRMSG_INIT_VOLUME_AE,
				ERRMSG_INIT_SESSION_ENUM,
				ERRMSG_SESSION_LIST,
				ERRMSG_NO_INIT,
				EMPTY,
				NONE
			};

			static std::wstring GetMsg(AudioSessionErrors::Id err) {
				if (((int)err >= (int)AudioSessionErrors::Id::NONE) || ((int)err < 0))
					return (wchar_t*)(_ERRSESMSGS[(int)AudioSessionErrors::Id::NONE]);
				return std::wstring((wchar_t*)(_ERRSESMSGS[(int)err]));
			}
		};
	}
}
