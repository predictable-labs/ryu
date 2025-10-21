# CMakeLists.txt for libneo4j-omni (vendored)
cmake_minimum_required(VERSION 3.15)

# Find OpenSSL (required dependency for libneo4j-omni)
find_package(OpenSSL REQUIRED)

# Collect all source files
file(GLOB NEO4J_CLIENT_SOURCES
    lib/src/buffering_iostream.c
    lib/src/chunking_iostream.c
    lib/src/client_config.c
    lib/src/connection.c
    lib/src/deserialization.c
    lib/src/dotdir.c
    lib/src/error_handling.c
    lib/src/init.c
    lib/src/iostream.c
    lib/src/logging.c
    lib/src/memory.c
    lib/src/messages.c
    lib/src/metadata.c
    lib/src/network.c
    lib/src/openssl.c
    lib/src/openssl_iostream.c
    lib/src/posix_iostream.c
    lib/src/print.c
    lib/src/render.c
    lib/src/render_plan.c
    lib/src/render_results.c
    lib/src/result_stream.c
    lib/src/ring_buffer.c
    lib/src/serialization.c
    lib/src/thread.c
    lib/src/tofu.c
    lib/src/transaction.c
    lib/src/u8.c
    lib/src/uri.c
    lib/src/util.c
    lib/src/values.c
)

# Generate config.h from config.h.cmake.in
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    @ONLY
)

# Create static library
add_library(neo4j-client STATIC ${NEO4J_CLIENT_SOURCES})

# Set include directories - include the binary dir for generated config.h and source dir for headers
target_include_directories(neo4j-client PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add compile definition to include config.h
target_compile_definitions(neo4j-client PRIVATE HAVE_CONFIG_H)

# Link OpenSSL
target_link_libraries(neo4j-client PUBLIC OpenSSL::SSL OpenSSL::Crypto)

# Disable warnings for third-party code
if(NOT MSVC)
    target_compile_options(neo4j-client PRIVATE
        -w                              # Disable all warnings
        -Wno-error                      # Don't treat warnings as errors
        -Wno-error=format-truncation    # Specifically disable format-truncation error
        -Wno-format-truncation          # Disable format-truncation warning
    )
else()
    target_compile_options(neo4j-client PRIVATE /W0)
endif()

# Remove any -Werror flags that might be inherited
get_target_property(NEO4J_COMPILE_OPTIONS neo4j-client COMPILE_OPTIONS)
if(NEO4J_COMPILE_OPTIONS)
    list(FILTER NEO4J_COMPILE_OPTIONS EXCLUDE REGEX "-Werror")
    set_target_properties(neo4j-client PROPERTIES COMPILE_OPTIONS "${NEO4J_COMPILE_OPTIONS}")
endif()

# Set C standard to C11 (needed for _Static_assert)
set_target_properties(neo4j-client PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)
