#ifndef C357_BASE_CONCURRENT_CANCELLATION_STATE_HPP
#define C357_BASE_CONCURRENT_CANCELLATION_STATE_HPP

#include <atomic>

namespace c357::base::concurrent {

struct cancellation_state final {
	std::atomic_bool cancelled = false;
};

}

#endif /* C357_BASE_CONCURRENT_CANCELLATION_STATE_HPP */
