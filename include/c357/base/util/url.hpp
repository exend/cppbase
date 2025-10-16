#ifndef C357_BASE_UTIL_URL_HPP
#define C357_BASE_UTIL_URL_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace c357::base::util {

class url {
public:
	struct query_item final {
		std::string_view key;
		std::string_view value;
	};
	
	static const std::uint16_t noport;
	url() = default;
	url(const std::string &str);
	url(const url &other) = default;
	url(url &&other) = default;
	virtual ~url() = default;
	url &operator=(const url &other) = default;
	url &operator=(url &&other) = default;
	url &operator=(const std::string &str);
	auto operator<=>(const url &) const noexcept = default;
	virtual bool is_relative() const noexcept;
	virtual std::string_view scheme() const noexcept;
	virtual url &set_scheme(const std::string &scheme);
	virtual std::string_view authority() const noexcept;
	virtual std::string_view userinfo() const noexcept;
	virtual std::string_view user() const noexcept;
	virtual std::string_view host() const noexcept;
	virtual std::uint16_t port() const noexcept;
	virtual std::string_view relative_url() const noexcept;
	virtual std::string_view path() const noexcept;
	virtual url &set_path(const std::string &scheme);
	virtual url &append_path(const std::string &scheme);
	virtual std::string_view last_path_component() const noexcept;
	virtual std::string_view path_extension() const noexcept;
	virtual std::vector<std::string_view> path_components() const;
	virtual std::string_view query() const noexcept;
	virtual std::vector<query_item> query_items() const;
	virtual std::string_view fragment() const noexcept;
	virtual bool is_dir() const noexcept;
	virtual const std::string &abs_string() const noexcept;

private:
	static const std::string authority_prefix;
	std::string srcstr;
	std::string_view empty_view() const noexcept;
	size_t scheme_suffix_pos() const noexcept;
	void replace(const std::string_view &view, const std::string &str);
};

}

#endif /* C357_BASE_UTIL_URL_HPP */
