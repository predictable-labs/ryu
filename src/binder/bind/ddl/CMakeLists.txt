add_library(ryu_binder_bind_ddl
        OBJECT
        bound_create_table_info.cpp)

set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:ryu_binder_bind_ddl>
        PARENT_SCOPE)
