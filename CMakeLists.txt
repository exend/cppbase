cmake_minimum_required(VERSION 3.24)
project(base LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} STATIC
	src/concurrent/thread_pool.cpp
	src/util/mime.cpp
	src/util/string.cpp
	src/util/url.cpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# TESTS
option(BASE_BUILD_TESTS "Build tests for base" ON)

if(BASE_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
