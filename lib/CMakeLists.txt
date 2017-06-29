cmake_minimum_required(VERSION 3.6)
project(lib)

set(LIB_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

if (TARGET ${PROJECT_NAME})
    message("")
else()
    set(SOURCE_FILES
            mpl.hpp mpl.cpp)

   add_library(${PROJECT_NAME} ${SOURCE_FILES})
endif()
