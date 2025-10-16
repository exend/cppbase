#include <c357/base/util/url.hpp>
#include <gtest/gtest.h>

using namespace std;
using namespace testing;
using namespace c357::base::util;

using url_params = tuple<
    bool,                       // [0]  is_relative
    string,                     // [1]  scheme
    bool,                       // [2]  has_authority
    string,                     // [3]  user
    string,                     // [4]  password
    string,                     // [5]  host
    uint16_t,                   // [6]  port
    bool,                       // [7]  path_begins_with_slash
    vector<string>,             // [8]  path_components
    string,                     // [9]  path_extension
    vector<url::query_item>,    // [10] query_items
    string,                     // [11] fragment
    bool,                       // [12] is_dir
	string						// [13] path_to_append
>;

struct url_case {
	bool expect_relative;
	string expect_scheme;
	string expect_authority;
	string expect_userinfo;
	string expect_user;
	string expect_host;
	uint16_t expect_port = 0;
	string expect_relative_uri;
	string expect_path;
	string expect_last_path_component;
	string expect_extension;
	vector<string> expect_path_components;
	string expect_query;
	vector<url::query_item> expect_query_items;
	string expect_fragment;
	bool expect_dir;
	string expect_abs_url;
	string expect_abs_url_after_append;
	string path_to_append;
};

// 🥹
url_case map_param(url_params params)
{
	url_case uc;
    string scheme_with_auth;
	bool has_auth = get<2>(params);
	uc.expect_relative = get<0>(params);
	if (!uc.expect_relative) {
		uc.expect_scheme = get<1>(params);
		scheme_with_auth = uc.expect_scheme + ':';
		if (has_auth) {
			string authority;
			string host = get<5>(params);
			if (!host.empty()) {
				string user = get<3>(params);
				uint16_t port = get<6>(params);
				if (!user.empty()) {
					string userinfo = user;
					string password = get<4>(params);
					if (!password.empty())
						userinfo += ':' + password;
					authority += userinfo + '@';
					uc.expect_user = user;
					uc.expect_userinfo = userinfo;
				}
				authority += host;
				if (port != url::noport)
					authority += ':' + to_string(port);
				uc.expect_host = host;
				uc.expect_port = port;
				uc.expect_authority = authority;
			}
			scheme_with_auth += "//" + authority;
		}
	}
    string path;
    bool path_begins_with_slash = get<7>(params);
	if (has_auth || path_begins_with_slash)
		path = '/';
	vector<string> path_comps = get<8>(params);
	if (!path_comps.empty()) {
		for (auto &comp : path_comps) {
			path += comp;
			if (comp != path_comps.back())
                path += '/';
		}
		uc.expect_path_components = path_comps;
		uc.expect_last_path_component = path_comps.back();
	}
	string ext = get<9>(params);
	if (!path_comps.empty() && !ext.empty()) {
		path += '.' + ext;
		uc.expect_extension = ext;
		uc.expect_last_path_component += '.' + ext;
		uc.expect_path_components.back() += '.' + ext;
	}
	bool is_dir = get<12>(params);
	if (is_dir && (has_auth && !uc.expect_relative || path != "/")) {
		path += '/';
		if ((has_auth || path_begins_with_slash) && path_comps.empty())
			uc.expect_path_components = { "" };
	}
	string path_to_append = get<13>(params);
	size_t insert_path_pos = scheme_with_auth.length() + path.length();
	size_t replace_count = 0;
	if (!path_to_append.empty() && !path.empty()) {
		if (path_to_append.front() == '/' && path.back() == '/') {
			--insert_path_pos;
			++replace_count;
		} else if (path_to_append.front() != '/' && path.back() != '/') {
			path_to_append.insert(0, 1, '/'); 
		}
	}
	uc.expect_dir = is_dir && path.length() > 1;
	uc.expect_path = path;
	uc.expect_relative_uri = path;
	string query;
	vector<url::query_item> query_items = get<10>(params);
	if (!query_items.empty()) {
		for (auto &[key, val] : query_items) {
			query += string(key) + '=' + string(val);
			if (key != query_items.back().key)
				query += '&';
		}
        uc.expect_query = query;
		uc.expect_query_items = query_items;
		uc.expect_relative_uri += '?' + query;
	}
	string frag = get<11>(params);
	if (!frag.empty()) {
		uc.expect_fragment = frag;
		uc.expect_relative_uri += '#' + uc.expect_fragment;
	}
	uc.expect_abs_url = scheme_with_auth + uc.expect_relative_uri;
	if (!path_to_append.empty()) {
		uc.path_to_append = path_to_append;
		uc.expect_abs_url_after_append = uc.expect_abs_url;
		uc.expect_abs_url_after_append.replace(insert_path_pos, replace_count, path_to_append);
	}
	return uc;
}

class url_cmb_test : public TestWithParam<url_params>
{ };

TEST_P(url_cmb_test, variants)
{
	auto param = GetParam();
	url_case uc = map_param(param);
	url test(uc.expect_abs_url);
	EXPECT_EQ(test.is_relative(), uc.expect_relative);
	EXPECT_EQ(test.scheme(), uc.expect_scheme);
	EXPECT_EQ(test.authority(), uc.expect_authority);
	EXPECT_EQ(test.userinfo(), uc.expect_userinfo);
	EXPECT_EQ(test.user(), uc.expect_user);
	EXPECT_EQ(test.host(), uc.expect_host);
	EXPECT_EQ(test.port(), uc.expect_port);
	EXPECT_EQ(test.relative_url(), uc.expect_relative_uri);
	EXPECT_EQ(test.path(), uc.expect_path);
	EXPECT_EQ(test.last_path_component(), uc.expect_last_path_component);
	EXPECT_EQ(test.path_extension(), uc.expect_extension);
	auto test_comps = test.path_components();
	auto expect_comps = uc.expect_path_components;
	int min_comp_size = min(test_comps.size(), expect_comps.size());
	EXPECT_EQ(test_comps.size(), expect_comps.size());
	for (int i = 0; i < min_comp_size; ++i)
		EXPECT_EQ(test_comps[i], expect_comps[i]);
	EXPECT_EQ(test.query(), uc.expect_query);
	auto test_queries = test.query_items();
	auto expect_queries = uc.expect_query_items;
	int min_queries_size = min(test_queries.size(), expect_queries.size());
	EXPECT_EQ(test_queries.size(), expect_queries.size());
	for (int i = 0; i < min_queries_size; ++i) {
		EXPECT_EQ(test_queries[i].key, expect_queries[i].key);
		EXPECT_EQ(test_queries[i].value, expect_queries[i].value);
	}
	EXPECT_EQ(test.fragment(), uc.expect_fragment);
	EXPECT_EQ(test.is_dir(), uc.expect_dir);

	if (!uc.path_to_append.empty()) {
		test.append_path(uc.path_to_append);
		EXPECT_EQ(test.abs_string(), uc.expect_abs_url_after_append);
	}
}

INSTANTIATE_TEST_SUITE_P(
    append_path, url_cmb_test,
    Combine(
        Bool(), // [0]  is_relative
        Values("http"), // [1]  scheme
        Bool(), // [2]  has_authority
        Values(""), // [3]  user
        Values(""), // [4]  password
        Values("www.l-o_c~a+l!(h$o&s't*;foo=4+2).net"), // [5]  host
        Values(url::noport, uint16_t(8080)), // [6]  port
        Bool(), // [7]  path_begins_with_slash
        Values(
            vector<string>(),
            vector<string> { "smth" },
            vector<string> { "path", "to", "smth" }), // [8]  path_components
        Values("", "png"), // [9]  path_extension
        Values(
            vector<url::query_item>(),
            vector<url::query_item> {
                { "key1", "val1" },
                { "key2", "val2" } }), // [10] query_items
        Values("", "~:frag=42"), // [11] fragment
        Bool(), // [12] is_dir
        Values("/append/path", "append/path") // [13] path_to_append
        ));

INSTANTIATE_TEST_SUITE_P(
    relative, url_cmb_test,
    Combine(
        Values(true), // [0]  is_relative
        Values(""), // [1]  scheme
        Values(false), // [2]  has_authority
        Values(""), // [3]  user
        Values(""), // [4]  password
        Values(""), // [5]  host
        Values(url::noport), // [6]  port
        Bool(), // [7]  path_begins_with_slash
        Values(
            vector<string>(),
            vector<string> { "smth" },
            vector<string> { ".", "smth" },
            vector<string> { "~", "smth" },
            vector<string> { "path", "to", "smth" },
            vector<string> { ".", "path", "to", "smth" },
            vector<string> { "~", "path", "to", "smth" }), // [8]  path_components
        Values("", "png"), // [9]  path_extension
        Values(
            vector<url::query_item>(),
            vector<url::query_item> {
                { "key1", "val1" },
                { "key2", "val2" } }), // [10] query_items
        Values("", "~:frag=42"), // [11] fragment
        Bool(), // [12] is_dir
        Values("") // [13] path_to_append
        ));

INSTANTIATE_TEST_SUITE_P(
    http_without_auth, url_cmb_test,
    Combine(
        Values(false), // [0]  is_relative
        Values("http"), // [1]  scheme
        Values(false), // [2]  has_authority
        Values(""), // [3]  user
        Values(""), // [4]  password
        Values(""), // [5]  host
        Values(url::noport), // [6]  port
        Bool(), // [7]  path_begins_with_slash
        Values(
            vector<string>(),
            vector<string> { "smth" },
            vector<string> { "path", "to", "smth" }), // [8]  path_components
        Values("", "png"), // [9]  path_extension
        Values(
            vector<url::query_item>(),
            vector<url::query_item> {
                { "key1", "val1" },
                { "key2", "val2" } }), // [10] query_items
        Values("", "~:frag=42"), // [11] fragment
        Bool(), // [12] is_dir
        Values("") // [13] path_to_append
        ));

INSTANTIATE_TEST_SUITE_P(
    http_with_empty_auth, url_cmb_test,
    Combine(
        Values(false), // [0]  is_relative
        Values("http"), // [1]  scheme
        Values(true), // [2]  has_authority
        Values(""), // [3]  user
        Values(""), // [4]  password
        Values(""), // [5]  host
        Values(url::noport), // [6]  port
        Bool(), // [7]  path_begins_with_slash
        Values(
            vector<string>(),
            vector<string> { "smth" },
            vector<string> { "path", "to", "smth" }), // [8]  path_components
        Values("", "png"), // [9]  path_extension
        Values(
            vector<url::query_item>(),
            vector<url::query_item> {
                { "key1", "val1" },
                { "key2", "val2" } }), // [10] query_items
        Values("", "~:frag=42"), // [11] fragment
        Bool(), // [12] is_dir
        Values("") // [13] path_to_append
        ));

INSTANTIATE_TEST_SUITE_P(
    http_with_host, url_cmb_test,
    Combine(
        Values(false), // [0]  is_relative
        Values("http"), // [1]  scheme
        Values(true), // [2]  has_authority
        Values(""), // [3]  user
        Values(""), // [4]  password
        Values(
            "localhost",
            "www.l-o_c~a+l!(h$o&s't*;foo=4+2).net",
            "127.0.0.1",
            "[2001:db8::1]"), // [5]  host
        Values(url::noport, uint16_t(80), uint16_t(8080)), // [6]  port
        Bool(), // [7]  path_begins_with_slash
        Values(
            vector<string>(),
            vector<string> { "smth" },
            vector<string> { "path", "to", "smth" }), // [8]  path_components
        Values("", "png"), // [9]  path_extension
        Values(
            vector<url::query_item>(),
            vector<url::query_item> {
                { "key1", "val1" },
                { "key2", "val2" } }), // [10] query_items
        Values("", "~:frag=42"), // [11] fragment
        Bool(), // [12] is_dir
        Values("") // [13] path_to_append
        ));

INSTANTIATE_TEST_SUITE_P(
    http_with_usr, url_cmb_test,
    Combine(
        Values(false), // [0]  is_relative
        Values("http"), // [1]  scheme
        Values(true), // [2]  has_authority
        Values("usr%20-foo_bar~4.2"), // [3]  user
        Values("", "notused"), // [4]  password
        Values(
            "localhost",
            "www.l-o_c~a+l!(h$o&s't*;foo=4+2).net",
            "127.0.0.1",
            "[2001:db8::1]"), // [5]  host
        Values(url::noport, uint16_t(80), uint16_t(8080)), // [6]  port
        Bool(), // [7]  path_begins_with_slash
        Values(
            vector<string>(),
            vector<string> { "smth" },
            vector<string> { "path", "to", "smth" },
            vector<string> { "~", "path", "to", "smth" }), // [8]  path_components
        Values("", "png"), // [9]  path_extension
        Values(
            vector<url::query_item>(),
            vector<url::query_item> {
                { "key1", "val1" },
                { "key2", "val2" } }), // [10] query_items
        Values("", "~:frag=42"), // [11] fragment
        Bool(), // [12] is_dir
        Values("") // [13] path_to_append
        ));
