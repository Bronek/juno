cmake_minimum_required(VERSION 3.8)
project(juno)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wpedantic")

add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(test)
