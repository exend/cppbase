#ifndef C357_BASE_CONCURRENT_EXECUTOR_HPP
#define C357_BASE_CONCURRENT_EXECUTOR_HPP

#include <functional>

namespace c357::base::concurrent {

struct executor {
	virtual ~executor() = default;
	virtual void execute(const std::function<void()> &) = 0;
};

typedef std::shared_ptr<executor> executor_sptr;

}

#endif /* C357_BASE_CONCURRENT_EXECUTOR_HPP */
