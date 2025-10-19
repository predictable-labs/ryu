add_library(ryu_processor_operator_create_macro
        OBJECT
        create_macro.cpp)

set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:ryu_processor_operator_create_macro>
        PARENT_SCOPE)
