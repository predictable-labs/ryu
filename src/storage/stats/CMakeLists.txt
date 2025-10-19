add_library(ryu_storage_stats
        OBJECT
        column_stats.cpp
        hyperloglog.cpp
        table_stats.cpp)

set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:ryu_storage_stats>
        PARENT_SCOPE)
