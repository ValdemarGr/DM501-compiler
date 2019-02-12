cmake_minimum_required(VERSION 2.8)

project(Hello)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
add_test(NAME AllTests COMMAND tests)
