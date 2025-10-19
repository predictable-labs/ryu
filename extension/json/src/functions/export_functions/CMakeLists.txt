add_library(ryu_json_export_functions
        OBJECT
        json_export.cpp)

set(JSON_EXTENSION_OBJECT_FILES
        ${JSON_EXTENSION_OBJECT_FILES} $<TARGET_OBJECTS:ryu_json_export_functions>
        PARENT_SCOPE)
