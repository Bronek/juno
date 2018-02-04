cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0037 OLD)
project(test)

set(SOURCE_FILES set.cpp tag.cpp)

find_package(GTest REQUIRED)

add_subdirectory(../lib lib)
include_directories(${LIB_INCLUDE})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib)
target_link_libraries(${PROJECT_NAME} ${GTEST_BOTH_LIBRARIES})
