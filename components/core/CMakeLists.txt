add_library(remus__core INTERFACE)

target_include_directories(remus__core INTERFACE include)

target_link_libraries(
    remus__core
        INTERFACE
            spdlog::spdlog
            glm
)

configure_remus_library(remus__core)

if(REMUS_BUILD_TESTING)
    add_executable(remus__core_tests
        tests/channel.test.cpp
        tests/event_bus.test.cpp
    )
    target_link_libraries(remus__core_tests PRIVATE
        remus__core
    )

    configure_remus_test(remus__core_tests)
endif()
