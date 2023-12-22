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

		#if defined(_DEBUG)
		/* #define _DEBUG_AUDIOSESSIONITEMCHANGE 1 */
		#endif

		AudioSessionItemChange::AudioSessionItemChange(AudioSessionItem* item, OnChangeType t, bool isevent) : isevent_(isevent) {
			if (!item) return;

			switch (selector_type(t)) {
				case OnChangeType::OnChangeUpdateData: {
					copychangedata_(*item);
					isinit_ = (!Item.App.empty() && !Item.App.guid_empty() && !Item.Id.empty() && (Item.GetType() != TypeItems::TypeNone));
					break;
				}
				case OnChangeType::OnChangeUpdateAllValues: {
					copychangevalue_(*item);
					isinit_ = (!Item.App.update_empty() && (Item.GetType() != TypeItems::TypeNone));
					break;
				}
				case OnChangeType::OnChangeRemove: {
					copychangeremove_(*item);
					isinit_ = (!Item.App.update_empty() && !Item.App.guid_empty());
					break;
				}
				default: return;
			}

			#if defined(_DEBUG_AUDIOSESSIONITEMCHANGE)
			log_string ls{};
			ls  << L"* AudioSessionItemChange + { name=" << item->Item.App.get<std::wstring>() << L", hash=" << item->Item.App.get<std::size_t>()
				<< L",\n\tonchage=" << AudioSessionHelper::OnChangeTypeHelper(t) << L"/" << AudioSessionHelper::OnChangeTypeHelper(selector_type(t))
				<< L",\n\tinit=" << std::boolalpha << isinit_ << L"/" << AudioSessionItemChange::filter_cb(t) << L" }\n";
			::OutputDebugStringW(ls.str().c_str());
			#endif

			if (isinit_) {
				actiontype_ = AudioSessionItemChange::filter_type(t);
				iscallaudiocb_ = AudioSessionItemChange::filter_cb(t);
			} else {
				actiontype_ = OnChangeType::OnChangeNone;
				iscallaudiocb_ = false;
			}
		}
		AudioSessionItemChange::~AudioSessionItemChange() {
			Item.Volume.set_onchangecb_defsault();
		}

		template<typename T>
		void AudioSessionItemChange::copychangedata_(T& item) {
			iscallaudiocb_ = false;
			Item.SetType(item.Item.GetType());
			if (!item.Item.Id.empty()) Item.Id.copy(item.Item.Id);
			if (!item.Item.App.empty()) Item.App.copy(item.Item.App);
			if constexpr (std::is_same_v<T, AudioSessionItemChange>)
				Item.Volume.copy(item.Item.Volume, item.GetAction());
			else
				Item.Volume.copy(item.Item.Volume, OnChangeType::OnChangeUpdateAllValues);
		}
		template void AudioSessionItemChange::copychangedata_(AudioSessionItem& item);
		template void AudioSessionItemChange::copychangedata_(AudioSessionItemChange& item);

		template<typename T>
		void AudioSessionItemChange::copychangevalue_(T& item) {
			iscallaudiocb_ = false;
			Item.SetType(item.Item.GetType());
			if (!item.Item.App.update_empty()) Item.App.copy(item.Item.App);
			if constexpr (std::is_same_v<T, AudioSessionItemChange>)
				Item.Volume.copy(item.Item.Volume, item.GetAction());
			else
				Item.Volume.copy(item.Item.Volume, OnChangeType::OnChangeUpdateAllValues);
		}
		template void AudioSessionItemChange::copychangevalue_(AudioSessionItem& item);
		template void AudioSessionItemChange::copychangevalue_(AudioSessionItemChange& item);

		template<typename T>
		void AudioSessionItemChange::copychangeremove_(T& item) {
			iscallaudiocb_ = false;
			Item.SetType(item.Item.GetType());
			Item.App.copy_id(item.Item.App);
			Item.App.Guid = item.Item.App.Guid;
		}
		template void AudioSessionItemChange::copychangeremove_(AudioSessionItem& item);
		template void AudioSessionItemChange::copychangeremove_(AudioSessionItemChange& item);

		const bool AudioSessionItemChange::IsEvent() {
			return isevent_;
		}
		const bool AudioSessionItemChange::IsCallAudioCb() {
			return iscallaudiocb_;
		}
		const bool AudioSessionItemChange::IsValid() {
			return (isinit_ && !Item.App.empty() && !Item.Id.empty() && (Item.GetType() != TypeItems::TypeNone));
		}
		const bool AudioSessionItemChange::IsUpdateValid() {
			try {
				switch (selector_type(actiontype_)) {
					case OnChangeType::OnChangeUpdateData: return IsValid();
					case OnChangeType::OnChangeUpdateAllValues: return (isinit_ && !Item.App.update_empty() && (Item.GetType() != TypeItems::TypeNone));
					case OnChangeType::OnChangeRemove:return (isinit_ && !Item.App.update_empty());
					default: return false;
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		const OnChangeType AudioSessionItemChange::GetAction() {
			return actiontype_;
		}

		void AudioSessionItemChange::Copy(AudioSessionItemChange& item, bool isclear) {
			try {
				actiontype_ = item.GetAction();
				iscallaudiocb_ = false;
				if (isclear) {
					Item.App = AudioSessionItemApp();
					Item.Id = AudioSessionItemId();
					Item.Volume = AudioSessionItemValue();
				}
				copychangedata_(item);
				isinit_ = (!Item.App.empty() && !Item.App.guid_empty() && (Item.GetType() != TypeItems::TypeNone));

			} catch(...) {}
		}
		void AudioSessionItemChange::Update(AudioSessionItemChange& item) {
			try {
				switch (selector_type(item.GetAction())) {
					case OnChangeType::OnChangeUpdateData: {
						copychangedata_(item);
						break;
					}
					case OnChangeType::OnChangeUpdateAllValues: {
						copychangevalue_(item);
						break;
					}
					case OnChangeType::OnChangeRemove: {
						copychangeremove_(item);
						break;
					}
					default: return;
				}
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		const GUID AudioSessionItemChange::GetGUID() {
			return Item.App.Guid;
		}
		const std::size_t AudioSessionItemChange::GetAppId() {
			return Item.App.get<std::size_t>();
		}
		std::wstring AudioSessionItemChange::GetAppName() {
			return Item.App.get<std::wstring>();
		}
		uint8_t AudioSessionItemChange::GetVolume() {
			return Item.Volume.get<uint8_t>();
		}
		bool AudioSessionItemChange::GetMute() {
			return Item.Volume.get<bool>();
		}

		void AudioSessionItemChange::SetVolume(const uint8_t u) {
			Item.Volume.set<uint8_t>(u, true);
		}
		void AudioSessionItemChange::SetMute(const bool b) {
			Item.Volume.set<bool>(b, true);
		}

		log_string AudioSessionItemChange::to_string() {
			try {
				log_string s;
				if (Item.IsEmptyApp()) {
					if (!Item.App.get<std::wstring>().empty())
						s << L"*[" << Item.App.get<std::wstring>() << L"], cmd:" << AudioSessionHelper::OnChangeTypeHelper(GetAction()) << L" - ";
					s << common_error_code::Get().get_error(common_error_id::err_EMPTY) << L"\n";
				} else {
					s << L"*[" << Item.App.get<std::wstring>() << L"/" << Item.App.Pid << L"/" << Utils::to_string(Item.App.Guid) << L"], cmd:["
						<< AudioSessionHelper::OnChangeTypeHelper(GetAction()) << L"/"
						<< AudioSessionHelper::TypeItemsHelper(Item.GetType()) << L"]"
						<< L", callcb=" << std::boolalpha << iscallaudiocb_
						<< L", init=" << isinit_
						<< L", " << Item.Volume.to_string();
					if (Item.Id)
						s << L",\n" << Item.Id.to_string() << L"\n";
				}
				return s;
			} catch (...) {}
			return log_string();
		}

		OnChangeType AudioSessionItemChange::selector_type(OnChangeType t) {
			switch (t) {
				case OnChangeType::OnChangeNew:
				case OnChangeType::OnChangeUpdateData:
				case OnChangeType::OnChangeNoCbUpdateData:		return OnChangeType::OnChangeUpdateData;
				case OnChangeType::OnChangeUpdatePan:
				case OnChangeType::OnChangeUpdateMute:
				case OnChangeType::OnChangeUpdateVolume:
				case OnChangeType::OnChangeUpdateAllValues:
				case OnChangeType::OnChangeNoCbUpdatePan:
				case OnChangeType::OnChangeNoCbUpdateMute:
				case OnChangeType::OnChangeNoCbUpdateVolume:
				case OnChangeType::OnChangeNoCbUpdateAllValues: return OnChangeType::OnChangeUpdateAllValues;
				case OnChangeType::OnChangeRemove:				return OnChangeType::OnChangeRemove;
				case OnChangeType::OnNoChange:
				case OnChangeType::OnChangeNone:
				default:										return OnChangeType::OnChangeNone;
			}
		}
		OnChangeType AudioSessionItemChange::filter_type(OnChangeType t) {
			switch (t) {
				case OnChangeType::OnChangeNew:
				case OnChangeType::OnChangeRemove:
				case OnChangeType::OnChangeUpdateData:
				case OnChangeType::OnChangeUpdatePan:
				case OnChangeType::OnChangeUpdateMute:
				case OnChangeType::OnChangeUpdateVolume:
				case OnChangeType::OnChangeUpdateAllValues: return t;
				case OnChangeType::OnChangeNoCbUpdatePan:	return OnChangeType::OnChangeUpdatePan;
				case OnChangeType::OnChangeNoCbUpdateMute:	return OnChangeType::OnChangeUpdateMute;
				case OnChangeType::OnChangeNoCbUpdateData:	return OnChangeType::OnChangeUpdateData;
				case OnChangeType::OnChangeNoCbUpdateVolume:return OnChangeType::OnChangeUpdateVolume;
				case OnChangeType::OnChangeNoCbUpdateAllValues: return OnChangeType::OnChangeUpdateAllValues;
				case OnChangeType::OnNoChange:
				case OnChangeType::OnChangeNone:
				default:									return OnChangeType::OnChangeNone;
			}
		}
		bool AudioSessionItemChange::filter_cb(OnChangeType t) {
			switch (t) {
				case OnChangeType::OnNoChange:
				case OnChangeType::OnChangeNew:
				case OnChangeType::OnChangeNone:
				case OnChangeType::OnChangeRemove:
				case OnChangeType::OnChangeNoCbUpdatePan:
				case OnChangeType::OnChangeNoCbUpdateData:
				case OnChangeType::OnChangeNoCbUpdateMute:
				case OnChangeType::OnChangeNoCbUpdateVolume:
				case OnChangeType::OnChangeNoCbUpdateAllValues: return false;
				default:										return true;
			}
		}
	}
}