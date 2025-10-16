#ifndef C357_BASE_CONCURRENT_CANCELLATION_SOURCE_HPP
#define C357_BASE_CONCURRENT_CANCELLATION_SOURCE_HPP

#include <c357/base/concurrent/cancellation_token.hpp>

namespace c357::base::concurrent {

struct cancellation_source final {
	cancellation_source();
	cancellation_source(const cancellation_source &) = delete;
	cancellation_source &operator=(const cancellation_source &) = delete;
	cancellation_token token() const noexcept;
	void cancel() const noexcept;

private:
	std::shared_ptr<cancellation_state> state_ptr;
};

inline cancellation_source::cancellation_source()
    : state_ptr(std::make_shared<cancellation_state>())
{ }

inline cancellation_token cancellation_source::token() const noexcept
{
	return cancellation_token(state_ptr);
}

inline void cancellation_source::cancel() const noexcept
{
	state_ptr->cancelled = true;
}

}

#endif /* C357_BASE_CONCURRENT_CANCELLATION_SOURCE_HPP */
