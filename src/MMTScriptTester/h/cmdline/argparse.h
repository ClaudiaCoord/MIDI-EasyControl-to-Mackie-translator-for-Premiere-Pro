/**
 * License: Apache 2.0 with LLVM Exception or GPL v3
 *
 * Author: Jesse Laning
 */

#pragma once

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace argparse {
	namespace detail {
		static inline bool _not_space(int ch) { return !std::isspace(ch); }
		static inline void _ltrim(std::wstring& s, bool (*f)(int) = _not_space) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), f));
		}
		static inline void _rtrim(std::wstring& s, bool (*f)(int) = _not_space) {
			s.erase(std::find_if(s.rbegin(), s.rend(), f).base(), s.end());
		}
		static inline void _trim(std::wstring& s, bool (*f)(int) = _not_space) {
			_ltrim(s, f);
			_rtrim(s, f);
		}
		static inline std::wstring _ltrim_copy(std::wstring s,
			bool (*f)(int) = _not_space) {
			_ltrim(s, f);
			return s;
		}
		static inline std::wstring _rtrim_copy(std::wstring s,
			bool (*f)(int) = _not_space) {
			_rtrim(s, f);
			return s;
		}
		static inline std::wstring _trim_copy(std::wstring s,
			bool (*f)(int) = _not_space) {
			_trim(s, f);
			return s;
		}
		template <typename InputIt>
		static inline std::wstring _join(InputIt begin, InputIt end,
			const std::wstring& separator = L" ") {
			std::wostringstream ss;
			if (begin != end) {
				ss << *begin++;
			}
			while (begin != end) {
				ss << separator;
				ss << *begin++;
			}
			return ss.str();
		}
		static inline bool _is_number(const std::wstring& arg) {
			std::wistringstream iss(arg);
			float f;
			iss >> std::noskipws >> f;
			return iss.eof() && !iss.fail();
		}

		static inline int _find_equal(const std::wstring& s) {
			for (size_t i = 0; i < s.length(); ++i) {
				// if find graph symbol before equal, end search
				// i.e. don't accept --asd)f=0 arguments
				// but allow --asd_f and --asd-f arguments
				if (std::ispunct(static_cast<int>(s[i]))) {
					if (s[i] == '=') {
						return static_cast<int>(i);
					}
					else if (s[i] == '_' || s[i] == '-') {
						continue;
					}
					return -1;
				}
			}
			return -1;
		}

		static inline size_t _find_name_end(const std::wstring& s) {
			size_t i;
			for (i = 0; i < s.length(); ++i) {
				if (std::ispunct(static_cast<int>(s[i]))) {
					break;
				}
			}
			return i;
		}

		namespace is_vector_impl {
			template <typename T>
			struct is_vector : std::false_type {};
			template <typename... Args>
			struct is_vector<std::vector<Args...>> : std::true_type {};
		}  // namespace is_vector_impl

		// type trait to utilize the implementation type traits as well as decay the
		// type
		template <typename T>
		struct is_vector {
			static constexpr bool const value =
				is_vector_impl::is_vector<typename std::decay<T>::type>::value;
		};
	}  // namespace detail

	class ArgumentParser {
	private:
	public:
		class Argument;

		class Result {
		public:
			Result() {}
			Result(std::wstring err) noexcept : _error(true), _what(err) {}

			operator bool() const { return _error; }

			//friend std::ostream& operator<<(std::ostream& os, const Result& dt);

			const std::wstring& what() const { return _what; }

		private:
			bool _error{ false };
			std::wstring _what{};
		};

		class Argument {
		public:
			enum Position : int { LAST = -1, DONT_CARE = -2 };
			enum Count : int { ANY = -1 };

			Argument& name(const std::wstring& name) {
				_names.push_back(name);
				return *this;
			}

			Argument& names(std::vector<std::wstring> names) {
				_names.insert(_names.end(), names.begin(), names.end());
				return *this;
			}

			Argument& description(const std::wstring& description) {
				_desc = description;
				return *this;
			}

			Argument& required(bool req) {
				_required = req;
				return *this;
			}

			Argument& position(int position) {
				if (position != Position::LAST) {
					// position + 1 because technically argument zero is the name of the
					// executable
					_position = position + 1;
				}
				else {
					_position = position;
				}
				return *this;
			}

			Argument& count(int count) {
				_count = count;
				return *this;
			}

			bool found() const { return _found; }

			template <typename T>
			typename std::enable_if<detail::is_vector<T>::value, T>::type get() {
				T t = T();
				typename T::value_type vt;
				for (auto& s : _values) {
					std::wistringstream in(s);
					in >> vt;
					t.push_back(vt);
				}
				return t;
			}

			template <typename T>
			typename std::enable_if<!detail::is_vector<T>::value, T>::type get() {
				std::wistringstream in(get<std::wstring>());
				T t = T();
				in >> t >> std::ws;
				return t;
			}

		private:
			Argument(const std::wstring& name, const std::wstring& desc,
				bool required = false)
				: _desc(desc), _required(required) {
				_names.push_back(name);
			}

			Argument() {}

			friend class ArgumentParser;
			int _position{ Position::DONT_CARE };
			int _count{ Count::ANY };
			std::vector<std::wstring> _names{};
			std::wstring _desc{};
			bool _found{ false };
			bool _required{ false };
			int _index{ -1 };

			std::vector<std::wstring> _values{};
		};

		//ArgumentParser(const std::wstring& bin, const std::wstring& desc)
			//: _bin(bin), _desc(desc) {}

		ArgumentParser(const std::wstring_view& bin, const std::wstring_view& desc)
			: _bin(bin), _desc(desc) {}

		Argument& add_argument() {
			_arguments.push_back({});
			_arguments.back()._index = static_cast<int>(_arguments.size()) - 1;
			return _arguments.back();
		}

		Argument& add_argument(const std::wstring& name, const std::wstring& long_name,
			const std::wstring& desc, const bool required = false) {
			_arguments.push_back(Argument(name, desc, required));
			_arguments.back()._names.push_back(long_name);
			_arguments.back()._index = static_cast<int>(_arguments.size()) - 1;
			return _arguments.back();
		}

		Argument& add_argument(const std::wstring& name, const std::wstring& desc,
			const bool required = false) {
			_arguments.push_back(Argument(name, desc, required));
			_arguments.back()._index = static_cast<int>(_arguments.size()) - 1;
			return _arguments.back();
		}

		void print_help(size_t count = 0, size_t page = 0) {
			if (page * count > _arguments.size()) {
				return;
			}
			if (page == 0) {
				std::wcout << "Usage: " << _bin;
				if (_positional_arguments.empty()) {
					std::wcout << " [options...]" << std::endl;
				}
				else {
					int current = 1;
					for (auto& v : _positional_arguments) {
						if (v.first != Argument::Position::LAST) {
							for (; current < v.first; current++) {
								std::wcout << " [" << current << "]";
							}
							std::wcout
								<< " ["
								<< detail::_ltrim_copy(
									_arguments[static_cast<size_t>(v.second)]._names[0],
									[](int c) -> bool { return c != static_cast<int>('-'); })
								<< "]";
						}
					}
					auto it = _positional_arguments.find(Argument::Position::LAST);
					if (it == _positional_arguments.end()) {
						std::wcout << " [options...]";
					}
					else {
						std::wcout
							<< " [options...] ["
							<< detail::_ltrim_copy(
								_arguments[static_cast<size_t>(it->second)]._names[0],
								[](int c) -> bool { return c != static_cast<int>('-'); })
							<< "]";
					}
					std::wcout << std::endl;
				}
				std::wcout << "Options:" << std::endl;
			}
			if (count == 0) {
				page = 0;
				count = _arguments.size();
			}
			for (size_t i = page * count;
				i < std::min<size_t>(page * count + count, _arguments.size()); i++) {
				Argument& a = _arguments[i];
				std::wstring name = a._names[0];
				for (size_t n = 1; n < a._names.size(); ++n) {
					name.append(L", " + a._names[n]);
				}
				std::wcout << "    " << std::setw(23) << std::left << name << std::setw(23)
					<< a._desc;
				if (a._required) {
					std::wcout << L" (Required)";
				}
				std::wcout << std::endl;
			}
		}

		Result parse(int argc, const wchar_t* argv[]) {
			Result err;
			if (argc > 1) {
				// build name map
				for (auto& a : _arguments) {
					for (auto& n : a._names) {
						std::wstring name = detail::_ltrim_copy(
							n, [](int c) -> bool { return c != static_cast<int>('-'); });
						if (_name_map.find(name) != _name_map.end()) {
							return Result(L"Duplicate of argument name: " + n);
						}
						_name_map[name] = a._index;
					}
					if (a._position >= 0 || a._position == Argument::Position::LAST) {
						_positional_arguments[a._position] = a._index;
					}
				}
				if (err) {
					return err;
				}

				// parse
				std::wstring current_arg;
				size_t arg_len;
				for (int argv_index = 1; argv_index < argc; ++argv_index) {
					current_arg = std::wstring(argv[argv_index]);
					arg_len = current_arg.length();
					if (arg_len == 0) {
						continue;
					}
					if (_help_enabled && (current_arg._Equal(L"-h") || current_arg._Equal(L"--help"))) {
						_arguments[static_cast<size_t>(_name_map[L"help"])]._found = true;
					}
					else if (argv_index == argc - 1 &&
						_positional_arguments.find(Argument::Position::LAST) !=
						_positional_arguments.end()) {
						err = _end_argument();
						Result b = err;
						err = _add_value(current_arg, Argument::Position::LAST);
						if (b) {
							return b;
						}
						if (err) {
							return err;
						}
					}
					else if (arg_len >= 2 && !detail::_is_number(current_arg)) {
						// ignores the case if
						// the arg is just a - look for -a (short) or --arg (long) args
						if (current_arg[0] == '-') {
							err = _end_argument();
							if (err) {
								return err;
							}
							// look for --arg (long) args
							if (current_arg[1] == '-') {
								err = _begin_argument(current_arg.substr(2), true, argv_index);
								if (err) {
									return err;
								}
							}
							else {  // short args
								err = _begin_argument(current_arg.substr(1), false, argv_index);
								if (err) {
									return err;
								}
							}
						}
						else {  // argument value
							err = _add_value(current_arg, argv_index);
							if (err) {
								return err;
							}
						}
					}
					else {  // argument value
						err = _add_value(current_arg, argv_index);
						if (err) {
							return err;
						}
					}
				}
			}
			if (_help_enabled && exists(L"help")) {
				return Result();
			}
			err = _end_argument();
			if (err) {
				return err;
			}
			for (auto& p : _positional_arguments) {
				Argument& a = _arguments[static_cast<size_t>(p.second)];
				if (a._values.size() > 0 && a._values[0][0] == '-') {
					std::wstring name = detail::_ltrim_copy(a._values[0], [](int c) -> bool {
						return c != static_cast<int>('-');
						});
					if (_name_map.find(name) != _name_map.end()) {
						if (a._position == Argument::Position::LAST) {
							return Result(
								L"Poisitional argument expected at the end, but argument " +
								a._values[0] + L" found instead");
						}
						else {
							return Result(
								L"Poisitional argument expected in position " +
								std::to_wstring(a._position) + L", but argument " +
								a._values[0] + L" found instead");
						}
					}
				}
			}
			for (auto& a : _arguments) {
				if (a._required && !a._found) {
					return Result(L"Required argument not found: " + a._names[0]);
				}
				if (a._position >= 0 && argc >= a._position && !a._found) {
					return Result(L"Argument " + a._names[0] + L" expected in position " +
						std::to_wstring(a._position));
				}
			}
			return Result();
		}

		void enable_help() {
			add_argument(L"-h", L"--help", L"this help page..", false);
			_help_enabled = true;
		}

		bool exists(const std::wstring& name) const {
			std::wstring n = detail::_ltrim_copy(
				name, [](int c) -> bool { return c != static_cast<int>('-'); });
			auto it = _name_map.find(n);
			if (it != _name_map.end()) {
				return _arguments[static_cast<size_t>(it->second)]._found;
			}
			return false;
		}

		template <typename T>
		T get(const std::wstring& name) {
			auto t = _name_map.find(name);
			if (t != _name_map.end()) {
				return _arguments[static_cast<size_t>(t->second)].get<T>();
			}
			return T();
		}

	private:
		Result _begin_argument(const std::wstring& arg, bool longarg, int position) {
			auto it = _positional_arguments.find(position);
			if (it != _positional_arguments.end()) {
				Result err = _end_argument();
				Argument& a = _arguments[static_cast<size_t>(it->second)];
				a._values.push_back((longarg ? L"--" : L"-") + arg);
				a._found = true;
				return err;
			}
			if (_current != -1) {
				return Result(L"Current argument left open");
			}
			size_t name_end = detail::_find_name_end(arg);
			std::wstring arg_name = arg.substr(0, name_end);
			if (longarg) {
				int equal_pos = detail::_find_equal(arg);
				auto nmf = _name_map.find(arg_name);
				if (nmf == _name_map.end()) {
					return Result(L"Unrecognized command line option '" + arg_name + L"'");
				}
				_current = nmf->second;
				_arguments[static_cast<size_t>(nmf->second)]._found = true;
				if (equal_pos == 0 ||
					(equal_pos < 0 &&
						arg_name.length() < arg.length())) {  // malformed argument
					return Result(L"Malformed argument: " + arg);
				}
				else if (equal_pos > 0) {
					std::wstring arg_value = arg.substr(name_end + 1);
					_add_value(arg_value, position);
				}
			}
			else {
				Result r;
				if (arg_name.length() == 1) {
					return _begin_argument(arg, true, position);
				}
				else {
					for (wchar_t& c : arg_name) {
						r = _begin_argument(std::wstring(1, c), true, position);
						if (r) {
							return r;
						}
						r = _end_argument();
						if (r) {
							return r;
						}
					}
				}
			}
			return Result();
		}

		Result _add_value(const std::wstring& value, int location) {
			if (_current >= 0) {
				Result err;
				Argument& a = _arguments[static_cast<size_t>(_current)];
				if (a._count >= 0 && static_cast<int>(a._values.size()) >= a._count) {
					err = _end_argument();
					if (err) {
						return err;
					}
					goto unnamed;
				}
				a._values.push_back(value);
				if (a._count >= 0 && static_cast<int>(a._values.size()) >= a._count) {
					err = _end_argument();
					if (err) {
						return err;
					}
				}
				return Result();
			}
			else {
			unnamed:
				auto it = _positional_arguments.find(location);
				if (it != _positional_arguments.end()) {
					Argument& a = _arguments[static_cast<size_t>(it->second)];
					a._values.push_back(value);
					a._found = true;
				}
				// TODO
				return Result();
			}
		}

		Result _end_argument() {
			if (_current >= 0) {
				Argument& a = _arguments[static_cast<size_t>(_current)];
				_current = -1;
				if (static_cast<int>(a._values.size()) < a._count) {
					return Result(L"Too few arguments given for " + a._names[0]);
				}
				if (a._count >= 0) {
					if (static_cast<int>(a._values.size()) > a._count) {
						return Result(L"Too many arguments given for " + a._names[0]);
					}
				}
			}
			return Result();
		}

		bool _help_enabled{ false };
		int _current{ -1 };
		std::wstring _bin{};
		std::wstring _desc{};
		std::vector<Argument> _arguments{};
		std::map<int, int> _positional_arguments{};
		std::map<std::wstring, int> _name_map{};
	};

	/*
	std::ostream& operator<<(std::ostream& os, const ArgumentParser::Result& r) {
		os << r.what();
		return os;
	}
	*/

	template <>
	inline std::wstring ArgumentParser::Argument::get<std::wstring>() {
		return detail::_join(_values.begin(), _values.end());
	}
	template <>
	inline std::vector<std::wstring>
		ArgumentParser::Argument::get<std::vector<std::wstring>>() {
		return _values;
	}

}

