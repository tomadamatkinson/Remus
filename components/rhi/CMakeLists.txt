add_library(remus__rhi_opengl
    STATIC
        src/opengl/buffers.cpp
        src/opengl/primitives.cpp
        src/opengl/rhi.cpp
        src/opengl/shaders.cpp)

target_include_directories(remus__rhi_opengl
    PUBLIC
        include)

target_link_libraries(remus__rhi_opengl PUBLIC remus__core glad)

add_library(remus__rhi INTERFACE)

target_link_libraries(remus__rhi INTERFACE remus__rhi_opengl)
