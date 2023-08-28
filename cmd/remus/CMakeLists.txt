
add_executable(remus src/main.cpp)

target_link_libraries(remus
    PRIVATE
        remus__core
        remus__platform)

configure_remus_executable(remus)
