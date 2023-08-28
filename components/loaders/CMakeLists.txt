add_library(remus__gltf_loader STATIC
    src/gltf_loader.cpp
)

target_include_directories(remus__gltf_loader
    PUBLIC
        include
)

target_include_directories(remus__gltf_loader
    PRIVATE
        include/loaders/models
)

target_link_libraries(remus__gltf_loader
    PUBLIC
        remus__core
        remus__scene_graph
        tinygltf)


if (REMUS_BUILD_TESTING)
    add_executable(remus__gltf_loader_tests tests/gltf_loader.test.cpp)
    target_link_libraries(remus__gltf_loader_tests PRIVATE remus__gltf_loader)
    configure_remus_test(remus__gltf_loader_tests)
endif()
