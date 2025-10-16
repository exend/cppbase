include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

add_executable(base_tests
	test_thread_pool.cpp
	test_url.cpp
)

target_link_libraries(base_tests PRIVATE
	base
	GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(base_tests)
