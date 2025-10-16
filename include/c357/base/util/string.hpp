#ifndef C357_BASE_UTIL_STRING_HPP
#define C357_BASE_UTIL_STRING_HPP

#include <string>
#include <string_view>
#include <vector>

namespace c357::base::util {

std::vector<std::string> split(
    const std::string &input,
    const std::string &delimiter);

std::vector<std::string_view> split(
    const std::string_view &input,
    const std::string &delimiter);

}

#endif /* C357_BASE_UTIL_STRING_HPP */
