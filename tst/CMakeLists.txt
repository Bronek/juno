cmake_minimum_required(VERSION 3.6)
project(tst)

set(SOURCE_FILES test.cpp)

find_package(GTest REQUIRED)

add_subdirectory(../lib lib)
include_directories(${LIB_INCLUDE})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib)
target_link_libraries(${PROJECT_NAME} ${GTEST_BOTH_LIBRARIES})
