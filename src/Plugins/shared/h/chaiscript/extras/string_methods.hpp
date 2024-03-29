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
 *     chai.add(chaiscript::bootstrap::standard_library::vector_type<std::vector<std::string>>("VectorString"));
 */

#ifndef CHAISCRIPT_EXTRAS_STRING_METHODS_HPP_
#define CHAISCRIPT_EXTRAS_STRING_METHODS_HPP_

#include <algorithm>
#include <string>
#include <vector>
#include "../chaiscript.hpp"

namespace chaiscript {
  namespace extras {
    namespace string_methods {

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
      std::string replaceString(const std::string& subject, const std::string& search, const std::string& replace) {
        std::string result(subject);
        size_t pos = 0;
        while ((pos = result.find(search, pos)) != std::string::npos) {
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
      std::string replaceChar(const std::string& subject, char search, char replace) {
        std::string result(subject);
        std::replace(result.begin(), result.end(), search, replace);
        return result;
      }

      /**
       * Trims the given string.
       */
      std::string trim(const std::string& subject) {
        std::string result(subject);
        std::string delimiters = "\t\n\v\f\r ";
        result.erase(0, result.find_first_not_of(delimiters));
        result.erase(0, result.find_last_not_of(delimiters));
        return result;
      }

      /**
       * Trims the beginning of the given string.
       */
      std::string trimStart(const std::string& subject) {
        std::string result(subject);
        std::string delimiters = "\t\n\v\f\r ";
        result.erase(0, result.find_first_not_of(delimiters));
        return result;
      }

      /**
       * Trims the end of the given string.
       */
      std::string trimEnd(const std::string& subject) {
        std::string result(subject);
        std::string delimiters = "\t\n\v\f\r ";
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
      std::vector<std::string> split(const std::string& subject, const std::string& token) {
        std::string str(subject);
        std::vector<std::string> result;
        while (str.size()) {
          size_t index = str.find(token);
          if (index != std::string::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0) {
              result.push_back(str);
            }
          } else {
            result.push_back(str);
            str = "";
          }
        }
        return result;
      }

      /**
       * Convert the given string to lowercase letters.
       */
      std::string toLowerCase(const std::string& subject) {
        std::string result(subject);
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
          return std::tolower(c);
        });
        return result;
      }

      /**
       * Convert the given string to uppercase letters.
       */
      std::string toUpperCase(const std::string& subject) {
        std::string result(subject);
        std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
          return std::toupper(c);
        });
        return result;
      }

      /**
       * Checks if a string includes the given string.
       *
       * @see includesChar
       */
      bool includes(const std::string& subject, const std::string& search) {
        return subject.find(search) != std::string::npos;
      }

      /**
       * Checks if a string includes the given character.
       *
       * @see includes
       */
      bool includesChar(const std::string& subject, char search) {
        return subject.find(search) != std::string::npos;
      }

      /**
       * Adds the String Methods to the given ChaiScript module.
       */
      ModulePtr bootstrap(ModulePtr m = std::make_shared<Module>())
      {
        m->add(fun(replaceString), "replace");
        m->add(fun(replaceChar), "replace");
        m->add(fun(trim), "trim");
        m->add(fun(split), "split");
        m->add(fun(toLowerCase), "toLowerCase");
        m->add(fun(toUpperCase), "toUpperCase");
        m->add(fun(includes), "includes");
        m->add(fun(includesChar), "includes");
        m->add(fun(trimStart), "trimStart");
        m->add(fun(trimEnd), "trimEnd");

        return m;
      }
    }
  }
}

#endif /* CHAISCRIPT_EXTRAS_STRING_METHODS_HPP_ */