add_library(remus__platform_headers INTERFACE)
target_include_directories(remus__platform_headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(
    remus__desktop_platform
        STATIC
            src/desktop_platform.cpp
            src/glfw_window.cpp)

target_include_directories(
    remus__desktop_platform
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/include/platforms)

target_link_libraries(remus__desktop_platform
    PUBLIC
        remus__platform_headers
        remus__core
        glfw
        glad)

add_library(remus__platform INTERFACE)
target_link_libraries(remus__platform INTERFACE remus__platform_headers remus__desktop_platform)
