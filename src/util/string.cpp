#include <c357/base/util/string.hpp>

using namespace std;
using namespace c357::base;

template <typename T>
static vector<T> split(const T &input, const string &delimiter)
{
	vector<T> tokens;
	size_t beg = 0;
	size_t end = input.find(delimiter);
	while (end != string::npos) {
		if (end)
			tokens.push_back(input.substr(beg, end - beg));
		beg = end + delimiter.length();
		end = input.find(delimiter, beg);
	}
	if (beg != input.length())
		tokens.push_back(input.substr(beg));
	return tokens;
}

vector<string> util::split(
    const string &input,
    const string &delimiter)
{
	return ::split<string>(input, delimiter);
}

vector<string_view> util::split(
    const string_view &input,
    const string &delimiter)
{
	return ::split<string_view>(input, delimiter);
}
