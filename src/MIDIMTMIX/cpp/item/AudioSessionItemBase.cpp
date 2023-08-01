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

		const bool AudioSessionItemBase::IsEmptyId() {
			return Id.empty();
		}
		const bool AudioSessionItemBase::IsEmptyApp() {
			if (App.empty()) return true;
			switch (Typeitem) {
				case TypeItems::TypeSession: return (App.Pid == 0U) || (App.Guid == GUID_NULL);
				case TypeItems::TypeDevice:
				case TypeItems::TypeMaster:  return (App.Guid == GUID_NULL);
				case TypeItems::TypeNone:
				default: break;
			}
			return true;
		}
		const bool AudioSessionItemBase::IsEqualsApp(const std::size_t z) {
			return App.equals(z);
		}
		const bool AudioSessionItemBase::IsEqualsGuid(const GUID& g) {
			return (g != GUID_NULL) && (App.Guid == g);
		}
		const MIDI::MidiUnitType AudioSessionItemBase::FoundId(const uint32_t k, const MIDI::MidiUnitType t) {
			if (t == MIDI::MidiUnitType::UNITNONE)
				return Id.found(k);
			return Id.found(k, t);
		}

		const GUID AudioSessionItemBase::GetGuid() const {
			return App.Guid;
		}
		const uint32_t AudioSessionItemBase::GetPid() const {
			return App.Pid;
		}
		const TypeItems AudioSessionItemBase::GetType() const {
			return Typeitem;
		}
		const std::wstring AudioSessionItemBase::GetName() const {
			return App;
		}
		const std::wstring AudioSessionItemBase::GetDesc() const {
			return App.Desc;
		}
		const std::wstring AudioSessionItemBase::GetIcon() const {
			return App.Icon;
		}
		const std::wstring AudioSessionItemBase::GetPath() const {
			return App.Path;
		}

		void AudioSessionItemBase::SetType(TypeItems t) {
			Typeitem = t;
		}
	}
}