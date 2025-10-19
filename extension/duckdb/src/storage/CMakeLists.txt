add_library(ryu_duckdb_storage
        OBJECT
        duckdb_storage.cpp)

set(DUCKDB_EXTENSION_OBJECT_FILES
        ${DUCKDB_EXTENSION_OBJECT_FILES} $<TARGET_OBJECTS:ryu_duckdb_storage>
        PARENT_SCOPE)
