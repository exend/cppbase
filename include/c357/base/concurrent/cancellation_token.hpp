#ifndef C357_BASE_CONCURRENT_CANCELLATION_TOKEN_HPP
#define C357_BASE_CONCURRENT_CANCELLATION_TOKEN_HPP

#include <c357/base/concurrent/cancellation_state.hpp>
#include <memory>

namespace c357::base::concurrent {

struct cancellation_token final {
	cancellation_token(const std::shared_ptr<cancellation_state> &) noexcept;
	bool is_cancellation_requested() const noexcept;

private:
	std::shared_ptr<cancellation_state> state_ptr;
};

inline cancellation_token::cancellation_token(
	const std::shared_ptr<cancellation_state> &state_ptr) noexcept
    : state_ptr(state_ptr)
{ }

inline bool cancellation_token::is_cancellation_requested() const noexcept
{
	return !state_ptr || state_ptr->cancelled;
}

}

#endif /* C357_BASE_CONCURRENT_CANCELLATION_TOKEN_HPP */
