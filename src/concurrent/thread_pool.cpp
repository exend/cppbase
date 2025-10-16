#include <c357/base/concurrent/thread_pool.hpp>

using namespace std;
using namespace c357::base::concurrent;

thread_pool::thread_pool()
	: thread_pool(thread::hardware_concurrency())
{ }

thread_pool::thread_pool(uint8_t thread_count)
{
	stop = false;
	for (uint8_t i = 0; i < thread_count; ++i)
		threads.emplace_back(&thread_pool::thread_loop, this);
}

thread_pool::~thread_pool()
{
	stop = true;
	cv.notify_all();
	for (auto &thread : threads)
		thread.join();
}

void thread_pool::execute(const std::function<void()> &f)
{
	unique_lock lock(mtx);
	tasks.emplace(f);
	lock.unlock();
	cv.notify_one();
}

void thread_pool::thread_loop()
{
	while (42) {
		function<void()> task;
		{
			unique_lock lock(mtx);
			cv.wait(lock, [this] {
				return stop || !tasks.empty();
			});
			if (stop && tasks.empty())
				return;
			task = std::move(tasks.front());
			tasks.pop();
		}
		try {
			task();
		} catch (...) { }
	}
}
