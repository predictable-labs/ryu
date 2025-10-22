add_library(ryu_storage_backup
        OBJECT
        backup_manager.cpp
        backup_metadata.cpp
        backup_shadow_file.cpp)

set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:ryu_storage_backup>
        PARENT_SCOPE)
