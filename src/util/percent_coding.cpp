#include <c357/base/util/percent_coding.hpp>
#include <array>
#include <cstdint>

using namespace std;
using namespace c357::base;

constexpr array<uint8_t, 0x100> hex_lookup = [] {
	array<uint8_t, 0x100> table {};
	for (uint8_t c = u8'0'; c <= u8'9'; ++c)
		table[c] = c - u8'0';
	for (uint8_t c = u8'A'; c <= u8'F'; ++c)
		table[c] = c - u8'A' + 0xA;
	for (uint8_t c = u8'a'; c <= u8'f'; ++c)
		table[c] = c - u8'a' + 0xA;
	return table;
}();

constexpr array<uint8_t, 0x10> hex_chars = [] {
	array<uint8_t, 0x10> table {};
	for (uint8_t i = 0; i <= 9; ++i)
		table[i] = i + '0';
	for (uint8_t i = 0xA; i <= 0xF; ++i)
		table[i] = i - 0xA + 'A';
	return table;
}();

constexpr array<bool, 0x100> unreserved_table = [] {
	array<bool, 0x100> table {};
	for (uint8_t c = u8'0'; c <= u8'9'; ++c)
		table[c] = true;
	for (uint8_t c = u8'A'; c <= u8'Z'; ++c)
		table[c] = true;
	for (uint8_t c = u8'a'; c <= u8'z'; ++c)
		table[c] = true;
	table['-'] = true;
	table['_'] = true;
	table['~'] = true;
	table['.'] = true;
	return table;
}();

constexpr bool is_unreserved(uint8_t c) noexcept
{
	return unreserved_table[c];
}

constexpr uint8_t decode_hex_pair(uint8_t first, uint8_t second) noexcept
{
	first = hex_lookup[first];
	second = hex_lookup[second];
	return first << 4 | second;
}

template<typename T>
static string percent_encode(const T &in, const std::string_view &safe)
{
	string out;
	out.reserve(in.size() * 3);
	for (uint8_t c : in) {
		if (is_unreserved(c) || safe.find(c) != safe.npos) {
			out.push_back(c);
			continue;
		}
		out.push_back('%');
		out.push_back(hex_chars[c >> 4]);
		out.push_back(hex_chars[c & 0x0F]);
	}
	return out;
}

string util::percent_encode(const std::string &in, const std::string_view &safe)
{
	return ::percent_encode(in, safe);
}

string util::percent_encode(const std::string_view &in, const std::string_view &safe)
{
	return ::percent_encode(in, safe);
}

string util::percent_encode(const string &in)
{
	return ::percent_encode(in, "");
}

template <typename T>
static string percent_decode(const T &in)
{
	string out;
	out.reserve(in.size());
	for (size_t i = 0; i < in.size(); ++i) {
		if (in[i] == '%' && i + 2 < in.size()) {
			uint8_t outch = decode_hex_pair(in[i + 1], in[i + 2]);
			if (outch) {
				out.push_back(outch);
				i += 2;
				continue;
			}
		}
		out.push_back(in[i]);
	}
	return out;
}

string util::percent_decode(const string &in)
{
	return ::percent_decode(in);
}

string util::percent_decode(const string_view &in)
{
	return ::percent_decode(in);
}
