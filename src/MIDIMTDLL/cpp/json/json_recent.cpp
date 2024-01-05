/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include <set>

namespace Common {
	namespace JSON {

		using namespace Tiny;
		using namespace std::string_view_literals;

		constexpr wchar_t FIELD_RECENTS[] = L"recents";
		constexpr std::wstring_view name_conf__ = L"RecentConfig.json"sv;

		json_recent::json_recent() {}

		void json_recent::Load() {
			try {
				if (!recents__.empty()) return;

				auto f = std::async(std::launch::async, [=]() {
					std::wstring cnf = Utils::runing_dir(name_conf__);
					(void)read_(cnf);
				});
				(void)f.get();
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void json_recent::Add(std::wstring& s) {
			try {
				if (s.empty()) return;

				worker_background::Get().to_async(std::async(std::launch::async, [=]() {
					recents__.push_back(s);
					std::set<std::wstring> u(recents__.begin(), recents__.end());
					recents__.assign(u.begin(), u.end());

					std::wstring cnf = Utils::runing_dir(name_conf__);
					(void)write_(cnf);
				}));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		std::vector<std::wstring>& json_recent::GetRecents() {
			Load();
			return std::ref(recents__);
		}
		std::wstring json_recent::Dump() {
			if (recents__.empty()) return L"";
			std::wstringstream ss{};

			ss << L"\t" << FIELD_RECENTS << L"=" << static_cast<int>(recents__.size()) << ":\n";
			for (auto& r : recents__)
				ss << L"\t\t\"" << r << "\",\n";
			return ss.str().c_str();
		}

		#pragma region private
		bool json_recent::read_(std::wstring cnfpath) {
			try {
				recents__.clear();
				do {
					std::wstring jtxt;
					std::wifstream cnf(cnfpath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
					if (cnf.is_open()) {
						#pragma warning( push )
						#pragma warning( disable : 4244 )
						std::streampos size = cnf.tellg();
						if (size == 0U) {
							to_log::Get() << log_string().to_log_format(
								__FUNCTIONW__,
								common_error_code::Get().get_error(common_error_id::err_JSONCONF_READ_ACCESS),
								cnfpath
							);
							break;
						}
						size_t bsz = 1 + size;
						#pragma warning( pop )

						wchar_t* text = new wchar_t[bsz] {};
						try {
							cnf.seekg(0, std::ios::beg);
							cnf.read(text, size);
							cnf.close();
							jtxt = std::wstring(text);
						} catch (...) {
							Utils::get_exception(std::current_exception(), __FUNCTIONW__);
						}
						delete[] text;
					}
					else {
						to_log::Get() << log_string().to_log_format(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_JSONCONF_READ_ACCESS),
							cnfpath
						);
						break;
					}
					if (jtxt.empty()) {
						to_log::Get() << log_string().to_log_format(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_JSONCONF_EMPTY),
							cnfpath
						);
						break;
					}

					TinyJson rjson;
					rjson.ReadJson(jtxt);

					try {
						xarray recents = rjson.Get<xarray>(FIELD_RECENTS);
						size_t cnt = recents.Count();

						if (cnt > 0) {
							for (uint16_t n = 0; n < cnt; n++) {
								recents.Enter(n);
								std::wstring a = recents.Get<std::wstring>();
								if (!a.empty()) recents__.push_back(a);
							}
						}
					} catch (...) {}
					return true;
				} while (0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		bool json_recent::write_(std::wstring cnfpath) {
			try {
				do {
					if (recents__.empty()) break;

					TinyJson rjson;
					TinyJson itemsjson;
					TinyJson recentsjson;
					for (auto& r : recents__)
						itemsjson[L""].Set(r);

					recentsjson.Push(std::move(itemsjson));
					rjson[FIELD_RECENTS].Set(std::move(recentsjson));

					std::wofstream cnf(cnfpath.c_str(), std::ios::trunc);
					if (!cnf.is_open()) {
						to_log::Get() << log_string().to_log_format(
							__FUNCTIONW__,
							common_error_code::Get().get_error(common_error_id::err_JSONCONF_WRITE_ACCESS),
							cnfpath
						);
						break;
					}
					cnf << rjson.WriteJson();
					cnf.close();
					return true;
				} while (0);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return false;
		}
		#pragma endregion
	}
}
