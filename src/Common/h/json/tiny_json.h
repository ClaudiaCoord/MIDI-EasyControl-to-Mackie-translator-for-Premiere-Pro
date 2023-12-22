/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace Tiny {

		class TinyJson;
		class ParseJson;

		template<typename T>
		class ValueArray : public T {
		protected:
			std::vector<std::wstring> vo_{};
		public:
			ValueArray() {}
			ValueArray(std::vector<std::wstring> vo) { vo_ = vo; }
			bool Enter(uint16_t i = 0U) {
				if (i >= vo_.size()) return false;
				std::wstring obj = vo_[i];
				if (obj.empty()) return false;
				return this->ReadJson(obj);
			}
			size_t Count() { return vo_.size(); }
		};

		template<typename T>
		class VArray : public ValueArray<T> {
		public:
			VArray() {}
			VArray(std::vector<std::wstring> vo) : ValueArray<T>::ValueArray(std::move(vo)) {}
		};

		template<typename T>
		class VObject : public ValueArray<T> {
		public:
			VObject() {}
			VObject(std::vector<std::wstring> vo) : ValueArray<T>::ValueArray(std::move(vo)) {}
		};

		typedef VArray<TinyJson> xarray;
		typedef VObject<TinyJson> xobject;

		class FLAG_EXPORT Value {
		private:
			std::wstring value_{};
			bool nokey_;

		public:
			Value();
			Value(std::wstring val);
			~Value();

			std::wstring value();
			template<typename T1>
			T1 GetAs() {
				std::wistringstream iss(value_);
				T1 v{};
				iss >> v;
				return v;
			}
			template<typename T2>
			void Set(T2 v) {
				std::wostringstream oss;
				if (nokey_)	oss << v;
				else		oss << L"\"" << value_ << L"\"" << L":" << v;
				value_ = oss.str();
			}

			template<typename T3>
			void Push(T3& v) {
				std::wostringstream oss;
				if (v.get_nokey()) oss << v.WriteJson(0);
				else			   oss << v.WriteJson(1);
				value_ = oss.str();
			}
		};

		template<> inline bool Value::GetAs() { return value_ == L"true" ? true : false; }
		template<> inline std::wstring Value::GetAs() { return value_; }
		template<> inline void Value::Set(std::wstring v) {
			std::wostringstream oss;
			if (nokey_) oss << L"\"" << v << L"\"";
			else		oss << L"\"" << value_ << L"\"" << L":" << L"\"" << v << L"\"";
			value_ = oss.str();
		}
		template<> inline void Value::Set(const wchar_t* v) {
			Set(std::wstring(v));
		}
		template<> inline void Value::Set(bool v) {
			std::wostringstream oss;
			std::wstring val = v == true ? L"\"true\"" : L"\"false\"";
			if (nokey_)	oss << val;
			else		oss << L"\"" << value_ << L"\"" << L":" << val;
			value_ = oss.str();
		}

		class FLAG_EXPORT ParseJson {
		private:
			std::vector<wchar_t> token_;
			std::vector<std::wstring> keyval_;
		public:
			ParseJson();
			~ParseJson();

			bool ParseArray(std::wstring json, std::vector<std::wstring>& vo);
			bool ParseObj(std::wstring json);
			std::vector<std::wstring> GetKeyVal();

		protected:
			std::wstring FetchArrayStr_(std::wstring inputstr, int inpos, int& offset);
			std::wstring FetchObjStr_(std::wstring inputstr, int inpos, int& offset);
			std::wstring FetchStrStr_(std::wstring inputstr, int inpos, int& offset);
			std::wstring FetchNumStr_(std::wstring inputstr, int inpos, int& offset);
		};

		class FLAG_EXPORT TinyJson {
			friend class ValueArray<TinyJson>;
			friend class VObject<TinyJson>;
			friend class VArray<TinyJson>;
		private:
			std::vector<std::wstring> KeyVal_{};
			std::vector<Value> Items_{};
			bool isnokey_;

		public:

			bool isarray_;

			TinyJson();
			~TinyJson();

			template<typename T1>
			inline T1 Get(std::wstring key, T1 defVal) {
				auto itr = std::find(KeyVal_.begin(), KeyVal_.end(), key);
				if (itr == KeyVal_.end())
					return defVal;
				return Value(*(++itr)).GetAs<T1>();
			}
			template<typename T2>
			inline T2 Get(std::wstring key) {
				return Get(key, T2());
			}
			template<typename T3>
			inline T3 Get() {
				return Value(KeyVal_[0]).GetAs<T3>();
			}

			Value& operator[] (std::wstring k);

			bool ReadJson(std::wstring json);
			void Push(TinyJson item);
			bool get_nokey();
			std::wstring WriteJson();
			std::wstring WriteJson(int type);
		};

		FLAG_EXPORT std::wostream& operator << (std::wostream& os, TinyJson& ob);

		template<> inline xobject TinyJson::Get<xobject>(std::wstring key) {
			std::wstring val = Get<std::wstring>(key);
			if (val.empty()) return xobject();
			std::vector<std::wstring> vo{ val };
			return xobject(vo);
		}
		template<> inline xarray TinyJson::Get<xarray>(std::wstring key) {
			std::wstring val = Get<std::wstring>(key);
			ParseJson p{};
			std::vector<std::wstring> vo{};
			if (!p.ParseArray(val, vo)) return xarray();
			return xarray(vo);
		}
		template<> inline void Value::Set(TinyJson v) {
			std::wostringstream oss;
			if (v.isarray_)
				oss << L"\"" << value_ << L"\"" << L":" << v.WriteJson(2);
			else
				oss << L"\"" << value_ << L"\"" << L":" << v.WriteJson(1);
			value_ = oss.str();
		}
	}
}
