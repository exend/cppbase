#include <c357/base/util/url.hpp>
#include <c357/base/util/string.hpp>
#include <cctype>
#include <charconv>

using namespace std;
using namespace c357::base::util;

const uint16_t url::noport = 0;
const string url::authority_prefix = "//";

url::url(const std::string &str)
    : srcstr(str)
{ }

url &url::operator=(const string &str)
{
	srcstr = str;
	return *this;
}

bool url::is_relative() const noexcept
{
	static const char relative_prefixes[] = "/~.?#";
	for (char p : relative_prefixes)
		if (srcstr.starts_with(p))
			return true;
	return scheme_suffix_pos() == srcstr.npos;
}

string_view url::scheme() const noexcept
{
	if (is_relative())
		return empty_view();
	size_t pos = srcstr.find(':');
	return string_view(srcstr.data(), pos);
}

url &url::set_scheme(const string &scheme)
{
	string_view view = this->scheme();
	if (view.empty())
		srcstr.insert(0, scheme + ':');
	else
		replace(view, scheme);
	return *this;
}

string_view url::authority() const noexcept
{
	size_t begpos = srcstr.find(authority_prefix);
	if (begpos == srcstr.npos || (begpos && (srcstr[begpos - 1] != ':')))
		return empty_view();
	begpos += authority_prefix.length();
	size_t endpos = srcstr.find_first_of("/?#", begpos);
	if (endpos == srcstr.npos)
		endpos = srcstr.length();
	return string_view(srcstr.data() + begpos, endpos - begpos);
}

string_view url::userinfo() const noexcept
{
	string_view auth = this->authority();
	size_t endpos = auth.find('@');
	if (endpos == auth.npos)
		return {};
	return string_view(auth.data(), endpos);
}

string_view url::user() const noexcept
{
	string_view usrinf = this->userinfo();
	size_t endpos = usrinf.find(':');
	if (endpos == usrinf.npos)
		return usrinf;
	return string_view(usrinf.data(), endpos);
}

string_view url::host() const noexcept
{
	string_view auth = this->authority();
	if (!auth.size())
		return auth;
	size_t begpos = auth.find('@');
	size_t endpos;
	if (begpos++ == auth.npos)
		begpos = 0;
	if (auth.at(begpos) != '[')
		endpos = auth.rfind(':');
	else if (auth.back() == ']')
		endpos = auth.length();
	else
		for (size_t i = auth.length() - 1; i; --i)
			if (!isdigit(auth.at(i))) {
				endpos = i;
				break;
			}
	if (endpos == auth.npos || endpos <= begpos)
		endpos = auth.length();
	return string_view(auth.data() + begpos, endpos - begpos);
}

uint16_t url::port() const noexcept
{
	string_view auth = this->authority();
	if (auth.empty())
		return noport;
	size_t pos = auth.length() - 1;
	while (pos > 0 && isdigit(auth[pos]))
		--pos;
	if (auth[pos++] != ':')
		return noport;
	const char *begptr = auth.data() + pos;
	const char *endptr = auth.data() + auth.length();
	uint16_t port;
	if (from_chars(begptr, endptr, port).ec != errc())
		return noport;
	return port;
}

string_view url::relative_url() const noexcept
{
	auto pptr = srcstr.data();
	size_t pos = scheme_suffix_pos();
	pptr += pos != srcstr.npos ? pos + 1 : 0;
	auto *eptr = srcstr.data() + srcstr.length();
	if (pptr + 1 < eptr) {
		size_t authprfxsize = authority_prefix.size();
		if (!memcmp(pptr, authority_prefix.data(), authprfxsize))
			pptr += authprfxsize + authority().size();
	}
	return string_view(pptr, eptr);
}

string_view url::path() const noexcept
{
	string_view reluri = relative_url();
	size_t endpos = reluri.find_first_of("?#");
	if (endpos == reluri.npos)
		return reluri;
	return string_view(reluri.data(), endpos);
}

url &url::set_path(const string &path)
{
	string_view path_view = this->path();
	size_t pos = path_view.data() - srcstr.data();
	if (authority().empty())
		srcstr.replace(pos, path_view.size(), path);
	else
		srcstr.replace(pos, path_view.size(), '/' + path);
	return *this;
}

url &url::append_path(const string &path)
{
	size_t endpos = srcstr.find_first_of("?#");
	if (endpos == srcstr.npos)
		endpos = srcstr.length();
	if (!endpos)
		srcstr.insert(0, path);
	else {
		string_view suburi(srcstr.data(), endpos);
		if (suburi.back() == ':')
			srcstr.insert(endpos, path);
		else if (suburi == "//" || suburi.ends_with("://") || suburi.back() != '/')
			srcstr.insert(endpos, path.front() == '/' ? path : '/' + path);
		else if (path.front() == '/')
			srcstr.replace(endpos - 1, 1, path);
		else
			srcstr.insert(endpos, path);
	}
	return *this;
}

string_view url::last_path_component() const noexcept
{
	string_view path = this->path();
	if (path.empty())
		return path;
	size_t backpos = path.length() - (path.back() == '/' ? 2 : 1);
	size_t pos = path.rfind('/', backpos);
	if (pos++ == path.npos)
		pos = 0;
	if (pos >= ++backpos)
		return empty_view();
	return string_view(path.data() + pos, backpos - pos);
}

string_view url::path_extension() const noexcept
{
	string_view comp = this->last_path_component();
	if (comp.empty())
		return comp;
	size_t pos = comp.rfind('.');
	if (pos++ == comp.npos)
		return {};
	return string_view(comp.data() + pos, comp.length() - pos);
}

vector<string_view> url::path_components() const
{
	string_view path = this->path();
	if (!path.empty())
		return split(path, "/");
	return vector<string_view>();
}

string_view url::query() const noexcept
{
	size_t begpos = srcstr.find('?');
	if (begpos++ != srcstr.npos) {
		size_t endpos = srcstr.find('#', begpos);
		if (endpos == srcstr.npos)
			endpos = srcstr.length();
		return string_view(srcstr.data() + begpos, endpos - begpos);
	}
	return {};
}

vector<url::query_item> url::query_items() const
{
	string_view query = this->query();
	if (query.empty())
		return {};
	vector<url::query_item> items;
	for (const string_view &item : split(query, "&")) {
		vector<string_view> kv = split(item, "=");
		if (kv.size() == 2)
			items.push_back({ kv.front(), kv.back() });
	}
	return items;
}

string_view url::fragment() const noexcept
{
	size_t pos = srcstr.rfind('#');
	if (pos++ != srcstr.npos)
		return string_view(srcstr.data() + pos, srcstr.length() - pos);
	return {};
}

bool url::is_dir() const noexcept
{
	auto ph = path();
	return ph.back() == '/' && ph.length() > 1;
}

const string &url::abs_string() const noexcept
{
	return srcstr;
}

string_view url::empty_view() const noexcept
{
	return string_view(srcstr.data() + srcstr.length(), 0);
}

size_t url::scheme_suffix_pos() const noexcept
{
	auto it = ranges::find_if_not(srcstr, [](unsigned char c) {
		return isalnum(c) || "+.-"s.find_first_of(c) != string::npos;
	});
	if (it == srcstr.end() || *it != ':')
		return srcstr.npos;
	return distance(srcstr.begin(), it);
}

void url::replace(const string_view &view, const string &str)
{
	srcstr.replace(view.data() - srcstr.data(), view.size(), str);
}
