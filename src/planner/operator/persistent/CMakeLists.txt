add_library(ryu_planner_persistent
        OBJECT
        logical_copy_from.cpp
        logical_copy_to.cpp
        logical_insert.cpp
        logical_delete.cpp
        logical_merge.cpp
        logical_set.cpp
)

set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:ryu_planner_persistent>
        PARENT_SCOPE)
