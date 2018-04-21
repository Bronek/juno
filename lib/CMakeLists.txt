cmake_minimum_required(VERSION 3.8)
project(lib)

set(LIB_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

if (TARGET ${PROJECT_NAME})
    message("")
else()
    set(SOURCE_FILES
            map.hpp map.cpp mpl.hpp mpl.cpp tag.hpp tag.cpp set.hpp set.cpp tagset.cpp tagset.hpp)

   add_library(${PROJECT_NAME} ${SOURCE_FILES})
endif()
