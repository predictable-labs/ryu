add_library(ryu_json_type
        OBJECT
        json_type.cpp)

set(JSON_EXTENSION_OBJECT_FILES
        ${JSON_EXTENSION_OBJECT_FILES} $<TARGET_OBJECTS:ryu_json_type>
        PARENT_SCOPE)
