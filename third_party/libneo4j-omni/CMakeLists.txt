# CMakeLists.txt for libneo4j-omni (vendored)
cmake_minimum_required(VERSION 3.15)

# Required for feature checks
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCSourceCompiles)

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

# Feature detection for config.h
check_include_file(endian.h HAVE_ENDIAN_H)
check_include_file(sys/endian.h HAVE_SYS_ENDIAN_H)
check_include_file(libkern/OSByteOrder.h HAVE_LIBKERN_OSBYTEORDER_H)
check_include_file(stdatomic.h HAVE_STDATOMIC_H)
check_include_file(stdbool.h HAVE_STDBOOL_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdio.h HAVE_STDIO_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(langinfo.h HAVE_LANGINFO_H)
check_include_file(dlfcn.h HAVE_DLFCN_H)

# Check for byte order functions
set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
if(HAVE_ENDIAN_H)
    set(CMAKE_REQUIRED_INCLUDES endian.h)
endif()

check_symbol_exists(htobe64 "endian.h" HAVE_HTOBE64)
check_symbol_exists(be64toh "endian.h" HAVE_BE64TOH)
check_symbol_exists(htonll "arpa/inet.h" HAVE_HTONLL)
check_symbol_exists(ntohll "arpa/inet.h" HAVE_NTOHLL)

# Check for bswap_64
check_c_source_compiles("
#include <byteswap.h>
int main() { unsigned long long x = 1; bswap_64(x); return 0; }
" HAVE_BSWAP_64)

# Check for OSSwap functions (macOS)
if(HAVE_LIBKERN_OSBYTEORDER_H)
    check_symbol_exists(OSSwapHostToBigInt64 "libkern/OSByteOrder.h" HAVE_OSSWAPHOSTTOBIGINT64)
    check_symbol_exists(OSSwapBigToHostInt64 "libkern/OSByteOrder.h" HAVE_OSSWAPBIGTOHOSTINT64)
endif()

# Check for other functions
check_symbol_exists(memset_s "string.h" HAVE_MEMSET_S)
check_symbol_exists(mkstemp "stdlib.h" HAVE_MKSTEMP)
check_symbol_exists(open_memstream "stdio.h" HAVE_OPEN_MEMSTREAM)

# OpenSSL functions
check_symbol_exists(BIO_meth_new "openssl/bio.h" HAVE_BIO_METH_NEW)
check_symbol_exists(ASN1_STRING_get0_data "openssl/asn1.h" HAVE_ASN1_STRING_GET0_DATA)
check_symbol_exists(CRYPTO_set_locking_callback "openssl/crypto.h" HAVE_CRYPTO_SET_LOCKING_CALLBACK)

# Set OpenSSL flag
set(HAVE_OPENSSL 1)

# Set pthread flag (assuming pthreads are available on Linux)
set(HAVE_PTHREADS 1)
set(HAVE_PTHREAD_PRIO_INHERIT 1)

# Package info for config.h
set(PACKAGE_NAME "libneo4j-client")
set(PACKAGE_STRING "libneo4j-client 2.2.0")
set(PACKAGE_TARNAME "libneo4j-client")
set(PACKAGE_URL "")
set(PACKAGE_VERSION "2.2.0")
set(VERSION "2.2.0")

# Generate config.h from config.h.cmake.in into the source directory
# (source files use relative includes like "../../config.h")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake.in
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h
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
