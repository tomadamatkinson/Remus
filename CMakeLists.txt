cmake_minimum_required(VERSION 3.22)
project(remus VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(REMUS_BUILD_TESTING "Build testing" OFF)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if (BUILD_TESTING)
        set(REMUS_BUILD_TESTING ON)
    endif()
endif()

add_subdirectory(cmake)
add_subdirectory(third_party)
add_subdirectory(components)
add_subdirectory(cmd)
