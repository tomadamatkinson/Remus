


add_library(remus__core INTERFACE)

target_include_directories(remus__core INTERFACE include)

target_link_libraries(
    remus__core
        INTERFACE
            spdlog::spdlog
)

configure_remus_library(remus__core)