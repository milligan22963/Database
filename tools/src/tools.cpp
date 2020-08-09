/**
 * tools.cpp
 */

#include <algorithm>
#include <cctype>
#include <sstream>

#include "tools/tools.h"

namespace afm {
    namespace tools {

        std::size_t split_string(StringTokens &tokens, const std::string &source, char target, bool include_empty)
        {
            bool done = false;
            const char *pStart = source.c_str();

            tokens.clear();

		    while (done == false)
		    {
                const char *pEnd = pStart;

		        while ((*pEnd != target) && (*pEnd != 0)) {
		            pEnd++;
                }

                // By default empty tokens are ignored
                std::string token = trim_string(std::string(pStart, pEnd));
                if ((token.length() > 0) || (include_empty == true)) {
    		        tokens.push_back(token);
                }

                if (*pEnd != 0) {
                    pStart = pEnd + 1;
                    if (*pStart == 0) {
                        done = true;
                    }
                } else {
                    done = true;
                }
		    }

            return tokens.size();
        }

        std::size_t split_string(StringTokens &tokens, const std::string &source, const std::string &target, bool include_empty)
        {
            tokens.clear();

            return tokens.size();
        }

        std::string trim_string(const std::string &target)
        {
            return ltrim_string(rtrim_string(target));
        }

        std::string ltrim_string(const std::string &target)
        {
            std::string temp(target);

			temp.erase(temp.begin(), std::find_if(temp.begin(), temp.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));

			return temp;
        }

        std::string rtrim_string(const std::string &target)
        {
            std::string temp(target);

			temp.erase(std::find_if(temp.rbegin(), temp.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), temp.end());

			return temp;
        }

        std::string to_upper(const std::string &source)
        {
            std::stringstream upper;

            for (auto character : source) {
                upper << static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
            }

            return upper.str();
        }

        std::string to_lower(const std::string &source)
        {
            std::stringstream lower;

            for (auto character : source) {
                lower << static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
            }

            return lower.str();
        }
    }
}