# FindDuckDB.cmake - Find DuckDB library and headers
#
# This module defines:
#  DuckDB_FOUND - system has DuckDB
#  DuckDB_INCLUDE_DIRS - the DuckDB include directories
#  DuckDB_LIBRARIES - link these to use DuckDB
#  DuckDB_VERSION - the version of DuckDB found

find_path(DuckDB_INCLUDE_DIR
    NAMES duckdb.h duckdb.hpp
    PATHS
        /usr/local/include
        /usr/include
        ${DuckDB_DIR}/include
        $ENV{DuckDB_DIR}/include
        "C:/Program Files/duckdb/include"
    DOC "DuckDB include directory"
)

if(DuckDB_USE_STATIC_LIBS)
    find_library(DuckDB_LIBRARY
        NAMES libduckdb_static.a duckdb_static duckdb
        PATHS
            /usr/local/lib
            /usr/lib
            ${DuckDB_DIR}/lib
            $ENV{DuckDB_DIR}/lib
            "C:/Program Files/duckdb/lib"
        DOC "DuckDB static library"
    )
else()
    find_library(DuckDB_LIBRARY
        NAMES duckdb libduckdb.so libduckdb.dylib
        PATHS
            /usr/local/lib
            /usr/lib
            ${DuckDB_DIR}/lib
            $ENV{DuckDB_DIR}/lib
            "C:/Program Files/duckdb/lib"
        DOC "DuckDB shared library"
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DuckDB
    REQUIRED_VARS DuckDB_LIBRARY DuckDB_INCLUDE_DIR
)

if(DuckDB_FOUND)
    set(DuckDB_LIBRARIES ${DuckDB_LIBRARY})
    set(DuckDB_INCLUDE_DIRS ${DuckDB_INCLUDE_DIR})

    mark_as_advanced(DuckDB_INCLUDE_DIR DuckDB_LIBRARY)

    if(NOT TARGET DuckDB::DuckDB)
        add_library(DuckDB::DuckDB UNKNOWN IMPORTED)
        set_target_properties(DuckDB::DuckDB PROPERTIES
            IMPORTED_LOCATION "${DuckDB_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${DuckDB_INCLUDE_DIR}"
        )
    endif()
endif()
