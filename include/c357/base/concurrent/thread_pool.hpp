#ifndef C357_BASE_CONCURRENT_THREAD_POOL_HPP
#define C357_BASE_CONCURRENT_THREAD_POOL_HPP

#include <c357/base/concurrent/executor.hpp>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <queue>

namespace c357::base::concurrent {

class thread_pool : public executor {
public:
	thread_pool();
	thread_pool(uint8_t thread_count);
	~thread_pool() override;
	void execute(const std::function<void()> &) override;

private:
	std::atomic_bool stop;
	std::mutex mtx;
	std::condition_variable cv;
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;
	void thread_loop();
};

}

#endif /* C357_BASE_CONCURRENT_THREAD_POOL_HPP */
