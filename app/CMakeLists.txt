cmake_minimum_required(VERSION 3.6)
project(app)

set(SOURCE_FILES main.cpp)

add_subdirectory(../lib lib)
include_directories(${LIB_INCLUDE})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} lib)
