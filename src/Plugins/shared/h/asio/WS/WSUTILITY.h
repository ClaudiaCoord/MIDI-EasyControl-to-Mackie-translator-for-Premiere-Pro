/*
	MIDI-MT is a high level application driver for USB MIDI control surface.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (Remote Controls Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!

	Original authored by a Ole Christian Eidheim: https://gitlab.com/eidheim
*/

#pragma once

namespace Common {
	namespace WS {

		inline bool case_insensitive_equal(const std::string& str1, const std::string& str2) {
			return str1.size() == str2.size() &&
				std::equal(str1.begin(), str1.end(), str2.begin(), [](char a, char b) {
				return tolower(a) == tolower(b);
			});
		}

		class CaseInsensitiveEqual {
		public:
			bool operator()(const std::string& str1, const std::string& str2) const {
				return case_insensitive_equal(str1, str2);
			}
		};

		class CaseInsensitiveHash {
		public:
			std::size_t operator()(const std::string& str) const {
				std::size_t h = 0;
				std::hash<int> hash;
				for (auto c : str)
					h ^= hash(tolower(c)) + 0x9e3779b9 + (h << 6) + (h >> 2);
				return h;
			}
		};

		using CaseInsensitiveMultimap = std::unordered_multimap<std::string, std::string, CaseInsensitiveHash, CaseInsensitiveEqual>;

		class HttpHeader {
		public:
			static CaseInsensitiveMultimap parse(std::istream& stream) {
				CaseInsensitiveMultimap result;
				std::string line;
				std::size_t param_end;
				while (getline(stream, line) && (param_end = line.find(':')) != std::string::npos) {
					std::size_t value_start = param_end + 1;
					while (value_start + 1 < line.size() && line[value_start] == ' ') ++value_start;
					if (value_start < line.size())
						result.emplace(line.substr(0, param_end), line.substr(value_start, line.size() - value_start - (line.back() == '\r' ? 1 : 0)));
				}
				return result;
			}
		};

		class RequestMessage {
		public:
			static bool parse(std::istream& stream, std::string& method, std::string& path, std::string& query_string, std::string& version, CaseInsensitiveMultimap& header) {
				std::string line;
				std::size_t method_end;
				if (getline(stream, line) && (method_end = line.find(' ')) != std::string::npos) {
					method = line.substr(0, method_end);

					std::size_t query_start = std::string::npos;
					std::size_t path_and_query_string_end = std::string::npos;
					for (std::size_t i = method_end + 1; i < line.size(); ++i) {
						if (line[i] == '?' && (i + 1) < line.size() && query_start == std::string::npos)
							query_start = i + 1;
						else if (line[i] == ' ') {
							path_and_query_string_end = i;
							break;
						}
					}
					if (path_and_query_string_end != std::string::npos) {
						if (query_start != std::string::npos) {
							path = line.substr(method_end + 1, query_start - method_end - 2);
							query_string = line.substr(query_start, path_and_query_string_end - query_start);
						}
						else
							path = line.substr(method_end + 1, path_and_query_string_end - method_end - 1);

						std::size_t protocol_end;
						if ((protocol_end = line.find('/', path_and_query_string_end + 1)) != std::string::npos) {
							if (line.compare(path_and_query_string_end + 1, protocol_end - path_and_query_string_end - 1, "HTTP") != 0)
								return false;
							version = line.substr(protocol_end + 1, line.size() - protocol_end - 2);
						}
						else
							return false;

						header = HttpHeader::parse(stream);
					}
					else
						return false;
				}
				else
					return false;
				return true;
			}
		};

		class ScopeRunner {
		private:
			std::atomic<long> count;

		public:
			class SharedLock {
				friend class ScopeRunner;
				std::atomic<long>& count;
				SharedLock(std::atomic<long>& count) : count(count) {}
				SharedLock& operator=(const SharedLock&) = delete;
				SharedLock(const SharedLock&) = delete;

			public:
				~SharedLock() {
					count.fetch_sub(1);
				}
			};

			ScopeRunner() : count(0) {}

			std::unique_ptr<SharedLock> continue_lock() {
				long expected = count;
				while (expected >= 0 && !count.compare_exchange_weak(expected, expected + 1))
					spin_loop_pause();

				if (expected < 0) return nullptr;
				else return std::unique_ptr<SharedLock>(new SharedLock(count));
			}

			void stop() {
				long expected = 0;
				while (!count.compare_exchange_weak(expected, -1)) {
					if (expected < 0) return;
					expected = 0;
					spin_loop_pause();
				}
			}
		};
	}
}