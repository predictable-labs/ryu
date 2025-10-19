add_library(ryu_duckdb_catalog
        OBJECT
        duckdb_catalog.cpp
        duckdb_table_catalog_entry.cpp)

set(DUCKDB_EXTENSION_OBJECT_FILES
        ${DUCKDB_EXTENSION_OBJECT_FILES} $<TARGET_OBJECTS:ryu_duckdb_catalog>
        PARENT_SCOPE)
