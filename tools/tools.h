/**
 * tools.h
 */

#ifndef _H_TOOLS
#define _H_TOOLS

#include <string>
#include <vector>

namespace afm {
    namespace tools {
        using StringTokens = std::vector<std::string>;

        std::size_t split_string(StringTokens &tokens, const std::string &source, char target, bool include_empty = false);
        std::size_t split_string(StringTokens &tokens, const std::string &source, const std::string &target, bool include_empty = false);

        std::string trim_string(const std::string &target);
        std::string ltrim_string(const std::string &target);
        std::string rtrim_string(const std::string &target);
        std::string to_upper(const std::string &source);
        std::string to_lower(const std::string &source);
    }
}
#endif