#include "pch.h"
#include "CppUnitTest.h"

#include "../../Common/h/CommonApi.h"
#include "../../MIDIMTDLL/MIDIMTApi.h"
#include <stdexcept>

#pragma comment(lib, "MIDIMTLIB.lib")

using namespace std::placeholders;
using namespace std::string_view_literals;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
/* using namespace Common; */

namespace MIDIMTLIBTest
{
	TEST_CLASS(MIDIMTLIBTest)
	{
	public:

		void print(const std::wstring& w) {
			Common::log_string ls;
			ls << L"*LOG {" << w.c_str() << "}\n";
			const std::wstring ws = ls.str();
			Logger::WriteMessage(ws.c_str());
		}

		MIDIMTLIBTest() {
			Common::to_log& log = Common::to_log::Get();
			log.add(
				std::bind(static_cast<void(MIDIMTLIBTest::*)(const std::wstring&)>(&MIDIMTLIBTest::print), this, _1)
			);
		}

		TEST_CLASS_INITIALIZE(ClassInitialize) {
			Logger::WriteMessage(L"MIDIMTLIB test Initialize\n");
		}
		TEST_CLASS_CLEANUP(ClassCleanup) {
			Logger::WriteMessage(L"MIDIMTLIB test Cleanup\n");
		}

		TEST_METHOD(TestMethod1) {
			try {
				throw Common::make_common_error(Common::common_error_id::err_JSONCONF_WRITE_EMPTY);
			} catch (...) {
				Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		TEST_METHOD(TestMethod2) {
			try {
				throw Common::make_common_error(
					Common::log_string().to_log_format(
						__FUNCTIONW__,
						Common::common_error_code::Get().get_error(Common::common_error_id::err_MIDI_COUNT_DEVICES),
						1, 2
					).str()
				);
			} catch (...) {
				Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		TEST_METHOD(TestMethod3) {
			try {
				throw Common::make_common_error(
					Common::log_string().to_log_format(
						__FUNCTIONW__,
						Common::common_error_code::Get().get_error(Common::common_error_id::err_MIDI_COUNT_DEVICES),
						1, 2
					)
				);
			} catch (...) {
				Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		TEST_METHOD(TestMethod4) {
			try {
				Common::to_log::Get() << Common::log_string().to_log_format(
					__FUNCTIONW__,
					Common::common_error_code::Get().get_error(Common::common_error_id::err_MIDI_DEVICE_OPEN),
					1, 2
				);
			} catch (...) {
				Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				Assert::Fail();
			}
		}
		TEST_METHOD(TestMethod5) {
			try {
				const std::wstring_view s1 = L"1 - Find linked bugs by selecting the bugs indicator..."sv;
				const std::string s2 = "2 - Find linked bugs by selecting the bugs indicator...";
				const LPWSTR s3 = LPWSTR(L"3 - Find linked bugs by selecting the bugs indicator...");
				const char s4[] = "4 - Find linked bugs by selecting the bugs indicator...";
				const char* s5 = "5 - Выберите значок для получения дополнительной информации...";
				const std::wstring s6 = L"6 - Выберите значок для получения дополнительной информации...";
				const std::wstring s7 = L"7 - nch number of character to convert or 0 if string is null-terminated...";

				print(Common::Utils::to_string(s1));
				print(Common::Utils::to_string(s2));
				print(Common::Utils::to_string(s3));
				print(Common::Utils::to_string(s4));
				print(Common::Utils::to_string(s5));
				print(Common::Utils::to_string(s6));
				print(Common::Utils::to_string(s7));

			} catch (...) {
				Common::Utils::get_exception(std::current_exception(), __FUNCTIONW__);
				Assert::Fail();
			}
		}
	};
}
