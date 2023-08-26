
macro(configure_remus_executable)
    message(STATUS "Executable ${ARGV0}")
    set_target_properties(${ARGV0} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
    )
endmacro(configure_remus_executable)

add_custom_target(remus__components)

macro(configure_remus_library)
    message(STATUS "Library ${ARGV0}")
    set_target_properties(${ARGV0} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
    )
    add_dependencies(remus__components ${ARGV0})
endmacro(configure_remus_library)

add_custom_target(remus__tests)

macro(configure_remus_test)
    message(STATUS "Test ${ARGV0}")
    set_target_properties(${ARGV0} PROPERTIES 
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
        CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
    )
    target_link_libraries(${ARGV0} PRIVATE Catch2::Catch2WithMain)
    add_test(NAME ${ARGV0} COMMAND ${ARGV0})
    add_dependencies(remus__tests ${ARGV0})
endmacro(configure_remus_test)
