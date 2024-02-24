/**
 * @file ChaiScript String Methods
 *
 * Adds some additional string methods to ChaiScript strings:
 *
 *     string::replace(string search, string replace)
 *     string::replace(char search, char replace)
 *     string::trim()
 *     string::trimStart()
 *     string::trimEnd()
 *     string::split(string token)
 *     string::toLowerCase()
 *     string::toUpperCase()
 *     string::includes(string search)
 *
 * To allow selecting indexes from split(), ensure the vector of strings type is added:
 *
 *     chai.add(chaiscript::bootstrap::standard_library::vector_type<std::vector<std::wstring>>("VectorWstring"));
 */

#ifndef CHAISCRIPT_EXTRAS_WSTRING_METHODS_HPP_
#define CHAISCRIPT_EXTRAS_WSTRING_METHODS_HPP_

#include <algorithm>
#include <string>
#include <vector>
#include "../chaiscript.hpp"

namespace chaiscript {
  namespace extras {
    namespace wstring_methods {

      /**
       * Replaces all occurances of a string within the given string.
       *
       * @code
       * var hello = "Hello World"
       * hello.replace("Hello", "Goodbye")
       * // => "Goodbye World"
       * @endcode
       *
       * @see replaceChar
       */
      std::wstring replaceWstring(const std::wstring& subject, const std::wstring& search, const std::wstring& replace) {
        std::wstring result(subject);
        size_t pos = 0;
        while ((pos = result.find(search, pos)) != std::wstring::npos) {
          result.replace(pos, search.length(), replace);
          pos += replace.length();
        }
        return result;
      }

      /**
       * Replaces all occurances of a character within the given character.
       *
       * @see replaceString
       */
      std::wstring replaceWchar(const std::wstring& subject, wchar_t search, wchar_t replace) {
        std::wstring result(subject);
        std::replace(result.begin(), result.end(), search, replace);
        return result;
      }

      /**
       * Trims the given string.
       */
      std::wstring trimW(const std::wstring& subject) {
        std::wstring result(subject);
        std::wstring delimiters = L"\t\n\v\f\r ";
        result.erase(0, result.find_first_not_of(delimiters));
        result.erase(0, result.find_last_not_of(delimiters));
        return result;
      }

      /**
       * Trims the beginning of the given string.
       */
      std::wstring trimStartW(const std::wstring& subject) {
        std::wstring result(subject);
        std::wstring delimiters = L"\t\n\v\f\r ";
        result.erase(0, result.find_first_not_of(delimiters));
        return result;
      }

      /**
       * Trims the end of the given string.
       */
      std::wstring trimEndW(const std::wstring& subject) {
        std::wstring result(subject);
        std::wstring delimiters = L"\t\n\v\f\r ";
        result.erase(result.find_last_not_of(delimiters) + 1);
        return result;
      }

      /**
       * Splits the given string into a vector of strings.
       *
       * @code
       * var input = "Hello|World|How|Are|You"
       * var words = input.split("|")
       * words[1]
       * // => "World"
       */
      std::vector<std::wstring> splitW(const std::wstring& subject, const std::wstring& token) {
        std::wstring str(subject);
        std::vector<std::wstring> result;
        while (str.size()) {
          size_t index = str.find(token);
          if (index != std::wstring::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0) {
              result.push_back(str);
            }
          } else {
            result.push_back(str);
            str = std::wstring();
          }
        }
        return result;
      }

      /**
       * Convert the given string to lowercase letters.
       */
      std::wstring toLowerCaseW(const std::wstring& subject) {
        std::wstring result(subject);
        std::transform(result.begin(), result.end(), result.begin(), [](wchar_t c) {
          return std::tolower(c);
        });
        return result;
      }

      /**
       * Convert the given string to uppercase letters.
       */
      std::wstring toUpperCaseW(const std::wstring& subject) {
        std::wstring result(subject);
        std::transform(result.begin(), result.end(), result.begin(), [](wchar_t c) {
          return std::toupper(c);
        });
        return result;
      }

      /**
       * Checks if a string includes the given string.
       *
       * @see includesChar
       */
      bool includesW(const std::wstring& subject, const std::wstring& search) {
        return subject.find(search) != std::wstring::npos;
      }

      /**
       * Checks if a string includes the given character.
       *
       * @see includes
       */
      bool includesWchar(const std::wstring& subject, wchar_t search) {
        return subject.find(search) != std::wstring::npos;
      }

      /**
       * Adds the String Methods to the given ChaiScript module.
       */
      ModulePtr bootstrap(ModulePtr m = std::make_shared<Module>())
      {
        m->add(fun(replaceWstring), "replacew");
        m->add(fun(replaceWchar), "replacew");
        m->add(fun(trimW), "trimw");
        m->add(fun(splitW), "splitw");
        m->add(fun(toLowerCaseW), "toLowerCasew");
        m->add(fun(toUpperCaseW), "toUpperCasew");
        m->add(fun(includesW), "includesw");
        m->add(fun(includesWchar), "includesw");
        m->add(fun(trimStartW), "trimStartw");
        m->add(fun(trimEndW), "trimEndw");

        return m;
      }
    }
  }
}

#endif /* CHAISCRIPT_EXTRAS_STRING_METHODS_HPP_ */