cmake_minimum_required(VERSION 3.6)
project(juno)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wpedantic")

add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(tst)
