#include <gtest/gtest.h>
#include <c357/base/concurrent/thread_pool.hpp>

using namespace std;
using namespace c357::base::concurrent;

struct thread_pool_test : public testing::Test {
	const uint8_t thread_count { 4 };
	thread_pool tp { thread_count };
	atomic_int counter { 0 };
	mutex mtx;
	condition_variable cv;
};

TEST_F(thread_pool_test, executes_simple_task)
{
	tp.execute([&] {
		++counter;
		cv.notify_one();
	});
	unique_lock lock(mtx);
	bool finished = cv.wait_for(lock, 1s, [&] {
		return counter > 0;
	});

	EXPECT_TRUE(finished);
	EXPECT_EQ(counter, 1);
}

TEST_F(thread_pool_test, executes_multiple_tasks)
{
	const int total = 146;

	for (int i = 0; i < total; ++i)
		tp.execute([&] {
			++counter;
			cv.notify_one();
		});
	unique_lock lock(mtx);
	bool finished = cv.wait_for(lock, 1s, [&] {
		return counter >= total;
	});

	EXPECT_TRUE(finished);
	EXPECT_EQ(counter, total);
}

TEST_F(thread_pool_test, executes_multiple_tasks_in_parallel)
{
	const int total = thread_count * 2;
	atomic_int active;
	atomic_int max_active;

	for (int i = 0; i < total; ++i)
		tp.execute([&] {
			int current = ++active;
			max_active = max(max_active.load(), current);
			this_thread::sleep_for(50ms);
			--active;
			++counter;
			cv.notify_one();
		});
	unique_lock lock(mtx);
	bool finished = cv.wait_for(lock, 1s, [&] {
		return counter >= total;
	});

	EXPECT_TRUE(finished);
	EXPECT_EQ(max_active, thread_count);
	EXPECT_EQ(counter, total);
}

TEST_F(thread_pool_test, thread_safe_task_execution)
{
	const int total = 400;
	deque<thread> producer;

	for (int i = 0; i < total / 50; ++i)
		producer.emplace_back([&] {
			for (int i = 0; i < total / 8; ++i) {
				tp.execute([&] {
					++counter;
					cv.notify_one();
				});
			}
		});
	for (auto &t : producer)
			t.join();
	unique_lock lock(mtx);
	bool finished = cv.wait_for(lock, 1s, [&] {
		return counter >= total;
	});

	EXPECT_TRUE(finished);
	EXPECT_EQ(counter, total);
}

TEST_F(thread_pool_test, destructor_waits_for_tasks)
{
	const int total = 10;
	{
		thread_pool tp(2);
		for (int i = 0; i < total; ++i)
			tp.execute([&] {
				this_thread::sleep_for(20ms);
				++counter;
			});
	}

	EXPECT_EQ(counter, total);
}

TEST_F(thread_pool_test, exceptions_in_tasks_dont_crash)
{
	thread_pool tp(1);

	tp.execute([&] {
		throw runtime_error("die!");
	});
	tp.execute([&] {
		++counter;
		cv.notify_one();
	});
	unique_lock lock(mtx);
	bool finished = cv.wait_for(lock, 1s, [&] {
		return counter > 0;
	});

	EXPECT_TRUE(finished);
	EXPECT_EQ(counter, 1);
}
