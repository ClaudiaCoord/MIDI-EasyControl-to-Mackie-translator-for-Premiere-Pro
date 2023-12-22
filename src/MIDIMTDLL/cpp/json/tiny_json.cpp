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

/**
*	Origin:
*	tiny::TinyJson library
*	Copyright 2017 Button
*
*/

namespace Common {
	namespace Tiny {

		using namespace std::string_view_literals;
		static constexpr std::wstring_view FALSE_ = L"false"sv;
		static constexpr std::wstring_view TRUE_ = L"true"sv;

		enum class FetchType : int {
			Fetch_None = 0,
			Fetch_Array,
			Fetch_Object,
			FetchString,
			FetchNumber
		};

		static inline std::wstring FetchTrims_(std::wstring s, wchar_t lc, wchar_t rc) {
			if ((s.find(lc) == std::wstring::npos) && (s.find(rc) == std::wstring::npos))
				return s;
			size_t b = s.find_first_of(lc);
			size_t e = s.find_last_of(rc);
			return s.substr(b + 1, e - b - 1);
		}

		template<FetchType T1>
		static inline std::wstring FetchObject_(const wchar_t l, const wchar_t r, std::wstring istr, int ipos, int& offset) {
			std::wstring ostr{};
			[[maybe_unused]] long tcount = 0L;
			long sz = static_cast<long>(istr.size()), i = ipos;

			for (; i < sz; i++) {
				if (::iswspace(istr[i])) continue;
				break;
			}
			for (; i < sz; i++) {
				wchar_t c = istr[i];
				if constexpr ((T1 == FetchType::Fetch_Object) || (T1 == FetchType::Fetch_Array) || (T1 == FetchType::FetchString)) {
					if (c == l) ++tcount;
					if (c == r) --tcount;
				} else if constexpr (T1 == FetchType::FetchNumber) {
					if ((c == l) || (c == r)) break;
				}

				ostr.push_back(c);

				if constexpr ((T1 == FetchType::Fetch_Object) || (T1 == FetchType::Fetch_Array)) {
					if (tcount == 0) break;
				} else if constexpr (T1 == FetchType::FetchString) {
					if ((tcount % 2 == 0) && ((c == L',') || (c == L':'))) break;
				}
			}
			offset = i - ipos;
			return ostr;
		}

		Value::Value() {
			value_.clear();
			nokey_ = false;
		}
		Value::Value(std::wstring val) : value_(val) {
			if (value_ == L"") {
				value_.clear();
				nokey_ = true;
			}
			else {
				nokey_ = false;
			}
		}
		Value::~Value() {}
		std::wstring Value::value() { return value_; }

		ParseJson::ParseJson() {}
		ParseJson::~ParseJson() {}
		std::vector<std::wstring> ParseJson::GetKeyVal() {
			return keyval_;
		}

		bool ParseJson::ParseArray(std::wstring json, std::vector<std::wstring>& vo) {
			json = FetchTrims_(json, L'[', L']');
			std::wstring tokens;

			for (int i = 0; i < static_cast<long>(json.size()); ++i) {
				wchar_t c = json[i];
				if (::iswspace(c) || c == L'\"') continue;
				if (c == L':' || c == L',' || c == L'{') {
					if (!tokens.empty()) {
						vo.push_back(tokens);
						tokens.clear();
					}
					if (c == L',') continue;
					int offset = 0;
					wchar_t nextc = c;
					for (; c != L'{';) {
						nextc = json[++i];
						if (::iswspace(nextc)) continue;
						break;
					}
					if (nextc == L'{') {
						tokens = FetchObjStr_(json, i, offset);
					}
					else if (nextc == L'[') {
						tokens = FetchArrayStr_(json, i, offset);
					}
					i += offset;
					continue;
				}
				tokens.push_back(c);
			}
			if (!tokens.empty()) {
				vo.push_back(tokens);
			}
			return true;
		}
		bool ParseJson::ParseObj(std::wstring json) {
			auto LastValidChar = [&](int index) -> wchar_t {
				for (int i = index - 1; i >= 0; --i) {
					if (::iswspace(json[i])) continue;
					wchar_t tmp = json[i];
					return tmp;
				}
				return L'\0';
			};

			json = FetchTrims_(json, L'{', L'}');
			
			for (int i = 0; i < static_cast<long>(json.size()); ++i) {
				wchar_t nextc = json[i];
				if (::iswspace(nextc)) continue;

				std::wstring tokens{};
				int offset = 0;
				if (nextc == L'{') {
					tokens = FetchObjStr_(json, i, offset);
				}
				else if (nextc == L'[') {
					tokens = FetchArrayStr_(json, i, offset);
				}
				else if (nextc == L'\"') {
					tokens = FetchStrStr_(json, i, offset);
				}
				else if ((::iswdigit(nextc) || (nextc == L'-')) && (LastValidChar(i) == L':')) {
					tokens = FetchNumStr_(json, i, offset);
				}
				else if ((LastValidChar(i) == L':') && ((nextc == L'f') || (nextc == L't'))) {
					bool b = (nextc == L't');
					std::wstring s = json.substr(i, (b ? 4 : 5));
					if (b && s.starts_with(TRUE_.data()))		 tokens = TRUE_.data();
					else if (!b && s.starts_with(FALSE_.data())) tokens = FALSE_.data();
				}
				else continue;

				keyval_.push_back(tokens);
				i += offset;
			}
			if (keyval_.size() == 0)
				keyval_.push_back(json);
			return true;
		}
		std::wstring ParseJson::FetchArrayStr_(std::wstring istr, int ipos, int& offset) {
			return FetchObject_<FetchType::Fetch_Array>(L'[', L']', istr, ipos, offset);
		}
		std::wstring ParseJson::FetchObjStr_(std::wstring istr, int ipos, int& offset) {
			return FetchObject_<FetchType::Fetch_Object>(L'{', L'}', istr, ipos, offset);
		}
		std::wstring ParseJson::FetchStrStr_(std::wstring istr, int ipos, int& offset) {
			std::wstring ostr = FetchObject_<FetchType::FetchString>(L'\"', L'\0', istr, ipos, offset);
			return FetchTrims_(ostr, L'\"', L'\"');
		}
		std::wstring ParseJson::FetchNumStr_(std::wstring istr, int ipos, int& offset) {
			return FetchObject_<FetchType::FetchNumber>(L',', L'\0', istr, ipos, offset);
		}

		TinyJson::TinyJson() : isnokey_(false), isarray_(false) { }
		TinyJson::~TinyJson() {}

		bool TinyJson::ReadJson(std::wstring json) {
			ParseJson p{};
			p.ParseObj(json);
			KeyVal_ = p.GetKeyVal();
			return true;
		}
		Value& TinyJson::operator[] (std::wstring k) {
			Items_.push_back(Value(k));
			Value& v = Items_[Items_.size() - 1];
			if (k == L"") isnokey_ = true;
			return v;
		}
		void TinyJson::Push(TinyJson item) {
			Items_.push_back(Value(L""));
			Value& v = Items_[Items_.size() - 1];
			isnokey_ = true;
			v.Push(item);
			isarray_ = true;
		}
		bool TinyJson::get_nokey() {
			return isnokey_;
		}
		std::wstring TinyJson::WriteJson() {
			return WriteJson(1);
		}
		std::wstring TinyJson::WriteJson(int type)
		{
			std::wstring prefix = (type == 1) ? L"{" : ((type == 2) ? L"[" : L"");
			std::wstring suffix = (type == 1) ? L"}" : ((type == 2) ? L"]" : L"");
			std::wstringstream oss;
			oss << prefix;
			int i = 0;
			long size = static_cast<long>(Items_.size());
			std::wstring seq = L",";
			for (; i < size; ++i) {
				Value& v = Items_[i];
				oss << v.value() << seq;
			}
			std::wstring jsonstring = oss.str();
			if (jsonstring.back() == L',') {
				jsonstring = jsonstring.substr(0, jsonstring.find_last_of(L','));
			}
			jsonstring += suffix;
			return jsonstring;
		}

		std::wostream& operator << (std::wostream& os, TinyJson& ob) {
			os << ob.WriteJson();
			return os;
		}
	}
}