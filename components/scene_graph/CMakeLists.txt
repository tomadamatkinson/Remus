
add_library(
    remus__scene_graph
        STATIC
            src/scene_graph.cpp
        )

target_include_directories(remus__scene_graph PUBLIC include/)
target_include_directories(remus__scene_graph PRIVATE include/scene_graph/)

target_link_libraries(
    remus__scene_graph
        PUBLIC
            remus__core
            EnTT::EnTT
            )

if(REMUS_BUILD_TESTING)
    add_executable(remus__scene_graph_tests
        tests/node.test.cpp
        tests/system.test.cpp
    )
    target_link_libraries(remus__scene_graph_tests PRIVATE
        remus__scene_graph
    )

    configure_remus_test(remus__scene_graph_tests)
endif()