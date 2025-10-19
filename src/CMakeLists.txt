include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Have to pass this down to every subdirectory, which actually adds the files.
# This doesn't affect parent directories.
add_compile_definitions(RYU_EXPORTS)
add_compile_definitions(ANTLR4CPP_STATIC)
add_subdirectory(binder)
add_subdirectory(c_api)
add_subdirectory(catalog)
add_subdirectory(common)
add_subdirectory(expression_evaluator)
add_subdirectory(function)
add_subdirectory(graph)
add_subdirectory(main)
add_subdirectory(optimizer)
add_subdirectory(parser)
add_subdirectory(planner)
add_subdirectory(processor)
add_subdirectory(storage)
add_subdirectory(transaction)
add_subdirectory(extension)

add_library(ryu STATIC ${ALL_OBJECT_FILES})
add_library(ryu_shared SHARED ${ALL_OBJECT_FILES})

set(RYU_LIBRARIES antlr4_cypher antlr4_runtime brotlidec brotlicommon fast_float utf8proc re2 fastpfor parquet snappy thrift yyjson zstd miniz mbedtls lz4 roaring_bitmap simsimd)
if (NOT __SINGLE_THREADED__)
        set(RYU_LIBRARIES ${RYU_LIBRARIES} Threads::Threads)
endif()
if(NOT WIN32)
        set(RYU_LIBRARIES dl ${RYU_LIBRARIES})
endif()
# Seems to be needed for clang on linux only
# for compiling std::atomic<T>::compare_exchange_weak
if ((NOT APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND NOT __WASM__ AND NOT __SINGLE_THREADED__)
        set(RYU_LIBRARIES atomic ${RYU_LIBRARIES})
endif()
if (ENABLE_BACKTRACES)
        set(RYU_LIBRARIES ${RYU_LIBRARIES} cpptrace::cpptrace)
endif()
target_link_libraries(ryu PUBLIC ${RYU_LIBRARIES})
target_link_libraries(ryu_shared PUBLIC ${RYU_LIBRARIES})
unset(RYU_LIBRARIES)

set(RYU_INCLUDES $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include/c_api)
target_include_directories(ryu PUBLIC ${RYU_INCLUDES})
target_include_directories(ryu_shared PUBLIC ${RYU_INCLUDES})
unset(RYU_INCLUDES)

if(WIN32)
        # Anything linking against the static library must not use dllimport.
        target_compile_definitions(ryu INTERFACE RYU_STATIC_DEFINE)
endif()

if(NOT WIN32)
        set_target_properties(ryu_shared PROPERTIES OUTPUT_NAME ryu)
endif()

install(TARGETS ryu ryu_shared)

if(${BUILD_SINGLE_FILE_HEADER})
        # Create a command to generate ryu.hpp, and then create a target that is
        # always built that depends on it. This allows our generator to detect when
        # exactly to build ryu.hpp, while still building the target by default.
        find_package(Python3 3.9...4 REQUIRED)
        add_custom_command(
                OUTPUT ryu.hpp
                COMMAND
                        ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/scripts/collect-single-file-header.py ${CMAKE_CURRENT_BINARY_DIR}/..
                DEPENDS
                        ${PROJECT_SOURCE_DIR}/scripts/collect-single-file-header.py ryu_shared)
        add_custom_target(single_file_header ALL DEPENDS ryu.hpp)
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/c_api/ryu.h TYPE INCLUDE)

if(${BUILD_SINGLE_FILE_HEADER})
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ryu.hpp  TYPE INCLUDE)
endif()
