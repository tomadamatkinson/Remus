
add_executable(remus src/main.cpp)
target_link_libraries(remus PRIVATE remus__core)
set_target_properties(remus PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
