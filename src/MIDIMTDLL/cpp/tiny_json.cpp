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
			json = Trims_(json, L'[', L']');
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
			auto LastValidChar = [&](int index)->wchar_t {
				for (int i = index - 1; i >= 0; --i) {
					if (::iswspace(json[i])) continue;
					wchar_t tmp = json[i];
					return tmp;
				}
				return L'\0';
			};

			json = Trims_(json, L'{', L'}');
			
			for (int i = 0; i < static_cast<long>(json.size()); ++i) {
				wchar_t nextc = json[i];
				if (::iswspace(nextc)) continue;

				std::wstring tokens;
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
				else if ((isdigit(nextc) || nextc == L'-') && LastValidChar(i) == L':')
				{
					tokens = FetchNumStr_(json, i, offset);
				}
				else {
					continue;
				}
				keyval_.push_back(tokens);
				i += offset;
			}
			if (keyval_.size() == 0)
				keyval_.push_back(json);
			return true;
		}
		int  ParseJson::GetFirstNotSpaceChar_(std::wstring& s, int cur)
		{
			for (int i = cur; i < static_cast<long>(s.size()); i++) {
				if (::iswspace(s[i])) continue;
				return i - cur;
			}
			return 0;
		}
		std::wstring ParseJson::Trims_(std::wstring s, wchar_t lc, wchar_t rc)
		{
			std::wstring ss = s;
			if (s.find(lc) != std::wstring::npos && s.find(rc) != std::wstring::npos) {
				size_t b = s.find_first_of(lc);
				size_t e = s.find_last_of(rc);
				ss = s.substr(b + 1, e - b - 1);
			}
			return ss;
		}
		std::wstring ParseJson::FetchArrayStr_(std::wstring inputstr, int inpos, int& offset)
		{
			int tokencount = 0;
			std::wstring objstr;
			int i = static_cast<int>(inpos + GetFirstNotSpaceChar_(inputstr, inpos));
			for (; i < static_cast<long>(inputstr.size()); i++) {
				wchar_t c = inputstr[i];
				if (c == L'[') ++tokencount;
				if (c == L']') --tokencount;
				objstr.push_back(c);
				if (tokencount == 0) break;
			}
			offset = i - inpos;
			return objstr;
		}
		std::wstring ParseJson::FetchObjStr_(std::wstring inputstr, int inpos, int& offset)
		{
			int tokencount = 0;
			std::wstring objstr;
			int i = static_cast<int>(inpos + GetFirstNotSpaceChar_(inputstr, inpos));
			for (; i < static_cast<long>(inputstr.size()); i++) {
				wchar_t c = inputstr[i];
				if (c == L'{') {
					++tokencount;
				}
				if (c == L'}') {
					--tokencount;
				}
				objstr.push_back(c);
				if (tokencount == 0) {
					break;
				}
			}
			offset = i - inpos;
			return objstr;
		}
		std::wstring ParseJson::FetchStrStr_(std::wstring inputstr, int inpos, int& offset)
		{
			int tokencount = 0;
			std::wstring objstr;
			int i = static_cast<int>(inpos + GetFirstNotSpaceChar_(inputstr, inpos));
			for (; i < static_cast<long>(inputstr.size()); i++) {
				wchar_t c = inputstr[i];
				if (c == L'\"') {
					++tokencount;
				}
				objstr.push_back(c);
				if (tokencount % 2 == 0 && (c == L',' || c == L':')) {
					break;
				}
			}
			offset = i - inpos;

			return Trims_(objstr, L'\"', L'\"');
		}
		std::wstring ParseJson::FetchNumStr_(std::wstring inputstr, int inpos, int& offset)
		{
			std::wstring objstr;
			int i = static_cast<int>(inpos + GetFirstNotSpaceChar_(inputstr, inpos));
			for (; i < static_cast<long>(inputstr.size()); i++) {
				wchar_t c = inputstr[i];
				if (c == ',') {
					break;
				}
				objstr.push_back(c);
			}
			offset = i - inpos;

			return objstr;
		}

		TinyJson::TinyJson() : isnokey_(false), isarray_(false) { }
		TinyJson::~TinyJson() {}

		bool TinyJson::ReadJson(std::wstring json) {
			ParseJson p;
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
			std::wstring prefix = (type == 0) ? L"" : ((type == 1) ? L"{" : L"[");
			std::wstring suffix = (type == 0) ? L"" : ((type == 1) ? L"}" : L"]");
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