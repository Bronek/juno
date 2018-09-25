cmake_minimum_required(VERSION 3.8)
project(juno)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
  add_compile_options("/permissive-")
  add_compile_options("$<$<CONFIG:RELEASE>:/DNDEBUG>")
  add_compile_options("$<$<CONFIG:DEBUG>:/D_DEBUG>")
else()
  add_compile_options("-march=native" "-Wall" "-Wextra" "-Wpedantic")
  add_compile_options("$<$<CONFIG:DEBUG>:-O0>" "$<$<CONFIG:DEBUG>:-g>" "$<$<CONFIG:DEBUG>:-D_DEBUG>")
  add_compile_options("$<$<CONFIG:RELEASE>:-O3>" "$<$<CONFIG:RELEASE>:-DNDEBUG>")
endif()

set(SOURCE_FILES main.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
