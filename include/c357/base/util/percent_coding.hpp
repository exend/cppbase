#ifndef C357_BASE_UTIL_PERCENT_CODING_HPP
#define C357_BASE_UTIL_PERCENT_CODING_HPP

#include <string>

namespace c357::base::util {

std::string percent_encode(const std::string &in, const std::string_view &safe);
std::string percent_encode(const std::string &in);
std::string percent_decode(const std::string &in);

}

#endif /* C357_BASE_UTIL_PERCENT_CODING_HPP */
